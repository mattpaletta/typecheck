#include "type_solver.hpp"
#include <set>
#include <vector>
#include <iostream>

#define TYPECHECK_PROTOTYPE() std::cout << "Warning, prototype code: " << __FILE__ << ":" << __LINE__ << std::endl

std::vector<typecheck::Type> typecheck::TypeSolver::getAllConforms(const typecheck::Constraint& constraint) const {
	std::vector<typecheck::Type> options;
	TYPECHECK_PROTOTYPE();
	switch (constraint.conforms().protocol().kind_case()) {
	case typecheck::KnownProtocolKind::kLiteral:
		switch (constraint.conforms().protocol().literal()) {
		case typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger:
			// The first one is 'preferred'
			options.emplace_back();
			options.back().set_name("int");
			options.emplace_back();
			options.back().set_name("float");
			options.emplace_back();
			options.back().set_name("double");
			break;
		}
		break;
	case typecheck::KnownProtocolKind::kDefault:
		break;
	}
	return options;
}

typecheck::Type typecheck::TypeSolver::getPreferredConforms(const typecheck::Constraint& constraint) const {
	typecheck::Type type;
	switch (constraint.conforms().protocol().kind_case()) {
	case typecheck::KnownProtocolKind::kLiteral:
		switch (constraint.conforms().protocol().literal()) {
		case typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger:
			type.set_name("int");
			break;
		}
		break;
	case typecheck::KnownProtocolKind::kDefault:
		break;
	}
	return type;
}

bool typecheck::TypeSolver::HasMorePasses(const typecheck::ConstraintPass& pass) {
	return false;
}

bool typecheck::TypeSolver::ResolveConformsTo(const typecheck::Constraint& constraint, typecheck::ConstraintPass* pass) {
	auto conformVar = constraint.conforms().type();
	if (pass->resolvedTypes.find(conformVar.name()) == pass->resolvedTypes.end()) {
		// Unresolved
		auto preferred = this->getPreferredConforms(constraint);
		auto allCandidates = this->getAllConforms(constraint);
		
		// e.g. T0 -> int
		pass->resolvedTypes.insert(std::make_pair(conformVar.name(), preferred.name()));
		// TODO: Keep counter, so we know which 'type' to try.
		return true;
	} else {
		// Already been resolved
		return true;
	}
}

bool typecheck::TypeSolver::ResolveEquals(const typecheck::Constraint& constraint, typecheck::ConstraintPass* pass) {
	if (!constraint.types().has_first() || !constraint.types().has_second()) {
		// Error
		throw std::runtime_error("Equals constraint must have two types.");
	}
	auto eqType0 = constraint.types().first();
	auto eqType1 = constraint.types().second();

	if (pass->hasResolvedType(eqType0) && pass->hasResolvedType(eqType1)) {
		// They have already both been resolved
		// we will make sure they are the same when we add up the score
		return true;
	} else if (pass->hasResolvedType(eqType0)) {
		// Only T0 has been resolved, make T1 = T0;
		pass->resolvedTypes.emplace(std::make_pair(eqType1.name(), pass->resolvedTypes.at(eqType0.name())));
		return true;
	} else if (pass->hasResolvedType(eqType1)) {
		// Only T1 has been resolved, make T0 = T1;
		pass->resolvedTypes.emplace(std::make_pair(eqType0.name(), pass->resolvedTypes.at(eqType1.name())));
		return true;
	} else {
		// Neither has been resolved
		return false;
	}
}

void typecheck::TypeSolver::DoPass(ConstraintPass* pass) {
	std::set<std::size_t> resolvedConstraints;
	
	// Stop when we've resolved all of the constraints
	while (resolvedConstraints.size() < pass->constraints.size()) {
		for (std::size_t i = 0; i < pass->constraints.size(); ++i) {
			if (resolvedConstraints.find(i) != resolvedConstraints.end()) {
				// We've already processed this one
				continue;
			}

			bool did_resolve = false;
			auto& constraint = pass->constraints.at(i);

			switch (constraint.kind()) {
			case typecheck::ConstraintKind::ConformsTo:
				did_resolve = this->ResolveConformsTo(constraint, pass);
				break;
			case typecheck::ConstraintKind::Conversion:
				break;
			case typecheck::ConstraintKind::Equal:
				did_resolve = this->ResolveEquals(constraint, pass);
				break;
			} 
			/*
			else if (canResolveFunction(constraint)) {
				// Add in types T0: (T1, T2) -> T3
				// in solution
			}*/
			
			/*
			else if (canResolveConvertible(constraint)) {
				if (is_convertible(T0, T1) {

				}
			}
			*/
			
			if (did_resolve) {
				resolvedConstraints.insert(i);
			}
		}
	}

}