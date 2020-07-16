#include "typecheck/type_solver.hpp"
#include "typecheck/type_manager.hpp"
#include "typecheck/constraint_pass.hpp"
#include "typecheck/utils.hpp"

#include <numeric>
#include <set>

typecheck::TypeSolver::TypeSolver() {}

void typecheck::TypeSolver::InitPasses(typecheck::TypeManager* manager) {
	// this->RemoveDuplicates(manager);
	this->BuildRefGraph(manager);
}

bool typecheck::TypeSolver::solve(const TypeManager* manager) {
	// Builds utility data structures
	// this->InitPasses(manager);

	typecheck::ConstraintPass best_pass;
	this->DoPass(&best_pass, manager);
	const auto is_valid = best_pass.IsValid(manager);
	if (is_valid) {
		this->last_pass = std::move(best_pass);
	}
	return is_valid;
}

typecheck::Type typecheck::TypeSolver::getResolvedType(const typecheck::TypeVar& _typeVar) const {
	return this->last_pass.getResolvedType(_typeVar);
}
