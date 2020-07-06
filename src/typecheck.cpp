#include "typecheck.hpp"


TypeManager::TypeManager() : solver("simple_lp_program", operations_research::MPSolver::GLOP_LINEAR_PROGRAMMING) {}

void TypeManager::add(Constraint* constraint) {
	this->constraints.emplace_back(constraint);
}

void TypeManager::solve() {
	// Load all constraints into the solver
	for (auto& con : this->constraints) {

	}
	this->solver.Solve();
	// this->solver.Clear();
}

TypeSymbol TypeManager::getSymbol(const std::string& name) const {
	return TypeSymbol::Integer;
}
