#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include "typecheck.hpp"

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
