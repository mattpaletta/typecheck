#include "constraint_pass.hpp"
#include <algorithm>

typecheck::ConstraintPass typecheck::ConstraintPass::CreateNewPass(const std::vector<typecheck::Constraint>& constraints) {
	auto pass = typecheck::ConstraintPass();
	pass.constraints = constraints;
	return pass;
}

std::size_t typecheck::ConstraintPass::Score(const typecheck::ConstraintPass& pass) {
	return 0;
}

bool typecheck::ConstraintPass::hasResolvedType(const typecheck::Type& type) const {
	return this->resolvedTypes.find(type.name()) != this->resolvedTypes.end();
}