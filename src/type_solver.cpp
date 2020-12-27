#include "typecheck/type_solver.hpp"
#include "typecheck/type_manager.hpp"
#include "typecheck/constraint_pass.hpp"

#include <numeric>
#include <set>

using namespace typecheck;

TypeSolver::TypeSolver() = default;

auto TypeSolver::solve(const TypeManager* manager) -> bool {
	// Builds utility data structures
	// this->InitPasses(manager);

	ConstraintPass best_pass;
	this->DoPass(&best_pass, manager);
	const auto is_valid = best_pass.IsValid();
	if (is_valid) {
		this->last_pass = std::move(best_pass);
	}
	return is_valid;
}

auto TypeSolver::getResolvedType(const TypeVar& _typeVar) const -> Type {
	return this->last_pass.getResolvedType(_typeVar);
}
