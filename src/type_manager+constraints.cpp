#include "typecheck/type_manager.hpp"
#include "typecheck/debug.hpp"

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
#include <iostream>
#include <string>
#endif

#include <typecheck_protos/constraint.pb.h>

auto typecheck::TypeManager::getConstraintKindScore(const typecheck::ConstraintKind& kind) const -> int {
    switch (kind) {
        case BindParam:
        case Bind:
            return 0; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case ApplicableFunction:
            return 1; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case BindOverload:
            return 5; // return 2; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case ConformsTo:
            return 6; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case Conversion:
            return 3; // return 4; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case Equal:
            return 2;// return 5; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case ConstraintKind_INT_MIN_SENTINEL_DO_NOT_USE_:
        case ConstraintKind_INT_MAX_SENTINEL_DO_NOT_USE_:
            return 10; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    }

    return 10; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
}

void typecheck::TypeManager::SortConstraints() {
    // Sort the constraints by the order in which they need to be resolved
    std::sort(this->constraints.begin(), this->constraints.end(), [this](const Constraint& c1, const Constraint& c2) {
        return this->getConstraintKindScore(c1.kind()) < this->getConstraintKindScore(c2.kind());
    });
}

auto getNewBlankConstraint(typecheck::ConstraintKind kind, const long long& id) -> typecheck::Constraint {
	typecheck::Constraint constraint;
	constraint.set_kind(kind);
	constraint.set_hasrestriction(false);
	constraint.set_isactive(false);
	constraint.set_isdisabled(false);
	constraint.set_isfavoured(false);
	constraint.set_id(id);
	return constraint;
}

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
auto debug_constraint_headers(const typecheck::Constraint& constraint) -> std::string {
#ifdef TYPECHECK_PRINT_SHORT_DEBUG
    return "Typecheck: Creating constraint: " + constraint.ShortDebugString();
#else
    return "Typecheck: Creating constraint: " + constraint.DebugString();
#endif
}
#endif

auto typecheck::TypeManager::CreateEqualsConstraint(const typecheck::TypeVar& t0, const typecheck::TypeVar& t1) -> typecheck::ConstraintPass::IDType {
	auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::Equal, this->constraint_generator.next_id());
	 
	TYPECHECK_ASSERT(!t0.symbol().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(!t1.symbol().empty(), "Cannot use empty type when creating constraint.");

	TYPECHECK_ASSERT(this->registeredTypeVars.find(t0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
	TYPECHECK_ASSERT(this->registeredTypeVars.find(t1.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

	constraint.mutable_types()->mutable_first()->CopyFrom(t0);
	constraint.mutable_types()->mutable_second()->CopyFrom(t1);


#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

auto typecheck::TypeManager::CreateLiteralConformsToConstraint(const typecheck::TypeVar& t0, const typecheck::KnownProtocolKind_LiteralProtocol& protocol) -> typecheck::ConstraintPass::IDType {
	auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::ConformsTo, this->constraint_generator.next_id());

	TYPECHECK_ASSERT(!t0.symbol().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(this->registeredTypeVars.find(t0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

	constraint.mutable_conforms()->mutable_type()->CopyFrom(t0);
	constraint.mutable_conforms()->mutable_protocol()->set_literal(protocol);


#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

auto typecheck::TypeManager::CreateConvertibleConstraint(const typecheck::TypeVar& T0, const typecheck::TypeVar& T1) -> typecheck::ConstraintPass::IDType {
    auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::Conversion, this->constraint_generator.next_id());
    
    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    TYPECHECK_ASSERT(!T1.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T1.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    constraint.mutable_types()->mutable_first()->CopyFrom(T0);
    constraint.mutable_types()->mutable_second()->CopyFrom(T1);

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

    this->constraints.emplace_back(constraint);
    return constraint.id();
}

auto typecheck::TypeManager::CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const std::vector<typecheck::Type>& args, const typecheck::Type& return_type, const std::vector<ConstraintPass::IDType>& nested) -> typecheck::ConstraintPass::IDType {
    const auto returnVar = this->CreateTypeVar();
    std::vector<TypeVar> argVars;
    for (auto& arg : args) {
        argVars.emplace_back(this->CreateTypeVar());
    }
    const auto constraintID = this->CreateApplicableFunctionConstraint(functionid, argVars, returnVar, nested);

    // Wait until after we created the constraint before creating constraints, othersise, could have loose ends
    const auto returnConstraint = this->CreateBindToConstraint(returnVar, return_type);
    for (std::size_t i = 0; i < args.size(); ++i) {
        const auto argConstraint = this->CreateBindToConstraint(argVars.at(i), args.at(i));
    }

    return constraintID;
}

auto typecheck::TypeManager::CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const std::vector<TypeVar>& argVars, const TypeVar& returnTypeVar, const std::vector<ConstraintPass::IDType>& nested) -> ConstraintPass::IDType {
    typecheck::FunctionVar funcVar;
    funcVar.set_id(functionid);
    for (auto& arg : argVars) {
        funcVar.add_args()->CopyFrom(arg);
    }

    funcVar.mutable_returnvar()->CopyFrom(returnTypeVar);
    return this->CreateApplicableFunctionConstraint(functionid, funcVar, nested);
}

auto typecheck::TypeManager::CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const typecheck::FunctionVar& type, const std::vector<ConstraintPass::IDType>& nested) -> typecheck::ConstraintPass::IDType {
    TYPECHECK_ASSERT(type.id() == functionid, "Function type ID should match function id and be set.");

    this->functions.push_back(type);
    return type.id();
}

auto typecheck::TypeManager::CreateBindFunctionConstraint(const ConstraintPass::IDType& functionid, const typecheck::TypeVar& T0, const std::vector<typecheck::TypeVar>& args, const typecheck::TypeVar& returnType) -> typecheck::ConstraintPass::IDType {
    auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::BindOverload, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    constraint.mutable_overload()->mutable_type()->CopyFrom(T0);
    constraint.mutable_overload()->set_functionid(functionid);

    for (auto& arg : args) {
        TYPECHECK_ASSERT(!arg.symbol().empty(), "Cannot use empty type when creating constraint.");
        TYPECHECK_ASSERT(this->registeredTypeVars.find(arg.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
        constraint.mutable_overload()->add_argvars()->CopyFrom(arg);
    }

    TYPECHECK_ASSERT(!returnType.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(returnType.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    constraint.mutable_overload()->mutable_returnvar()->CopyFrom(returnType);

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

    this->constraints.emplace_back(constraint);
    return constraint.id();
}

auto typecheck::TypeManager::CreateBindToConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type) -> typecheck::ConstraintPass::IDType {
    auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::Bind, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    TYPECHECK_ASSERT(type.has_raw() || type.has_func(), "Must insert valid type.");

    constraint.mutable_explicit_()->mutable_var()->CopyFrom(T0);
    constraint.mutable_explicit_()->mutable_type()->CopyFrom(type);

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

    this->constraints.emplace_back(constraint);
    return constraint.id();
}
