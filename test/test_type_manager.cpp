//
//  test_type_manager.hpp
//  test_typecheck
//
//  Created by Matthew Paletta on 2020-07-24.
//
#include "test_include_catch.hpp"
#include "utils.hpp"

TEST_CASE("create function hash no args", "[type_manager]") {
    typecheck::TypeManager tm;
    CHECK(tm.CreateFunctionHash("foo", {}) != 0);
}

TEST_CASE("create lambda hash no args", "[type_manager]") {
    typecheck::TypeManager tm;
    CHECK(tm.CreateLambdaFunctionHash({}) != 0);
}

TEST_CASE("create function hash 1 args", "[type_manager]") {
    typecheck::TypeManager tm;
    CHECK(tm.CreateFunctionHash("foo", {"a"}) != 0);
}

TEST_CASE("create lambda hash 1 arg", "[type_manager]") {
    typecheck::TypeManager tm;
    CHECK(tm.CreateLambdaFunctionHash({"a"}) != 0);
}

TEST_CASE("create function hash 2 args", "[type_manager]") {
    typecheck::TypeManager tm;
    CHECK(tm.CreateFunctionHash("foo", {"a", "b"}) != 0);
}

TEST_CASE("create lambda hash 2 args", "[type_manager]") {
    typecheck::TypeManager tm;
    CHECK(tm.CreateLambdaFunctionHash({"a", "b"}) != 0);
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

TEST_CASE("test split to groups individual", "[type_solver]") {
	getDefaultTypeManager(tm);
	const auto intType = tm.getRegisteredType("int");
	const auto voidType = tm.getRegisteredType("void");
	const auto doubleType = tm.getRegisteredType("double");

	const auto T = CreateMultipleSymbols(tm, 10);

	// Group 1
	const auto c1 = tm.CreateBindToConstraint(T.at(0), intType);
	const auto c2 = tm.CreateBindToConstraint(T.at(1), intType);
	const auto c3 = tm.CreateBindToConstraint(T.at(2), doubleType);

	struct A : public typecheck::TypeSolver {
		virtual std::vector<typecheck::ConstraintGroup> SplitToGroups(const typecheck::TypeManager* manager) const override {
			return typecheck::TypeSolver::SplitToGroups(manager);
		}
	};

	A ts;
	const auto groups = ts.SplitToGroups(&tm);
	REQUIRE(groups.size() == 3);

	REQUIRE(groups.at(0).size() == 1);
	CHECK(groups.at(0).contains(c1));

	REQUIRE(groups.at(1).size() == 1);
	CHECK(groups.at(1).contains(c2));

	REQUIRE(groups.at(2).size() == 1);
	CHECK(groups.at(2).contains(c3));
}

TEST_CASE("test split to groups 2 groups", "[type_solver]") {
	getDefaultTypeManager(tm);
	const auto intType = tm.getRegisteredType("int");
	const auto voidType = tm.getRegisteredType("void");
	const auto doubleType = tm.getRegisteredType("double");

	const auto T = CreateMultipleSymbols(tm, 10);

	// Group 1
	const auto c1 = tm.CreateBindToConstraint(T.at(0), intType);
	const auto c2 = tm.CreateEqualsConstraint(T.at(0), T.at(1));
	const auto c3 = tm.CreateEqualsConstraint(T.at(1), T.at(2));

	const auto c4 = tm.CreateBindToConstraint(T.at(3), doubleType);
	const auto c5 = tm.CreateEqualsConstraint(T.at(3), T.at(4));
	const auto c6 = tm.CreateEqualsConstraint(T.at(4), T.at(5));

	struct A : public typecheck::TypeSolver {
		virtual std::vector<typecheck::ConstraintGroup> SplitToGroups(const typecheck::TypeManager* manager) const override {
			return typecheck::TypeSolver::SplitToGroups(manager);
		}
	};

	A ts;
	const auto groups = ts.SplitToGroups(&tm);
	REQUIRE(groups.size() == 2);
	REQUIRE(groups.at(0).size() == 3);
	CHECK(groups.at(0).contains(c1));
	CHECK(groups.at(0).contains(c2));
	CHECK(groups.at(0).contains(c3));

	CHECK(!groups.at(0).contains(c4));
	CHECK(!groups.at(0).contains(c5));
	CHECK(!groups.at(0).contains(c6));

	REQUIRE(groups.at(1).size() == 3);
	CHECK(!groups.at(1).contains(c1));
	CHECK(!groups.at(1).contains(c2));
	CHECK(!groups.at(1).contains(c3));

	CHECK(groups.at(1).contains(c4));
	CHECK(groups.at(1).contains(c5));
	CHECK(groups.at(1).contains(c6));
}

TEST_CASE("test split conforms", "[type_solver]") {
	getDefaultTypeManager(tm);
	const auto intType = tm.getRegisteredType("int");
	const auto voidType = tm.getRegisteredType("void");
	const auto doubleType = tm.getRegisteredType("double");

	const auto T = CreateMultipleSymbols(tm, 10);

	// Group 1
	const auto c1 = tm.CreateBindToConstraint(T.at(0), intType);
	const auto c2 = tm.CreateEqualsConstraint(T.at(0), T.at(1));

	// Group 2
	const auto c3 = tm.CreateLiteralConformsToConstraint(T.at(2), typecheck::KnownProtocolKind::ExpressibleByInteger);

	// Group 3
	const auto c4 = tm.CreateBindToConstraint(T.at(3), doubleType);
	const auto c5 = tm.CreateEqualsConstraint(T.at(3), T.at(4));
	const auto c6 = tm.CreateEqualsConstraint(T.at(4), T.at(5));

	struct A : public typecheck::TypeSolver {
		virtual std::vector<typecheck::ConstraintGroup> SplitToGroups(const typecheck::TypeManager* manager) const override {
			return typecheck::TypeSolver::SplitToGroups(manager);
		}
	};

	A ts;
	const auto groups = ts.SplitToGroups(&tm);
	REQUIRE(groups.size() == 3);

	REQUIRE(groups.at(0).size() == 2);
	CHECK(groups.at(0).contains(c1));
	CHECK(groups.at(0).contains(c2));

	REQUIRE(groups.at(1).size() == 1);
	CHECK(groups.at(1).contains(c3));

	REQUIRE(groups.at(2).size() == 3);
	CHECK(groups.at(2).contains(c4));
	CHECK(groups.at(2).contains(c5));
	CHECK(groups.at(2).contains(c6));
}

TEST_CASE("test split overload", "[type_solver]") {
	getDefaultTypeManager(tm);
	const auto intType = tm.getRegisteredType("int");
	const auto voidType = tm.getRegisteredType("void");
	const auto doubleType = tm.getRegisteredType("double");

	const auto T = CreateMultipleSymbols(tm, 10);

	// Group 1
	const auto c1 = tm.CreateBindToConstraint(T.at(0), intType);
	const auto c2 = tm.CreateEqualsConstraint(T.at(0), T.at(1));
	const auto c3 = tm.CreateBindFunctionConstraint(1234, T.at(2), {T.at(3), T.at(4)}, T.at(0));

	// Group 2
	const auto c4 = tm.CreateBindToConstraint(T.at(5), doubleType);
	const auto c5 = tm.CreateEqualsConstraint(T.at(5), T.at(6));
	const auto c6 = tm.CreateEqualsConstraint(T.at(6), T.at(7));

	struct A : public typecheck::TypeSolver {
		virtual std::vector<typecheck::ConstraintGroup> SplitToGroups(const typecheck::TypeManager* manager) const override {
			return typecheck::TypeSolver::SplitToGroups(manager);
		}
	};

	A ts;
	const auto groups = ts.SplitToGroups(&tm);
	REQUIRE(groups.size() == 2);
	REQUIRE(groups.at(0).size() == 3);
	CHECK(groups.at(0).contains(c1));
	CHECK(groups.at(0).contains(c2));
	CHECK(groups.at(0).contains(c3));

	REQUIRE(groups.at(1).size() == 3);
	CHECK(groups.at(1).contains(c4));
	CHECK(groups.at(1).contains(c5));
	CHECK(groups.at(1).contains(c6));
}

TEST_CASE("test merge groups", "[type_solver]") {
	getDefaultTypeManager(tm);
	const auto intType = tm.getRegisteredType("int");
	const auto voidType = tm.getRegisteredType("void");
	const auto doubleType = tm.getRegisteredType("double");

	const auto T = CreateMultipleSymbols(tm, 10);

	// Group 1
	const auto c1 = tm.CreateBindToConstraint(T.at(0), intType);
	const auto c2 = tm.CreateEqualsConstraint(T.at(0), T.at(1));
	const auto c3 = tm.CreateEqualsConstraint(T.at(1), T.at(2));

	// Group 2
	const auto c4 = tm.CreateBindToConstraint(T.at(5), doubleType);
	const auto c5 = tm.CreateEqualsConstraint(T.at(5), T.at(6));
	const auto c6 = tm.CreateEqualsConstraint(T.at(6), T.at(7));

	// This one will merge the other 2 groups
	const auto c7 = tm.CreateBindFunctionConstraint(1234, T.at(3), {T.at(2), T.at(8)}, T.at(7));

	struct A : public typecheck::TypeSolver {
		virtual std::vector<typecheck::ConstraintGroup> SplitToGroups(const typecheck::TypeManager* manager) const override {
			return typecheck::TypeSolver::SplitToGroups(manager);
		}
	};

	A ts;
	const auto groups = ts.SplitToGroups(&tm);
	REQUIRE(groups.size() == 1);
	REQUIRE(groups.at(0).size() == 7);
	CHECK(groups.at(0).contains(c1));
	CHECK(groups.at(0).contains(c2));
	CHECK(groups.at(0).contains(c3));
	CHECK(groups.at(0).contains(c4));
	CHECK(groups.at(0).contains(c5));
	CHECK(groups.at(0).contains(c6));
	CHECK(groups.at(0).contains(c7));
}
