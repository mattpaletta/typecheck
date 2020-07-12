#include "type_solver.hpp"
#include "type_manager.hpp"
#include "constraint_pass.hpp"
#include "utils.hpp"

#include <numeric>
#include <set>

typecheck::TypeSolver::TypeSolver() {}

void typecheck::TypeSolver::InitPasses() {
	this->RemoveDuplicates();
	this->FindOverloads();
	this->BuildRefGraph();
}

bool typecheck::TypeSolver::solve(const TypeManager* manager) {
	// Builds utility data structures
	this->InitPasses();

	typecheck::ConstraintPass best_pass;
	this->DoPass(&best_pass, manager);
	const auto is_valid = best_pass.IsValid(manager);
	if (is_valid) {
		this->last_pass = std::move(best_pass);
	}
	return is_valid;
}

typecheck::Type typecheck::TypeSolver::getResolvedType(const typecheck::TypeVar& _typeVar) const {
	return this->last_pass.getResolvedType(_typeVar.symbol());
}
