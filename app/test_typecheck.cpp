#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <iostream>

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
    pass.setResolvedType(T1, tm.getRegisteredType("int"));
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
    pass.setResolvedType(T1, tm.getRegisteredType("string"));
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
    pass.setResolvedType(T1, tm.getRegisteredType("float"));
    auto resolver = typecheck::ResolveConformsTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 1);
}

TEST_CASE("test resolve equals have both", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    auto constraintID = tm.CreateEqualsConstraint(T1, T2);

    typecheck::ConstraintPass pass;
    pass.setResolvedType(T1, tm.getRegisteredType("int"));
    pass.setResolvedType(T2, tm.getRegisteredType("int"));
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
    pass.setResolvedType(T1, tm.getRegisteredType("int"));
    auto resolver = typecheck::ResolveBindTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
    CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve bindto conflicting", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    auto constraintID = tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));

    typecheck::ConstraintPass pass;
    pass.setResolvedType(T1, tm.getRegisteredType("float"));
    auto resolver = typecheck::ResolveBindTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
    CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) > 0);
}

TEST_CASE("test resolve equals have both not equal", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
	pass.setResolvedType(T1, tm.getRegisteredType("int"));
	pass.setResolvedType(T2, tm.getRegisteredType("float"));
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
	pass.setResolvedType(T1, tm.getRegisteredType("int"));
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
	pass.setResolvedType(T2, tm.getRegisteredType("int"));
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


TEST_CASE("test generic type generator", "[type_generator]") {
	typecheck::GenericTypeGenerator g;
	std::cout << "Starting Test" << std::endl;
	CHECK(g.next() == "A");
	CHECK(g.next() == "B");
	CHECK(g.next() == "C");
	CHECK(g.next() == "D");
	for (int i = 4; i < 26; ++i) {
		// Discard values
		g.next();
	}
	CHECK(g.next() == "BA");
	CHECK(g.next() == "BB");
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

	auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	auto constraintT2 = tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	auto constraintT3 = tm.CreateEqualsConstraint(T1, T2);

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

	auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	auto constraintT2 = tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
	auto constraintT3 = tm.CreateEqualsConstraint(T1, T2);

	CHECK(tm.solve());

    REQUIRE(tm.getResolvedType(T1).has_raw());
    REQUIRE(tm.getResolvedType(T2).has_raw());

    CHECK(tm.getResolvedType(T1).raw().name() == "float");
    CHECK(tm.getResolvedType(T2).raw().name() == "float");
}

TEST_CASE("solve basic type equals mutally recursive constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	auto T3 = tm.CreateTypeVar();

    auto constraintT1 = tm.CreateEqualsConstraint(T2, T1);
	auto constraintT2 = tm.CreateEqualsConstraint(T1, T2);

	CHECK(!tm.solve());
}

TEST_CASE("solve basic type equals triangle constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();

    auto constraintT1 = tm.CreateEqualsConstraint(T3, T1);
    auto constraintT2 = tm.CreateEqualsConstraint(T1, T2);
    auto constraintT3 = tm.CreateEqualsConstraint(T2, T3);

    CHECK(!tm.solve());
}

TEST_CASE("solve basic type conforms to triangle solvable constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	auto T3 = tm.CreateTypeVar();

	auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	auto constraintT2 = tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    auto constraintT4 = tm.CreateEqualsConstraint(T1, T2);
    auto constraintT5 = tm.CreateEqualsConstraint(T2, T3);
    auto constraintT3 = tm.CreateEqualsConstraint(T3, T1);

	CHECK(tm.solve());
}

TEST_CASE("solve basic type conforms to triangle conversion solvable constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	auto T3 = tm.CreateTypeVar();

	auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	auto constraintT2 = tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
	auto constraintT3 = tm.CreateEqualsConstraint(T1, T2);
	auto constraintT4 = tm.CreateEqualsConstraint(T2, T3);
	auto constraintT5 = tm.CreateEqualsConstraint(T3, T1);

	CHECK(tm.solve());
}

TEST_CASE("solve convertible constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    auto constraintT3 = tm.CreateConvertibleConstraint(T1, T2);

    CHECK(tm.solve());
}

TEST_CASE("solve convertible reverse constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
    auto constraintT3 = tm.CreateConvertibleConstraint(T2, T1);

    REQUIRE(!tm.solve());
}

TEST_CASE("solve convertible bind reverse constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
    auto constraintT2 = tm.CreateBindToConstraint(T2, tm.getRegisteredType("int"));
    auto constraintT3 = tm.CreateConvertibleConstraint(T2, T1);

    REQUIRE(!tm.solve());

    CHECK(tm.getResolvedType(T1).raw().name() == "int");
    CHECK(tm.getResolvedType(T2).raw().name() == "int");
}

TEST_CASE("solve convertible conversion explicit constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
    auto constraintT2 = tm.CreateBindToConstraint(T2, tm.getRegisteredType("float"));
    auto constraintT3 = tm.CreateEqualsConstraint(T2, T1);

    REQUIRE(tm.solve());

    CHECK(tm.getResolvedType(T1).raw().name() == "float");
    CHECK(tm.getResolvedType(T2).raw().name() == "float");
}

TEST_CASE("solve function application constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T0 = tm.CreateTypeVar();
    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();

    auto constraintT1 = tm.CreateApplicableFunctionConstraint(T0, {tm.getRegisteredType("int"), tm.getRegisteredType("float")}, tm.getRegisteredType("double"));
    auto constraintT2 = tm.CreateBindFunctionConstraint(T0, {T1, T2}, T3);

    // T0 = (T1, T2) -> T3
    CHECK(tm.solve());
    REQUIRE(tm.getResolvedType(T1).has_raw());
    REQUIRE(tm.getResolvedType(T2).has_raw());
    REQUIRE(tm.getResolvedType(T3).has_raw());

    CHECK(tm.getResolvedType(T1).raw().name() == "int");
    CHECK(tm.getResolvedType(T2).raw().name() == "float");
    CHECK(tm.getResolvedType(T3).raw().name() == "double");
}
