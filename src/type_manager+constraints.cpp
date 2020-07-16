#include "typecheck/type_manager.hpp"
#include "typecheck/debug.hpp"

#include <typecheck_protos/constraint.pb.h>

int getConstraintKindScore(const typecheck::ConstraintKind& kind) {
    switch (kind) {
        case typecheck::ConstraintKind::Bind:
            return 0;
        case typecheck::ConstraintKind::ApplicableFunction:
            return 1;
        case typecheck::ConstraintKind::BindOverload:
            return 2;
        case typecheck::ConstraintKind::ConformsTo:
            return 3;
        case typecheck::ConstraintKind::Conversion:
            return 4;
        case typecheck::ConstraintKind::Equal:
            return 5;
        default:
            TYPECHECK_ASSERT(false, "Unknown constraint type, cannot get score.");
    }
}

void typecheck::TypeManager::SortConstraints() {
    // Sort the constraints by the order in which they need to be resolved
    std::sort(this->constraints.begin(), this->constraints.end(), [](const Constraint& c1, const Constraint& c2) {
        return getConstraintKindScore(c1.kind()) < getConstraintKindScore(c2.kind());
    });
}

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

	constraint.mutable_types()->mutable_first()->CopyFrom(t0);
	constraint.mutable_types()->mutable_second()->CopyFrom(t1);

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

std::size_t typecheck::TypeManager::CreateLiteralConformsToConstraint(const typecheck::TypeVar& t0, const typecheck::KnownProtocolKind_LiteralProtocol& protocol) {
	auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::ConformsTo, this->constraint_generator.next_id());

	TYPECHECK_ASSERT(!t0.symbol().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(this->registeredTypeVars.find(t0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

	constraint.mutable_conforms()->mutable_type()->CopyFrom(t0);
	constraint.mutable_conforms()->mutable_protocol()->set_literal(protocol);

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

std::size_t typecheck::TypeManager::CreateConvertibleConstraint(const typecheck::TypeVar& T0, const typecheck::TypeVar& T1) {
    auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::Conversion, this->constraint_generator.next_id());
    
    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    TYPECHECK_ASSERT(!T1.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T1.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    constraint.mutable_types()->mutable_first()->CopyFrom(T0);
    constraint.mutable_types()->mutable_second()->CopyFrom(T1);
    this->constraints.emplace_back(constraint);
    return constraint.id();
}

std::size_t typecheck::TypeManager::CreateApplicableFunctionConstraint(const typecheck::TypeVar& T0, const std::vector<typecheck::Type>& args, const typecheck::Type& return_type) {
    // overload to support creating from a vector.
    typecheck::Type funcType;
    for (auto& arg : args) {
        funcType.mutable_func()->add_args()->CopyFrom(arg);
        if (arg.has_raw()) {
            // These types are 'type': int, float, etc., so don't have to look them up as registered TypeVars.
            TYPECHECK_ASSERT(!arg.raw().name().empty(), "Cannot use empty type when creating constraint.");
        }
    }
    funcType.mutable_func()->mutable_returntype()->CopyFrom(return_type);
    return this->CreateApplicableFunctionConstraint(T0, funcType);
}

std::size_t typecheck::TypeManager::CreateApplicableFunctionConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type) {
    auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::ApplicableFunction, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    TYPECHECK_ASSERT(type.has_func(), "Must use a function type when creating function constraint.");

    constraint.mutable_explicit_()->mutable_var()->CopyFrom(T0);
    constraint.mutable_explicit_()->mutable_type()->CopyFrom(type);

    this->constraints.emplace_back(constraint);
    return constraint.id();
}

std::size_t typecheck::TypeManager::CreateBindFunctionConstraint(const typecheck::TypeVar& T0, const std::vector<typecheck::TypeVar>& args, const typecheck::TypeVar& returnType) {
    auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::BindOverload, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    constraint.mutable_overload()->mutable_type()->CopyFrom(T0);

    for (auto& arg : args) {
        TYPECHECK_ASSERT(!arg.symbol().empty(), "Cannot use empty type when creating constraint.");
        TYPECHECK_ASSERT(this->registeredTypeVars.find(arg.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
        constraint.mutable_overload()->add_argvars()->CopyFrom(arg);
    }

    TYPECHECK_ASSERT(!returnType.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(returnType.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    constraint.mutable_overload()->mutable_returnvar()->CopyFrom(returnType);

    this->constraints.emplace_back(constraint);
    return constraint.id();
}

std::size_t typecheck::TypeManager::CreateBindToConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type) {
    auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::Bind, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    TYPECHECK_ASSERT(type.has_raw() || type.has_func(), "Must insert valid type.");

    constraint.mutable_explicit_()->mutable_var()->CopyFrom(T0);
    constraint.mutable_explicit_()->mutable_type()->CopyFrom(type);

    this->constraints.emplace_back(constraint);
    return constraint.id();
}
