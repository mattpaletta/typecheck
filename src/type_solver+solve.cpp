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

    constexpr auto init_val = std::numeric_limits<std::size_t>::max();
	this->DoPass_internal(pass, unresolvedConstraints, manager, init_val, init_val);
}

void typecheck::TypeSolver::DoPass_internal(typecheck::ConstraintPass* pass, std::deque<std::size_t>/* this is a copy */ indexes, const TypeManager* manager, const std::size_t& prev_failed, const std::size_t& prev_emplaced) const {
    auto best_pass = pass->CreateCopy();
    
    if (!indexes.empty()) {
        // Create an original copy, used for score.
        auto original_indices = indexes;
        std::size_t i = indexes.front();
        auto* current_constraint = &manager->constraints.at(i);
        indexes.pop_front();

        auto iterPass = pass->CreateCopy();

        bool did_any_resolve = false;
        while (iterPass.GetResolver(*current_constraint, manager)->hasMoreSolutions(*current_constraint, manager)) {
            const auto did_resolve = iterPass.GetResolver(*current_constraint, manager)->resolveNext(*current_constraint, manager);
            auto computed = iterPass.CreateCopy();
            if (did_resolve) {
                did_any_resolve = true;
                this->DoPass_internal(&computed, indexes, manager, prev_failed, prev_emplaced);
            } else if (indexes.size() > 1 && prev_failed != i && prev_emplaced == std::numeric_limits<std::size_t>::max()) {
                auto new_list = indexes;
                // It failed, add it to the end of the queue
                new_list.push_back(i);

                this->DoPass_internal(&computed, new_list, manager, prev_failed == std::numeric_limits<std::size_t>::max() ? i : prev_failed, prev_emplaced);
            } else {
                // We failed the current one twice, not valid
                break;
            }

            if (typecheck::ConstraintPass::IsScoreBetter(computed.CalcScoreMap(original_indices, manager), best_pass.CalcScoreMap(original_indices, manager, false))) {
                computed.CopyToExisting(&best_pass);
            }
        }

        if (!did_any_resolve && prev_emplaced != i) {
            if (original_indices.size() > 1) {
                auto computed = pass->CreateCopy();

                auto new_list = indexes;
                // It failed, add it to the end of the queue
                new_list.push_back(i);

                this->DoPass_internal(&computed, new_list, manager, prev_failed, prev_emplaced == std::numeric_limits<std::size_t>::max() ? i : prev_emplaced);

                if (typecheck::ConstraintPass::IsScoreBetter(computed.CalcScoreMap(new_list, manager), best_pass.CalcScoreMap(new_list, manager, false))) {
                    computed.CopyToExisting(&best_pass);
                }
            }
        }
    }

    // Merge best_pass in 'pass'
    best_pass.CopyToExisting(pass);
}
