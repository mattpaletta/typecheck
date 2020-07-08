#include "type_solver.hpp"

#include <google/protobuf/util/message_differencer.h>
#include <algorithm>
#include <set>

void typecheck::TypeSolver::RemoveDuplicates() {
	std::set<std::size_t> duplicates;
	// Only check items to the left, meaning we'll leave the original, but move consequent items.
	for (std::size_t i = 0; i < this->constraints.size(); ++i) {
		for (std::size_t j = 0; i < j; ++j) {
			if (duplicates.find(j) == duplicates.end() && google::protobuf::util::MessageDifferencer::Equals(this->constraints.at(i), this->constraints.at(j))) {
				duplicates.insert(j);
				break;
			} else if (duplicates.find(j) != duplicates.end()) {
				// Once we know it's a duplicate, just continue to the next item
				break;
			}
		}
	}

	std::vector<Constraint> newWithoutDuplicates;
	for (std::size_t i = 0; i < this->constraints.size(); ++i) {
		if (duplicates.find(i) == duplicates.end()) {
			newWithoutDuplicates.push_back(this->constraints.at(i));
		}
	}
	this->constraints = newWithoutDuplicates;
}

void typecheck::TypeSolver::FindOverloads() {
	// Arrange these into a map, so we can quickly get all of the overloads available for a function.
	for (const auto& func : this->functions) {
		this->funcOverloads[func.name()].push_back(func);
	}
}

void typecheck::TypeSolver::BuildRefGraph() {

}