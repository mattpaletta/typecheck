#include <typecheck/constraint_group.hpp>

auto typecheck::ConstraintGroup::contains(const ConstraintPass::IDType constraint) const -> bool {
	return this->_constraints.find(constraint) != this->_constraints.end();
}

void typecheck::ConstraintGroup::add(const ConstraintPass::IDType constraint) {
	this->_constraints.insert(constraint);
}

void typecheck::ConstraintGroup::remove(const ConstraintPass::IDType constraint) {
	this->_constraints.erase(constraint);
}

auto typecheck::ConstraintGroup::toList() const -> std::vector<typecheck::ConstraintPass::IDType> {
	std::vector<typecheck::ConstraintPass::IDType> out;
	for (const auto id : this->_constraints) {
		out.push_back(id);
	}
	return out;
}

auto typecheck::ConstraintGroup::size() const -> std::size_t {
	return this->_constraints.size();
}
