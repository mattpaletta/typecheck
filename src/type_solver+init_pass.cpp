#include <typecheck/type_solver.hpp>
#include <typecheck/resolver.hpp>
#include <typecheck/type_manager.hpp>
#include <typecheck/debug.hpp>
#include <typecheck/type_var.hpp>

#include <algorithm>
#include <numeric>
#include <set>
#include <iostream>

using namespace typecheck;

namespace {
	auto GetSymbolsExplicit(const Constraint::ExplicitType& _explicit) -> std::vector<TypeVar> {
		if (_explicit.has_var()) {
			return {_explicit.var()};
		}

		return {};
	}

	auto GetSymbolsOverload(const Constraint::Overload& _overload) -> std::vector<TypeVar> {
		std::vector<TypeVar> out;
		if (_overload.has_type()) {
			out.push_back(_overload.type());
		}

		out.push_back(_overload.returnvar());
		for (int i = 0; i < _overload.argvars_size(); ++i) {
			out.push_back(_overload.argvars(i));
		}

		return out;
	}

	auto GetSymbolsConforms(const Constraint::Conforms& _conforms) -> std::vector<TypeVar> {
		if (_conforms.has_type()) {
			return {_conforms.type()};
		}

		return {};
	}

	auto GetSymbolsTypes(const Constraint::Types& _types) -> std::vector<TypeVar> {
		std::vector<TypeVar> out;
		if (_types.has_first()) {
			out.push_back(_types.first());
		}

		if (_types.has_second()) {
			out.push_back(_types.second());
		}

		if (_types.has_third()) {
			out.push_back(_types.third());
		}
		return out;
	}

	auto GetConstraintSymbols(const Constraint& constraint) -> std::vector<TypeVar> {
		if (constraint.has_explicit_()) {
			return GetSymbolsExplicit(constraint.explicit_());
		} else if (constraint.has_overload()) {
			return GetSymbolsOverload(constraint.overload());
		} else if (constraint.has_conforms()) {
			return GetSymbolsConforms(constraint.conforms());
		} else if (constraint.has_types()) {
			return GetSymbolsTypes(constraint.types());
		} else {
			TYPECHECK_ASSERT(false, "Don't know how to extract symbols from constraint.");
		}

		return {};
	}

	auto MergeGroups(const ConstraintGroup& g1, const ConstraintGroup& g2) -> ConstraintGroup {
		ConstraintGroup g3 = g1;
		for (const auto id : g2.toList()) {
			g3.add(id);
		}
		return g3;
	}

	void EmptyGroup(ConstraintGroup* group) {
		const auto existingConstraints= group->toList();
		for (const auto& id : existingConstraints) {
			group->remove(id);
		}
	}
}

auto TypeSolver::SplitToGroups(const typecheck::TypeManager* manager) const -> std::vector<ConstraintGroup> {
	std::vector<ConstraintGroup> groups;

	// Store which group each symbol is part of
	// If we hit that symbol again, we put it into the same group.
	// Combine groups if necessary
	std::map<TypeVar, std::size_t> symbolGroup;
	for (const auto& constraint : manager->constraints) {
		const auto symbols = GetConstraintSymbols(constraint);

		std::unordered_set<std::size_t> constraintGroups;
		for (const auto& sym : symbols) {

			// Lookup existing group
			const auto it = symbolGroup.find(sym);
			if (it != symbolGroup.end()) {
				// We have a record for it
				constraintGroups.insert(it->second);
			}
		}

		if (constraintGroups.empty()) {
			// It fit into no existing groups, create a new one
			groups.emplace_back();
			const auto newGroupId = groups.size() - 1;
			for (const auto& sym : symbols) {
				symbolGroup[sym] = newGroupId;
			}
			groups.at(newGroupId).add(constraint.id());
		} else if (constraintGroups.size() == 1) {
			// It fits into exactly one bucket, so add it
			const auto groupId = *(constraintGroups.begin());
			for (const auto& sym : symbols) {
				symbolGroup[sym] = groupId;
			}
			groups.at(groupId).add(constraint.id());
		} else {
			// It fits to multiple buckets, merge them
			// Merge into the first group, deleting as we go.
			const auto firstGroupId = *constraintGroups.begin();
			auto firstGroup = groups.at(firstGroupId);

			for (const auto group : constraintGroups) {
				firstGroup = MergeGroups(firstGroup, groups.at(group));

				// Delete it so, we don't have duplicate id's
				EmptyGroup(&groups.at(group));
			}
			groups.at(firstGroupId) = firstGroup;
			groups.at(firstGroupId).add(constraint.id());

			// Update symbol to point to new group
			for (const auto& sym : symbols) {
				symbolGroup[sym] = firstGroupId;
			}
		}
	}

	std::vector<ConstraintGroup> finalGroups;
	for (const auto& gr : groups) {
		if (gr.size() > 0) {
			finalGroups.push_back(gr);
		}
	}

	return finalGroups;
}

void TypeSolver::RemoveDuplicates(TypeManager* manager) const {
	std::set<std::size_t> duplicates;
	// Only check items to the left, meaning we'll leave the original, but move consequent items.
	for (std::size_t i = 0; i < manager->constraints.size(); ++i) {
		for (std::size_t j = 0; i < j; ++j) {
			if (duplicates.find(j) == duplicates.end() && proto_equal(manager->constraints.at(i), manager->constraints.at(j))) {
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
