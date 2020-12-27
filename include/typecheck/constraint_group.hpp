#pragma once

#include "constraint_pass.hpp"

#include <unordered_set>
#include <vector>

namespace typecheck {
	class ConstraintGroup {
	public:
		ConstraintGroup() = default;
		virtual ~ConstraintGroup() = default;

		bool contains(const ConstraintPass::IDType constraint) const;
		void add(const ConstraintPass::IDType constraint);
		void remove(const ConstraintPass::IDType constraint);

		std::vector<ConstraintPass::IDType> toList() const;
		std::size_t size() const;

	private:
		std::unordered_set<ConstraintPass::IDType> _constraints;
	};
}
