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
    auto iterPass = pass->CreateCopy();

    auto best_pass = pass->CreateCopy();

    if (indexes.size() == 0) {
		// If no nodes, return!
		return;
	}

    // Set it to initially invalid, until proven otherwise.
    // best_pass.is_valid = false;

    // Create an original copy, used for score.
    auto iter_indexes = indexes;
	auto i = indexes.front();
	auto* current_constraint = &manager->constraints.at(i);
    std::cout << "Solving constraint: " << i << std::endl;
	indexes.pop_front();

	// If we have a perfect score, stop searching
    while (/*best_pass.CalcScore(manager, true) > 0 && */ iterPass.GetResolver(*current_constraint, manager)->hasMoreSolutions(*current_constraint, manager)) {
        const auto did_resolve = iterPass.GetResolver(*current_constraint, manager)->resolveNext(*current_constraint, manager);
        if (did_resolve) {
            this->DoPass_internal(&iterPass, indexes, manager, prev_failed);
        }
        /*if (did_resolve && iterPass.IsValid(manager)) {
            // This one got resolved successfully, but the child did not
            // don't mark it as failed, but try again with this one at the end
            // Switch to a different constraint, push old i first

            indexes.push_back(i);
            iterPass.ResetResolver(*current_constraint, manager);

            i = indexes.front();

            current_constraint = &manager->constraints.at(i);
            indexes.pop_front();

            this->DoPass_internal(&iterPass, indexes, manager, prev_failed);
		} else */ if (!did_resolve || !iterPass.IsValid(manager)) {
            // iterPass.ResetResolver(*current_constraint, manager);
            
			// It failed, add it to the end of the queue
			indexes.push_back(i);
			if (prev_failed == std::numeric_limits<std::size_t>::max() /* default if nothing failed */) {
                std::cout << "Failed constraint: " << i << std::endl;
				// Set i as the first thing that failed
				this->DoPass_internal(&iterPass, indexes, manager, i);
			} else if (prev_failed == i) {
                std::cout << "Failed constraint: " << i << "twice, aborting" << std::endl;
				// We failed this same constraint twice
				// Abort!
				pass->is_valid = false;
				break;
			}
            indexes.pop_back();
		}

        if (iterPass.CalcScore(iter_indexes, manager) < best_pass.CalcScore(iter_indexes, manager, true)) {
			iterPass.CopyToExisting(&best_pass);
		}
        std::cout << "Finished iteration, current score: " << iterPass.CalcScore(iter_indexes, manager, false) << std::endl;
	}

	// Merge best_pass in 'pass'
    //if (iterPass.CalcScore(iter_indexes, manager) < best_pass.CalcScore(iter_indexes, manager, true)) {
        best_pass.CopyToExisting(pass);
    //}
    std::cout << "Finished constraint, current best: " << best_pass.CalcScore(iter_indexes, manager, true) << std::endl;
}
