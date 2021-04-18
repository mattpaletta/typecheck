#pragma once

#include "constraint.hpp"
#include "constraint_pass.hpp"
#include "constraint_group.hpp"

#include <vector>
#include <map>
#include <deque>
#include <numeric>

namespace typecheck {
	class TypeManager;

	class TypeSolver {
	public:
        friend TypeManager;
        
		TypeSolver();
		virtual ~TypeSolver() = default;

		bool solve(const TypeManager* manager);
		Type getResolvedType(const TypeVar& _typeVar) const;

	protected:
		// Implementations found in `type_solver+init_pass.cpp`
		virtual std::vector<ConstraintGroup> SplitToGroups(const typecheck::TypeManager* manager) const;
        virtual void RemoveDuplicates(typecheck::TypeManager* manager) const;

	private:
		std::map<std::string, std::vector<FunctionDefinition>> funcOverloads;

		ConstraintPass last_pass;

		// Implementations found in `type_solver+solver.cpp`
		void DoPass(ConstraintPass* pass, const TypeManager* manager) const;
		void DoPass_internal(ConstraintPass* pass, std::deque<std::size_t>/* this is a copy */ indexes, const TypeManager* manager, const std::size_t& prev_failed, const std::size_t& prev_emplaced) const;
	};
}
