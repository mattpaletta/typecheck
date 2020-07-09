#include "type_solver.hpp"
#include "constraint_pass.hpp"

#include <numeric>
#include <set>

typecheck::TypeSolver::TypeSolver() {}

void typecheck::TypeSolver::setConstraints(const std::vector<typecheck::Constraint>& _constraints) {
	// Create copy
	this->constraints = _constraints;
}

void typecheck::TypeSolver::setTypes(const std::vector<typecheck::Type> _types) {
	// Create copy
	this->types = _types;
}

void typecheck::TypeSolver::setConversions(const std::map<std::string, std::set<std::string>>& _conversions) {
	this->conversions = _conversions;
}

void typecheck::TypeSolver::setFunctions(const std::vector<typecheck::FunctionDefinition>& funcDef) {
	this->functions = funcDef;
}

void typecheck::TypeSolver::InitPasses() {
	this->RemoveDuplicates();
	this->FindOverloads();
	this->BuildRefGraph();
}

namespace {
	struct BestPass {
		const size_t score;
		const typecheck::ConstraintPass pass;
		BestPass(const std::size_t _score, const typecheck::ConstraintPass& _pass) : score(_score), pass(_pass) {}
	};
}

bool typecheck::TypeSolver::solve() {
	// Builds utility data structures
	this->InitPasses();

	auto current_pass = typecheck::ConstraintPass::CreateNewPass(this->constraints);
	auto bestPass = BestPass(std::numeric_limits<size_t>::max(), current_pass);

	do {
		this->DoPass(&current_pass);
		if (typecheck::ConstraintPass::Score(current_pass)) {
			// Replace with new one.
		}
	} while (this->HasMorePasses(current_pass));
	this->last_pass = current_pass;
	return true;
}

typecheck::Type typecheck::TypeSolver::getResolvedType(const typecheck::Type& _typeVar) const {
	const auto resolvedName = this->last_pass.resolvedTypes.at(_typeVar.name());
	for (auto& ty : this->types) {
		if (ty.name() == resolvedName) {
			return ty;
		}
	}
	return {};
}
