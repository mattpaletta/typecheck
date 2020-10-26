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

using namespace typecheck;

void TypeSolver::DoPass(ConstraintPass* pass, const TypeManager* manager) const {
	std::deque<std::size_t> unresolvedConstraints;

	// Build queue of constraints indexes
	for (std::size_t i = 0; i < manager->constraints.size(); ++i) {
		unresolvedConstraints.push_back(i);
	}

    constexpr auto init_val = std::numeric_limits<std::size_t>::max();
	this->DoPass_internal(pass, unresolvedConstraints, manager, init_val, init_val);
}

void TypeSolver::DoPass_internal(ConstraintPass* pass, std::deque<std::size_t>/* this is a copy */ indexes, const TypeManager* manager, const std::size_t& prev_failed, const std::size_t& prev_emplaced) const {
    auto best_pass = pass->CreateCopy();
    auto iterPass = pass->CreateCopy();

    if (!indexes.empty()) {
        // Create an original copy, used for score.
        auto original_indices = indexes;
        std::size_t i = 0;
        const Constraint* current_constraint = nullptr;

        std::size_t skippedItems = 0;

        for (auto& j : indexes) {
            // Skip ahead until we find one that's ready.
            current_constraint = &manager->constraints.at(j);

            if (iterPass.GetResolver(*current_constraint, manager)->hasMoreSolutions(*current_constraint, manager) && iterPass.GetResolver(*current_constraint, manager)->readyToResolve(*current_constraint, manager)) {
                i = j;
                break;
            }

            ++skippedItems;
        }

        // For Undefined Behavior
        if (skippedItems >= indexes.size()) {
            return;
        } else {
            // Add all the ones we skipped back to the front
            indexes.erase(indexes.begin() + static_cast<long>(skippedItems));
        }

        if (current_constraint == nullptr) {
            // Got an invalid result, exit early.
            return;
        }

        bool did_any_resolve = false;
        // Early stop if best_score reaches zero
        while (best_pass.CalcScore(original_indices, manager) > 0 && iterPass.GetResolver(*current_constraint, manager)->hasMoreSolutions(*current_constraint, manager)) {
            const auto did_resolve = iterPass.GetResolver(*current_constraint, manager)->resolveNext(*current_constraint, manager);
            auto computed = iterPass.CreateCopy();
            if (did_resolve) {
                did_any_resolve = true;
                if (prev_failed == i) {
                    // It was the last one to fail, but we resolved it.
                    this->DoPass_internal(&computed, indexes, manager, std::numeric_limits<std::size_t>::max(), prev_emplaced);
                } else {
                    this->DoPass_internal(&computed, indexes, manager, prev_failed, prev_emplaced);
                }
            } else if (indexes.size() > 1 && prev_failed != i && prev_emplaced == std::numeric_limits<std::size_t>::max()) {
                auto new_list = indexes;
                // It failed, add it to the end of the queue
                new_list.push_back(i);

                this->DoPass_internal(&computed, new_list, manager, prev_failed == std::numeric_limits<std::size_t>::max() ? i : prev_failed, prev_emplaced);
            } else {
                // We failed the current one twice, not valid
                break;
            }

            if (ConstraintPass::IsScoreBetter(computed.CalcScoreMap(original_indices, manager), best_pass.CalcScoreMap(original_indices, manager, false))) {
                computed.MoveToExisting(&best_pass);
            }
        }

        // Incur no penalty if not ready to resolve.
        if (!did_any_resolve && prev_emplaced != i) {
            if (original_indices.size() > 1) {
                auto computed = pass->CreateCopy();

                auto new_list = indexes;
                // It failed, add it to the end of the queue
                new_list.push_back(i);

                // Reset this if the 'prev_emplaced' isn't in the list
                auto contains_last = false;
                for (auto& contains : new_list) {
                    if (contains == prev_emplaced) {
                        contains_last = true;
                        break;
                    }
                }

                // Said it was ready, and then nothing resolved, so treat as failure
                this->DoPass_internal(&computed, new_list, manager, prev_failed, (!contains_last ||  prev_emplaced == std::numeric_limits<std::size_t>::max()) ? i : prev_emplaced);

                if (ConstraintPass::IsScoreBetter(computed.CalcScoreMap(new_list, manager), best_pass.CalcScoreMap(new_list, manager, false))) {
                    computed.MoveToExisting(&best_pass);
                }
            }
        }
    }

    // Merge best_pass in 'pass'
    best_pass.MoveToExisting(pass);
}
