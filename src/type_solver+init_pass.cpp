#include "typecheck/type_solver.hpp"
#include "typecheck/resolver.hpp"
#include "typecheck/type_manager.hpp"

#include <google/protobuf/util/message_differencer.h>
#include <algorithm>
#include <numeric>
#include <set>

void typecheck::TypeSolver::RemoveDuplicates(typecheck::TypeManager* manager) {
	std::set<std::size_t> duplicates;
	// Only check items to the left, meaning we'll leave the original, but move consequent items.
	for (std::size_t i = 0; i < manager->constraints.size(); ++i) {
		for (std::size_t j = 0; i < j; ++j) {
			if (duplicates.find(j) == duplicates.end() && google::protobuf::util::MessageDifferencer::Equals(manager->constraints.at(i), manager->constraints.at(j))) {
				duplicates.insert(j);
				break;
			} else if (duplicates.find(j) != duplicates.end()) {
				// Once we know it's a duplicate, just continue to the next item
				break;
			}
		}
	}

	std::vector<Constraint> newWithoutDuplicates;
	for (std::size_t i = 0; i < manager->constraints.size(); ++i) {
		if (duplicates.find(i) == duplicates.end()) {
			newWithoutDuplicates.push_back(manager->constraints.at(i));
		}
	}
	manager->constraints = std::move(newWithoutDuplicates);
}
