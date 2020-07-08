#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <iostream>

#include "type_manager.hpp"
#include "generic_type_generator.hpp"

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
#define getDefaultTypeManager(tm) \
	typecheck::TypeManager tm; \
	CHECK(tm.registerType("int")); \
	CHECK(tm.registerType("float")); \
	CHECK(tm.registerType("double")); \
	CHECK(tm.setConvertible("int", "double")); \
	CHECK(tm.setConvertible("int", "float")); \
	CHECK(tm.setConvertible("float", "double"));

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



TEST_CASE("solve basic type conversions", "[type_manager]") {
	getDefaultTypeManager(tm)

	// let a   =  1   +   2;
	//     T4  T3 T2  T0  T1
	// auto T0 = tm.CreateTypeVar();
	auto T1 = tm.CreateTypeVar();
	auto T2 = tm.CreateTypeVar();
	//auto T3 = tm.CreateTypeVar();
	//auto T4 = tm.CreateTypeVar();

	/*
	// foo(T1, T2) -> T3
	typecheck::FunctionDefinition fooFunc;
	fooFunc.set_name("foo");
	fooFunc.mutable_returntype()->set_name(T3->name());
	auto* arg1 = fooFunc.add_argtypes();
	arg1->set_name(T1->name());
	auto* arg2 = fooFunc.add_argtypes();
	arg2->set_name(T2->name());
	tm.registerFunctionDefinition(fooFunc);
	*/

	typecheck::KnownProtocolKind intType;
	intType.set_literal(typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
	// Look them up again, because the pointers to T1, T2 could change as the vector resizes
	auto constraintT1 = tm.CreateConformsToConstraint(tm.getRegisteredType(T1), intType);
	auto constraintT2 = tm.CreateConformsToConstraint(tm.getRegisteredType(T2), intType);
	auto constraintT3 = tm.CreateEqualsConstraint(tm.getRegisteredType(T1), tm.getRegisteredType(T2));

	tm.solve();
	CHECK(tm.getResolvedType(tm.getRegisteredType(T1)).name() == "int");
	CHECK(tm.getResolvedType(tm.getRegisteredType(T2)).name() == "int");
}
