#include <typecheck/type_manager.hpp>
#include <typecheck/debug.hpp>
#include <typecheck/constraint.hpp>

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
#include <iostream>
#include <string>
#endif

#include <algorithm> // for std::sort

using namespace typecheck;

auto TypeManager::getConstraintKindScore(const ConstraintKind& kind) const -> int {
    switch (kind) {
        case BindParam:
        case Bind:
            return 0; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case ApplicableFunction:
            return 1; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case Conversion:
            return 2; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case ConformsTo:
            return 3; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case Equal:
            return 4; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        case BindOverload:
            return 5; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    }

    return 10; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
}

void TypeManager::SortConstraints() {
    // Sort the constraints by the order in which they need to be resolved
    std::sort(this->constraints.begin(), this->constraints.end(), [this](const Constraint& c1, const Constraint& c2) {
        return this->getConstraintKindScore(c1.kind()) < this->getConstraintKindScore(c2.kind());
    });
}

auto getNewBlankConstraint(ConstraintKind kind, const long long& id) -> Constraint {
	Constraint constraint;
	constraint.set_kind(kind);
	constraint.set_id(id);
	return constraint;
}

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
auto debug_constraint_headers(const Constraint& constraint) -> std::string {
#ifdef TYPECHECK_PRINT_SHORT_DEBUG
    return "Typecheck: Creating constraint: " + constraint.ShortDebugString();
#else
    return "Typecheck: Creating constraint: " + constraint.DebugString();
#endif
}
#endif

auto TypeManager::CreateEqualsConstraint(const TypeVar& t0, const TypeVar& t1) -> ConstraintPass::IDType {
	auto constraint = getNewBlankConstraint(ConstraintKind::Equal, this->constraint_generator.next_id());

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

auto TypeManager::CreateLiteralConformsToConstraint(const TypeVar& t0, const KnownProtocolKind::LiteralProtocol& protocol) -> ConstraintPass::IDType {
	auto constraint = getNewBlankConstraint(ConstraintKind::ConformsTo, this->constraint_generator.next_id());

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

auto TypeManager::CreateConvertibleConstraint(const TypeVar& T0, const TypeVar& T1) -> ConstraintPass::IDType {
    auto constraint = getNewBlankConstraint(ConstraintKind::Conversion, this->constraint_generator.next_id());

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

auto TypeManager::CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const std::vector<Type>& args, const Type& return_type) -> ConstraintPass::IDType {
    const auto returnVar = this->CreateTypeVar();
    std::vector<TypeVar> argVars;
    for (std::size_t i = 0; i < args.size(); ++i) {
        argVars.emplace_back(this->CreateTypeVar());
    }
    const auto constraintID = this->CreateApplicableFunctionConstraint(functionid, argVars, returnVar);

    // Wait until after we created the constraint before creating constraints, othersise, could have loose ends
    this->CreateBindToConstraint(returnVar, return_type);
    for (std::size_t i = 0; i < args.size(); ++i) {
        this->CreateBindToConstraint(argVars.at(i), args.at(i));
    }

    return constraintID;
}

auto TypeManager::CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const std::vector<TypeVar>& argVars, const TypeVar& returnTypeVar) -> ConstraintPass::IDType {
    FunctionVar funcVar;
    funcVar.set_id(functionid);
    for (auto& arg : argVars) {
        funcVar.add_args()->CopyFrom(arg);
    }

    funcVar.mutable_returnvar()->CopyFrom(returnTypeVar);
    return this->CreateApplicableFunctionConstraint(functionid, funcVar);
}

auto TypeManager::CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const FunctionVar& type) -> ConstraintPass::IDType {
    TYPECHECK_ASSERT(type.id() == functionid, "Function type ID should match function id and be set.");

    this->functions.push_back(type);
    return type.id();
}

auto TypeManager::CreateBindFunctionConstraint(const ConstraintPass::IDType& functionid, const TypeVar& T0, const std::vector<TypeVar>& args, const TypeVar& returnType) -> ConstraintPass::IDType {
    auto constraint = getNewBlankConstraint(ConstraintKind::BindOverload, this->constraint_generator.next_id());

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

auto TypeManager::CreateBindToConstraint(const TypeVar& T0, const Type& type) -> ConstraintPass::IDType {
    auto constraint = getNewBlankConstraint(ConstraintKind::Bind, this->constraint_generator.next_id());

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
