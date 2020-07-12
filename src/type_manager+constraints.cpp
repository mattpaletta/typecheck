#include "type_manager.hpp"
#include "debug.hpp"

#include <typecheck_protos/constraint.pb.h>

typecheck::Constraint getNewBlankConstraint(typecheck::ConstraintKind kind, const std::size_t& id) {
	typecheck::Constraint constraint;
	constraint.set_kind(kind);
	constraint.set_hasrestriction(false);
	constraint.set_isactive(false);
	constraint.set_isdisabled(false);
	constraint.set_isfavoured(false);
	constraint.set_id(id);
	return constraint;
}

std::size_t typecheck::TypeManager::CreateEqualsConstraint(const typecheck::TypeVar& t0, const typecheck::TypeVar& t1) {
	auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::Equal, this->constraint_generator.next_id());
	 
	TYPECHECK_ASSERT(!t0.symbol().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(!t1.symbol().empty(), "Cannot use empty type when creating constraint.");

	TYPECHECK_ASSERT(this->registeredTypeVars.find(t0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
	TYPECHECK_ASSERT(this->registeredTypeVars.find(t1.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

	constraint.mutable_types()->mutable_first()->set_name(t0.symbol());
	constraint.mutable_types()->mutable_second()->set_name(t1.symbol());

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

std::size_t typecheck::TypeManager::CreateLiteralConformsToConstraint(const typecheck::TypeVar& t0, const typecheck::KnownProtocolKind_LiteralProtocol& protocol) {
	auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::ConformsTo, this->constraint_generator.next_id());

	TYPECHECK_ASSERT(!t0.symbol().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(this->registeredTypeVars.find(t0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

	constraint.mutable_conforms()->mutable_type()->set_name(t0.symbol());
	constraint.mutable_conforms()->mutable_protocol()->set_literal(protocol);

	this->constraints.emplace_back(constraint);
	return constraint.id();
}