#include "typecheck/type_solver.hpp"
#include "typecheck/type_manager.hpp"
#include "typecheck/resolver.hpp"
#include "typecheck/debug.hpp"

#include "typecheck/protocols/ExpressibleByIntegerLiteral.hpp"
#include "typecheck/protocols/ExpressibleByFloatLiteral.hpp"
#include "typecheck/protocols/ExpressibleByDoubleLiteral.hpp"

#include <set>
#include <vector>
#include <iostream>
#include <deque>


void typecheck::TypeSolver::DoPass(ConstraintPass* pass, const TypeManager* manager) const {
	std::deque<std::size_t> unresolvedConstraints;

	// Build queue of constraints indexes
	for (std::size_t i = 0; i < manager->constraints.size(); ++i) {
		unresolvedConstraints.push_back(i);
	}

	this->DoPass_internal(pass, unresolvedConstraints, manager);
}

void typecheck::TypeSolver::DoPass_internal(typecheck::ConstraintPass* pass, std::deque<std::size_t>/* this is a copy */ indexes, const TypeManager* manager, const std::size_t& prev_failed) const {
    auto best_pass = pass->CreateCopy();
    
    if (!indexes.empty()) {
        // Create an original copy, used for score.
        auto original_indices = indexes;
        auto i = indexes.front();
        auto* current_constraint = &manager->constraints.at(i);
        indexes.pop_front();

        auto iterPass = pass->CreateCopy();

        // If we have a perfect score, stop searching
        while (iterPass.GetResolver(*current_constraint, manager)->hasMoreSolutions(*current_constraint, manager)) {
            const auto did_resolve = iterPass.GetResolver(*current_constraint, manager)->resolveNext(*current_constraint, manager);
            auto computed = iterPass.CreateCopy();
            if (did_resolve) {
                this->DoPass_internal(&computed, indexes, manager, prev_failed);
            } else if (indexes.size() > 1 && prev_failed != i) {
                auto new_list = indexes;
                // It failed, add it to the end of the queue
                new_list.push_back(i);

                this->DoPass_internal(&computed, new_list, manager, prev_failed == std::numeric_limits<std::size_t>::max() ? i : prev_failed);
            } else {
                // We failed the current one twice, not valid
                break;
            }

            if (computed.CalcScore(original_indices, manager) < best_pass.CalcScore(original_indices, manager, true)) {
                computed.CopyToExisting(&best_pass);
            }
        }
    }

    // Merge best_pass in 'pass'
    best_pass.CopyToExisting(pass);
}
