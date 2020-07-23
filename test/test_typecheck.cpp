#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <typecheck/type_manager.hpp>
#include <typecheck/generic_type_generator.hpp>

#include <typecheck/resolvers/ResolveConformsTo.hpp>
#include <typecheck/resolvers/ResolveEquals.hpp>
#include <typecheck/resolvers/ResolveBindTo.hpp>

// Utility function to avoid boilerplate code in testing.
void setupTypeManager(typecheck::TypeManager* tm) {
    // Turn this on for testing
    tm->use_reverse_sort = true;

    CHECK(tm->registerType("int"));
	CHECK(tm->registerType("float"));
	CHECK(tm->registerType("double"));
	CHECK(tm->setConvertible("int", "int"));
	CHECK(tm->setConvertible("int", "float"));
	CHECK(tm->setConvertible("int", "double"));
	CHECK(tm->setConvertible("double", "double"));
	CHECK(tm->setConvertible("float", "float"));
	CHECK(tm->setConvertible("float", "double"));
}

auto CreatMultipleSymbols(typecheck::TypeManager& tm, const std::size_t& num) -> std::vector<typecheck::TypeVar> {
    std::vector<typecheck::TypeVar> out;
    for (std::size_t i = 0; i < num; ++i) {
        out.push_back(tm.CreateTypeVar());
    }
    return out;
}

#define getDefaultTypeManager(tm) \
	typecheck::TypeManager tm; \
	setupTypeManager(&tm)

TEST_CASE("test resolve conforms to", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto constraintID = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);

	typecheck::ConstraintPass pass;
	auto resolver = typecheck::ResolveConformsTo(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve conforms to has neither", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();
    auto constraintID = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);

    typecheck::ConstraintPass pass;
    auto resolver = typecheck::ResolveConformsTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) > 0);
}

TEST_CASE("test resolve conforms to has one", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();
    auto constraintID = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);

    typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("int"), &tm);
    auto resolver = typecheck::ResolveConformsTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve conforms to has not conform", "[resolver]") {
    getDefaultTypeManager(tm);
    tm.registerType("string");
    auto T1 = tm.CreateTypeVar();
    auto constraintID = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);

    typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("string"), &tm);
    auto resolver = typecheck::ResolveConformsTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) > 1);
}

TEST_CASE("test resolve conforms to has not preferred", "[resolver]") {
    getDefaultTypeManager(tm);
    tm.registerType("string");
    auto T1 = tm.CreateTypeVar();
    auto constraintID = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);

    typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("float"), &tm);
    auto resolver = typecheck::ResolveConformsTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 1);
}

TEST_CASE("test resolve equals have both", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T = CreatMultipleSymbols(tm, 2);

    auto constraintID = tm.CreateEqualsConstraint(T.at(0), T.at(1));

    typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    typecheck::Constraint constraint1;
    constraint1.set_kind(typecheck::ConstraintKind::Bind);
    constraint1.set_id(2);
    pass.setResolvedType(constraint, T.at(0), tm.getRegisteredType("int"), &tm);
    pass.setResolvedType(constraint1, T.at(1), tm.getRegisteredType("int"), &tm);
    auto resolver = typecheck::ResolveEquals(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
    CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve bindto resolve", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();

    auto constraintID = tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));

	typecheck::ConstraintPass pass;
	auto resolver = typecheck::ResolveBindTo(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve bindto already resolved", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    auto constraintID = tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));

    typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("int"), &tm);
    auto resolver = typecheck::ResolveBindTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
    CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve bindto conflicting", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    auto constraint1 = tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    auto constraint2 = tm.CreateBindToConstraint(T1, tm.getRegisteredType("float"));

    typecheck::ConstraintPass pass;
    auto resolver1 = typecheck::ResolveBindTo(&pass, constraint1);
    auto resolver2 = typecheck::ResolveBindTo(&pass, constraint2);

    CHECK(resolver1.hasMoreSolutions(*tm.getConstraint(constraint1), &tm));
    CHECK(resolver2.hasMoreSolutions(*tm.getConstraint(constraint2), &tm));

    CHECK( resolver1.resolveNext(*tm.getConstraint(constraint1), &tm));
    CHECK(!resolver2.resolveNext(*tm.getConstraint(constraint2), &tm));

    CHECK(resolver1.score(*tm.getConstraint(constraint1), &tm) == 0);
    CHECK(resolver2.score(*tm.getConstraint(constraint2), &tm) > 0);
}

TEST_CASE("test resolve bindto conflicting full", "[type_solver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateBindToConstraint(T1, tm.getRegisteredType("float"));

    REQUIRE(!tm.solve());
}

TEST_CASE("test resolve equals have both not equal", "[resolver]") {
	getDefaultTypeManager(tm);
    auto T = CreatMultipleSymbols(tm, 2);

	auto constraintID = tm.CreateEqualsConstraint(T.at(0), T.at(1));

	typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    typecheck::Constraint constraint1;
    constraint1.set_kind(typecheck::ConstraintKind::Bind);
    constraint1.set_id(2);
	pass.setResolvedType(constraint, T.at(0), tm.getRegisteredType("int"), &tm);
	pass.setResolvedType(constraint1, T.at(1), tm.getRegisteredType("float"), &tm);
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) > 0);
}

TEST_CASE("test resolve equals have t0", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
	pass.setResolvedType(constraint, T1, tm.getRegisteredType("int"), &tm);
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve equals have t1", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
	pass.setResolvedType(constraint, T2, tm.getRegisteredType("int"), &tm);
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve equals have neither", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
	CHECK(!resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) > 0);
}

TEST_CASE("load basic type load", "[type_manager]") {
	typecheck::TypeManager tm;
	CHECK(tm.registerType("int"));
	CHECK(tm.registerType("float"));
	CHECK(tm.registerType("double"));
	CHECK(!tm.registerType("double"));
	CHECK(tm.hasRegisteredType("int"));
	CHECK(tm.hasRegisteredType("float"));
	CHECK(tm.hasRegisteredType("double"));
	CHECK(!tm.hasRegisteredType("string"));
}

TEST_CASE("load basic type conversions", "[type_manager]") {
	typecheck::TypeManager tm;
	CHECK(tm.registerType("int"));
	CHECK(tm.registerType("float"));
	CHECK(tm.registerType("double"));
	CHECK(tm.setConvertible("int", "double"));
	CHECK(tm.setConvertible("int", "float"));
	CHECK(tm.setConvertible("float", "double"));

	CHECK(tm.isConvertible("int", "int"));
	CHECK(tm.isConvertible("float", "float"));
	CHECK(tm.isConvertible("double", "double"));

	CHECK(tm.isConvertible("int", "double"));
	CHECK(tm.isConvertible("int", "float"));
	CHECK(tm.isConvertible("float", "double"));

	CHECK(!tm.isConvertible("double", "int"));
	CHECK(!tm.isConvertible("float", "int"));
	CHECK(!tm.isConvertible("double", "float"));
}

TEST_CASE("solve basic type int equals constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	// type(1) == type(2);
	auto T1 = tm.CreateTypeVar(); // These are names of type variables, not actual types.
	auto T2 = tm.CreateTypeVar();

	tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	tm.CreateEqualsConstraint(T1, T2);

	CHECK(tm.solve());

    REQUIRE(tm.getResolvedType(T1).has_raw());
    REQUIRE(tm.getResolvedType(T2).has_raw());

    CHECK(tm.getResolvedType(T1).raw().name() == "int");
    CHECK(tm.getResolvedType(T2).raw().name() == "int");
}

TEST_CASE("solve basic type float equals constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
	tm.CreateEqualsConstraint(T1, T2);

	REQUIRE(tm.solve());

    REQUIRE(tm.getResolvedType(T1).has_raw());
    REQUIRE(tm.getResolvedType(T2).has_raw());

    CHECK(tm.getResolvedType(T1).raw().name() == "float");
    CHECK(tm.getResolvedType(T2).raw().name() == "float");
}

TEST_CASE("solve basic type equals mutally recursive constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

    tm.CreateEqualsConstraint(T2, T1);
	tm.CreateEqualsConstraint(T1, T2);

	REQUIRE(!tm.solve());
}

TEST_CASE("solve basic type equals triangle constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreatMultipleSymbols(tm, 3);

    tm.CreateEqualsConstraint(T.at(2) , T.at(0));
    tm.CreateEqualsConstraint(T.at(0), T.at(1));
    tm.CreateEqualsConstraint(T.at(1), T.at(2));

    REQUIRE(!tm.solve());
}

TEST_CASE("solve basic type conforms to triangle solvable constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	auto T3 = tm.CreateTypeVar();

	tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateEqualsConstraint(T1, T2);
    tm.CreateEqualsConstraint(T2, T3);
    tm.CreateEqualsConstraint(T3, T1);

	REQUIRE(tm.solve());
}

TEST_CASE("solve basic type conforms to triangle conversion solvable constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	auto T3 = tm.CreateTypeVar();

	tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
	tm.CreateEqualsConstraint(T1, T2);
	tm.CreateEqualsConstraint(T2, T3);
	tm.CreateEqualsConstraint(T3, T1);

	REQUIRE(tm.solve());
}

TEST_CASE("solve convertible constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateConvertibleConstraint(T1, T2);

    REQUIRE(tm.solve());
}

TEST_CASE("solve convertible reverse constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
    tm.CreateConvertibleConstraint(T2, T1);

    REQUIRE(!tm.solve());
}

TEST_CASE("solve convertible bind reverse constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
    tm.CreateConvertibleConstraint(T1, T2);

    REQUIRE(tm.solve());
}

TEST_CASE("solve convertible conversion explicit constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
    tm.CreateBindToConstraint(T2, tm.getRegisteredType("float"));
    tm.CreateEqualsConstraint(T2, T1);

    REQUIRE(tm.solve());

    CHECK(tm.getResolvedType(T1).raw().name() == "float");
    CHECK(tm.getResolvedType(T2).raw().name() == "float");
}

TEST_CASE("solve function application constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreatMultipleSymbols(tm, 4);

    const auto T0FuncHash = std::hash<std::string>()(T.at(0).symbol());
    tm.CreateApplicableFunctionConstraint(T0FuncHash, {tm.getRegisteredType("int"), tm.getRegisteredType("float")}, tm.getRegisteredType("double"));
    tm.CreateBindFunctionConstraint(T0FuncHash, T.at(0), {T.at(1), T.at(2)}, T.at(3));

    // T0 = (T1, T2) -> T3
    REQUIRE(tm.solve());
    CHECK(tm.getResolvedType(T.at(0)).has_func());
    REQUIRE(tm.getResolvedType(T.at(1)).has_raw());
    REQUIRE(tm.getResolvedType(T.at(2)).has_raw());
    REQUIRE(tm.getResolvedType(T.at(3)).has_raw());

    CHECK(tm.getResolvedType(T.at(1)).raw().name() == "int");
    CHECK(tm.getResolvedType(T.at(2)).raw().name() == "float");
    CHECK(tm.getResolvedType(T.at(3)).raw().name() == "double");
}

TEST_CASE("solve function different num args application constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreatMultipleSymbols(tm, 3);

    // func foo(a: Int, b: Float) -> Double
    const auto T0FuncHash = std::hash<std::string>()("foo:a:b");
    tm.CreateApplicableFunctionConstraint(T0FuncHash, { tm.getRegisteredType("int"), tm.getRegisteredType("float") }, tm.getRegisteredType("double"));

    // func foo(a: Int) -> Double
    const auto T1FuncHash = std::hash<std::string>()("foo:a");
    tm.CreateApplicableFunctionConstraint(T1FuncHash, { tm.getRegisteredType("int") }, tm.getRegisteredType("double"));

    tm.CreateBindFunctionConstraint(T1FuncHash, T.at(0), { T.at(1) }, T.at(2));

    // T0 = (T1, T2) -> T3
    REQUIRE(tm.solve());
    REQUIRE(tm.getResolvedType(T.at(0)).has_func());
    REQUIRE(tm.getResolvedType(T.at(1)).has_raw());
    REQUIRE(tm.getResolvedType(T.at(2)).has_raw());

    // Check it got bound to: func foo(a: Int) -> Double
    CHECK(tm.getResolvedType(T.at(0)).func().args_size() == 1);
    REQUIRE(tm.getResolvedType(T.at(0)).func().args(0).has_raw());
    CHECK(tm.getResolvedType(T.at(0)).func().args(0).raw().name() == "int");
    REQUIRE(tm.getResolvedType(T.at(0)).func().has_returntype());
    REQUIRE(tm.getResolvedType(T.at(0)).func().returntype().has_raw());
    CHECK(tm.getResolvedType(T.at(0)).func().returntype().raw().name() == "double");

    CHECK(tm.getResolvedType(T.at(1)).raw().name() == "int");
    CHECK(tm.getResolvedType(T.at(2)).raw().name() == "double");
}

TEST_CASE("solve function same num args different types application constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreatMultipleSymbols(tm, 3);

    // func foo(a: Int) -> Double
    tm.CreateApplicableFunctionConstraint(tm.CreateFunctionHash("foo", {"a"}), { tm.getRegisteredType("int") }, tm.getRegisteredType("double"));

    // func foo(a: Float) -> Double
    tm.CreateApplicableFunctionConstraint(tm.CreateFunctionHash("foo", { "a" }), { tm.getRegisteredType("float") }, tm.getRegisteredType("double"));

    tm.CreateBindFunctionConstraint(tm.CreateFunctionHash("foo", { "a" }), T.at(0), { T.at(1) }, T.at(2));
    tm.CreateLiteralConformsToConstraint(T.at(1), typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);

    // T0 = (T1, T2) -> T3
    REQUIRE(tm.solve());
    REQUIRE(tm.getResolvedType(T.at(0)).has_func());
    REQUIRE(tm.getResolvedType(T.at(1)).has_raw());
    REQUIRE(tm.getResolvedType(T.at(2)).has_raw());

    // Check it got bound to: func foo(a: Int) -> Double
    CHECK(tm.getResolvedType(T.at(0)).func().args_size() == 1);
    REQUIRE(tm.getResolvedType(T.at(0)).func().args(0).has_raw());
    CHECK(tm.getResolvedType(T.at(0)).func().args(0).raw().name() == "int");
    REQUIRE(tm.getResolvedType(T.at(0)).func().has_returntype());
    REQUIRE(tm.getResolvedType(T.at(0)).func().returntype().has_raw());
    CHECK(tm.getResolvedType(T.at(0)).func().returntype().raw().name() == "double");

    CHECK(tm.getResolvedType(T.at(1)).raw().name() == "int");
    CHECK(tm.getResolvedType(T.at(2)).raw().name() == "double");
}

TEST_CASE("solve for-loop constraints", "[constraint]") {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    auto T0 = tm.CreateTypeVar();
    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();
    auto T4 = tm.CreateTypeVar();
    auto T5 = tm.CreateTypeVar();
    auto T6 = tm.CreateTypeVar();
    auto T7 = tm.CreateTypeVar();
    auto T8 = tm.CreateTypeVar();
    auto T9 = tm.CreateTypeVar();
    auto T10 = tm.CreateTypeVar();
    auto T11 = tm.CreateTypeVar();
    auto T12 = tm.CreateTypeVar();

    tm.CreateEqualsConstraint(T0, T3);
    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind_LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateEqualsConstraint(T3, T4);
    tm.CreateLiteralConformsToConstraint(T4, typecheck::KnownProtocolKind_LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateEqualsConstraint(T5, T3);
    tm.CreateBindToConstraint(T6, tm.getRegisteredType("bool"));
    tm.CreateEqualsConstraint(T7, T0);
    tm.CreateLiteralConformsToConstraint(T8, typecheck::KnownProtocolKind_LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateEqualsConstraint(T9, T7);
    tm.CreateBindToConstraint(T10, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T11, typecheck::KnownProtocolKind_LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateBindToConstraint(T12, tm.getRegisteredType("void"));

    REQUIRE(tm.solve());
    REQUIRE(tm.getResolvedType(T1).has_raw());
    REQUIRE(tm.getResolvedType(T2).has_raw());
    REQUIRE(tm.getResolvedType(T3).has_raw());
    REQUIRE(tm.getResolvedType(T4).has_raw());
    REQUIRE(tm.getResolvedType(T5).has_raw());
    REQUIRE(tm.getResolvedType(T6).has_raw());
    REQUIRE(tm.getResolvedType(T7).has_raw());
    REQUIRE(tm.getResolvedType(T8).has_raw());
    REQUIRE(tm.getResolvedType(T9).has_raw());
    REQUIRE(tm.getResolvedType(T10).has_raw());
    REQUIRE(tm.getResolvedType(T11).has_raw());
    REQUIRE(tm.getResolvedType(T12).has_raw());

    CHECK(tm.getResolvedType(T1).raw().name()  == "int");
    CHECK(tm.getResolvedType(T2).raw().name()  == "int");
    CHECK(tm.getResolvedType(T3).raw().name()  == "int");
    CHECK(tm.getResolvedType(T4).raw().name()  == "int");
    CHECK(tm.getResolvedType(T5).raw().name()  == "int");
    CHECK(tm.getResolvedType(T6).raw().name()  == "bool");
    CHECK(tm.getResolvedType(T7).raw().name()  == "int");
    CHECK(tm.getResolvedType(T8).raw().name()  == "int");
    CHECK(tm.getResolvedType(T9).raw().name()  == "int");
    CHECK(tm.getResolvedType(T10).raw().name() == "int");
    CHECK(tm.getResolvedType(T11).raw().name() == "int");
    CHECK(tm.getResolvedType(T12).raw().name() == "void");
}
