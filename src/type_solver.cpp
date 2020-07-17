#include "typecheck/type_solver.hpp"
#include "typecheck/type_manager.hpp"
#include "typecheck/constraint_pass.hpp"

#include <numeric>
#include <set>

typecheck::TypeSolver::TypeSolver() = default;

void typecheck::TypeSolver::InitPasses([[maybe_unused]] typecheck::TypeManager* manager) {

}

auto typecheck::TypeSolver::solve(const TypeManager* manager) -> bool {
	// Builds utility data structures
	// this->InitPasses(manager);

	typecheck::ConstraintPass best_pass;
	this->DoPass(&best_pass, manager);
	const auto is_valid = best_pass.IsValid();
	if (is_valid) {
		this->last_pass = std::move(best_pass);
	}
	return is_valid;
}

auto typecheck::TypeSolver::getResolvedType(const typecheck::TypeVar& _typeVar) const -> typecheck::Type {
	return this->last_pass.getResolvedType(_typeVar);
}
