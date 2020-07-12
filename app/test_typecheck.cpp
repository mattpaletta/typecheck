#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <iostream>

#include "type_manager.hpp"
#include "generic_type_generator.hpp"

#include "resolvers/ResolveConformsTo.hpp"
#include "resolvers/ResolveEquals.hpp"

/*
TEST_CASE("test integer constraint", "[typecheck]") {
	operations_research::MPSolver solver("integer_constant", operations_research::MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);
	std::vector<operations_research::MPVariable*> vars;
	auto make_var = [&vars, &solver](const std::string& name) {
		vars.emplace_back(solver.MakeIntVar(0, 1, name));
	};
	std::vector<std::string> vars_to_make = {"T0", "T1"};
	for (auto& name : vars_to_make) {
		// Create 'basic vars'
		make_var(name + "_i");
		make_var(name + "_f");
		make_var(name + "_b");


		// The type TX can only be one of (int, float, bool)
		auto* constraint = solver.MakeRowConstraint(1, 1, name + "_oneof");
		constraint->SetCoefficient(solver.LookupVariableOrNull(name + "_i"), 1);
		constraint->SetCoefficient(solver.LookupVariableOrNull(name + "_f"), 1);
		constraint->SetCoefficient(solver.LookupVariableOrNull(name + "_b"), 1);
	}

	// Minimize the number of conversions to do
	auto* objective = solver.MutableObjective();

	// T0 is an int
	objective->SetCoefficient(solver.LookupVariableOrNull("T0_i"), 1);
	objective->SetCoefficient(solver.LookupVariableOrNull("T0_b"), 0);
	objective->SetCoefficient(solver.LookupVariableOrNull("T0_f"), 0);

	// T1 is a float
	objective->SetCoefficient(solver.LookupVariableOrNull("T1_i"), 0);
	objective->SetCoefficient(solver.LookupVariableOrNull("T1_b"), 0);
	objective->SetCoefficient(solver.LookupVariableOrNull("T1_f"), 1);

	objective->SetMaximization();

	// T0 == T1
	solver.Solve();
}

TEST_CASE("test int constraint", "[typecheck]") {
	TypeManager tm;
	// i = 0;
	// i == int
	Constraint c;
	c.kind = Bind;
	c.restriction = DeepEquality;
	c.Types.first = TypeSymbol::Integer;
	tm.add(&c);
	tm.solve();
};
*/

// Utility function to avoid boilerplate code in testing.
void setupTypeManager(typecheck::TypeManager* tm) {
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
	CHECK(resolver.hasMoreSolutions(tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve equals have both", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
	pass.setResolvedType(T1.symbol(), tm.getRegisteredType("int"));
	pass.setResolvedType(T2.symbol(), tm.getRegisteredType("int"));
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve equals have both not equal", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
	pass.setResolvedType(T1.symbol(), tm.getRegisteredType("int"));
	pass.setResolvedType(T2.symbol(), tm.getRegisteredType("float"));
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(tm.getConstraint(constraintID), &tm) > 0);
}

TEST_CASE("test resolve equals have t0", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
	pass.setResolvedType(T1.symbol(), tm.getRegisteredType("int"));
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve equals have t1", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
	pass.setResolvedType(T2.symbol(), tm.getRegisteredType("int"));
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(tm.getConstraint(constraintID), &tm));
	CHECK(resolver.resolveNext(tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve equals have neither", "[resolver]") {
	getDefaultTypeManager(tm);
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintID = tm.CreateEqualsConstraint(T1, T2);

	typecheck::ConstraintPass pass;
	auto resolver = typecheck::ResolveEquals(&pass, constraintID);
	CHECK(resolver.hasMoreSolutions(tm.getConstraint(constraintID), &tm));
	CHECK(!resolver.resolveNext(tm.getConstraint(constraintID), &tm));

	CHECK(resolver.score(tm.getConstraint(constraintID), &tm) > 0);
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

	CHECK(tm.getResolvedType(T1).name() == "int");
	CHECK(tm.getResolvedType(T2).name() == "int");
}


TEST_CASE("solve basic type float equals constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();

	auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	auto constraintT2 = tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
	auto constraintT3 = tm.CreateEqualsConstraint(T1, T2);

	CHECK(tm.solve());

	CHECK(tm.getResolvedType(T1).name() == "float");
	CHECK(tm.getResolvedType(T2).name() == "float");
}


TEST_CASE("solve basic type equals triangle constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	auto T3 = tm.CreateTypeVar();

	auto constraintT1 = tm.CreateEqualsConstraint(T1, T3);
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
	auto constraintT3 = tm.CreateEqualsConstraint(T1, T3);
	auto constraintT4 = tm.CreateEqualsConstraint(T1, T2);
	auto constraintT5 = tm.CreateEqualsConstraint(T2, T3);

	CHECK(tm.solve());
}

TEST_CASE("solve basic type conforms to triangle conversion solvable constraint", "[constraint]") {
	getDefaultTypeManager(tm);

	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	auto T3 = tm.CreateTypeVar();

	auto constraintT1 = tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	auto constraintT2 = tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
	auto constraintT3 = tm.CreateEqualsConstraint(T1, T3);
	auto constraintT4 = tm.CreateEqualsConstraint(T1, T2);
	auto constraintT5 = tm.CreateEqualsConstraint(T2, T3);

	CHECK(tm.solve());
}