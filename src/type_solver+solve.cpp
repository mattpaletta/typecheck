#include "type_solver.hpp"
#include "type_manager.hpp"
#include "resolver.hpp"

#include "protocols/ExpressibleByIntegerLiteral.hpp"
#include "protocols/ExpressibleByFloatLiteral.hpp"
#include "protocols/ExpressibleByDoubleLiteral.hpp"

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
	// Set it to initially invalid, until proven otherwise.
	best_pass.is_valid = false;

	if (indexes.size() == 0) {
		// If no nodes, return!
		return;
	}

	const auto i = indexes.front();
	auto& current_constraint = manager->constraints.at(i);
	indexes.pop_front();

	typecheck::ConstraintPass iterPass = pass->CreateCopy();

	// If we have a perfect score, stop searching
	while (best_pass.score > 0 && iterPass.GetResolver(current_constraint, manager)->hasMoreSolutions(current_constraint, manager)) {
		if (iterPass.GetResolver(current_constraint, manager)->resolveNext(current_constraint, manager)) {
			this->DoPass_internal(&iterPass, indexes, manager, prev_failed);
		} else {
			// It failed, add it to the end of the queue
			indexes.push_back(i);
			if (prev_failed == std::numeric_limits<std::size_t>::max() /* default if nothing failed */) {
				// Set i as the first thing that failed
				this->DoPass_internal(&iterPass, indexes, manager, i);
			} else if (prev_failed == i) {
				// We failed this same constraint twice
				// Abort!
				pass->is_valid = false;
				break;
			}
		}

		auto best_score = best_pass.score;
		if (iterPass.CalcScore(manager) < best_pass.score) {
			best_pass = std::move(iterPass);
		}
	}

	// Merge best_pass in 'pass'
	best_pass.CopyToExisting(pass);
}