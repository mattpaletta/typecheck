//
//  test_constraints.hpp
//  test_typecheck
//
//  Created by Matthew Paletta on 2020-07-24.
//
#pragma once
#include "utils.hpp"


TEST_CASE("test resolve bindto conflicting full", "[constraints]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateBindToConstraint(T1, tm.getRegisteredType("float"));

    REQUIRE(!tm.solve());
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

    const auto T = CreateMultipleSymbols(tm, 3);

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

    const auto T = CreateMultipleSymbols(tm, 4);

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

TEST_CASE("solve inferred function application constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 7);

    const auto T0FuncHash = std::hash<std::string>()(T.at(0).symbol());
    tm.CreateApplicableFunctionConstraint(T0FuncHash, {T.at(1), T.at(2)}, T.at(3));
    tm.CreateBindFunctionConstraint(T0FuncHash, T.at(0), {T.at(4), T.at(5)}, T.at(6));

    // 'internal' constraints to the function
    tm.CreateBindToConstraint(T.at(1), tm.getRegisteredType("int"));
    tm.CreateBindToConstraint(T.at(2), tm.getRegisteredType("float"));
    tm.CreateBindToConstraint(T.at(3), tm.getRegisteredType("double"));

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

TEST_CASE("solve inferred function application constraint no args", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 3);

    const auto T0FuncHash = std::hash<std::string>()(T.at(0).symbol());
    tm.CreateApplicableFunctionConstraint(T0FuncHash, {}, T.at(1));
    tm.CreateBindFunctionConstraint(T0FuncHash, T.at(0), {}, T.at(2));

    // 'internal' constraints to the function
    tm.CreateBindToConstraint(T.at(1), tm.getRegisteredType("double"));

    // T0 = (T1, T2) -> T3
    REQUIRE(tm.solve());
    CHECK(tm.getResolvedType(T.at(0)).has_func());
    REQUIRE(tm.getResolvedType(T.at(1)).has_raw());
    REQUIRE(tm.getResolvedType(T.at(2)).has_raw());

    CHECK(tm.getResolvedType(T.at(1)).raw().name() == "double");
    CHECK(tm.getResolvedType(T.at(2)).raw().name() == "double");
}

TEST_CASE("solve function different num args application constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 3);

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

    const auto T = CreateMultipleSymbols(tm, 3);

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

TEST_CASE("solve function infer args later constraint", "[constraint]") {
    getDefaultTypeManager(tm);
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 9);

    const auto functionID = tm.CreateFunctionHash("foo", {});
    tm.CreateApplicableFunctionConstraint(functionID, {}, T.at(0));

    tm.CreateBindToConstraint(T.at(2), tm.getRegisteredType("void"));
    tm.CreateBindToConstraint(T.at(3), tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateEqualsConstraint(T.at(4), T.at(3));
    tm.CreateEqualsConstraint(T.at(5), T.at(3));
    tm.CreateEqualsConstraint(T.at(6), T.at(5));
    tm.CreateEqualsConstraint(T.at(0), T.at(6));
    tm.CreateBindFunctionConstraint(functionID, T.at(8), {}, T.at(7));

    /*
     T8 = () -> T7
     T8() -> T7 {
        ...
        T6
     }
     */
    REQUIRE(tm.solve());
    REQUIRE(tm.getResolvedType(T.at(8)).has_func());
    REQUIRE(tm.getResolvedType(T.at(7)).has_raw());

    // Check it got bound to: func foo() -> Int
    REQUIRE(tm.getResolvedType(T.at(8)).func().has_returntype());
    REQUIRE(tm.getResolvedType(T.at(8)).func().returntype().has_raw());
    CHECK(tm.getResolvedType(T.at(8)).func().returntype().raw().name() == "int");
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


TEST_CASE("mutually-recursive solve for-loop constraints", "[constraint]") {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 16);
    /*
     Typecheck: Creating constraint: explicit { var { symbol: "T0" } type { raw { name: "void" } } }
     Typecheck: Creating constraint: id: 1 explicit { var { symbol: "T2" } type { raw { name: "void" } } }
     Typecheck: Creating constraint: id: 2 explicit { var { symbol: "T3" } type { raw { name: "int" } } }
     Typecheck: Creating constraint: kind: ConformsTo id: 3 conforms { protocol { literal: ExpressibleByInteger } type { symbol: "T4" } }
     Typecheck: Creating constraint: kind: Equal id: 4 types { first { symbol: "T4" } second { symbol: "T3" } }
     Typecheck: Creating constraint: kind: Equal id: 5 types { first { symbol: "T5" } second { symbol: "T1" } }
     Typecheck: Creating constraint: kind: ConformsTo id: 6 conforms { protocol { literal: ExpressibleByInteger } type { symbol: "T6" } }
     Typecheck: Creating constraint: kind: Equal id: 7 types { first { symbol: "T5" } second { symbol: "T6" } }
     Typecheck: Creating constraint: kind: Equal id: 8 types { first { symbol: "T7" } second { symbol: "T5" } }
     Typecheck: Creating constraint: id: 9 explicit { var { symbol: "T8" } type { raw { name: "bool" } } }
     Typecheck: Creating constraint: kind: Equal id: 10 types { first { symbol: "T9" } second { symbol: "T1" } }
     Typecheck: Creating constraint: kind: ConformsTo id: 11 conforms { protocol { literal: ExpressibleByInteger } type { symbol: "T10" } }
     Typecheck: Creating constraint: kind: Equal id: 12 types { first { symbol: "T9" } second { symbol: "T10" } }
     Typecheck: Creating constraint: kind: Equal id: 13 types { first { symbol: "T11" } second { symbol: "T9" } }
     Typecheck: Creating constraint: id: 14 explicit { var { symbol: "T8" } type { raw { name: "bool" } } }
     Typecheck: Creating constraint: kind: Equal id: 15 types { first { symbol: "T3" } second { symbol: "T11" } }
     Typecheck: Creating constraint: kind: Equal id: 16 types { first { symbol: "T1" } second { symbol: "T11" } }
     Typecheck: Creating constraint: id: 17 explicit { var { symbol: "T12" } type { raw { name: "void" } } }
     Typecheck: Creating constraint: id: 18 explicit { var { symbol: "T13" } type { raw { name: "int" } } }
     Typecheck: Creating constraint: kind: ConformsTo id: 19 conforms { protocol { literal: ExpressibleByInteger } type { symbol: "T14" } }
     Typecheck: Creating constraint: kind: Equal id: 20 types { first { symbol: "T14" } second { symbol: "T13" } }
     Typecheck: Creating constraint: id: 21 explicit { var { symbol: "T15" } type { raw { name: "void" } } }
     */

    const auto intType = tm.getRegisteredType("int");
    const auto voidType = tm.getRegisteredType("void");
    const auto boolType = tm.getRegisteredType("bool");

    tm.CreateBindToConstraint(T.at(0), voidType);
    tm.CreateBindToConstraint(T.at(2), voidType);
    tm.CreateBindToConstraint(T.at(3), intType);
    tm.CreateBindToConstraint(T.at(8), boolType);
    tm.CreateBindToConstraint(T.at(12), voidType);
    tm.CreateBindToConstraint(T.at(13), intType);
    tm.CreateBindToConstraint(T.at(15), voidType);

    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(6), typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(10), typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(14), typecheck::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);

    tm.CreateEqualsConstraint(T.at(4), T.at(3));
    tm.CreateEqualsConstraint(T.at(5), T.at(1));
    tm.CreateEqualsConstraint(T.at(5), T.at(6));
    tm.CreateEqualsConstraint(T.at(7), T.at(5));
    tm.CreateEqualsConstraint(T.at(9), T.at(1));
    tm.CreateEqualsConstraint(T.at(9), T.at(10));
    tm.CreateEqualsConstraint(T.at(11), T.at(9));
    tm.CreateEqualsConstraint(T.at(3), T.at(11));
    tm.CreateEqualsConstraint(T.at(1), T.at(11));
    tm.CreateEqualsConstraint(T.at(14), T.at(13));

    REQUIRE(tm.solve());
    for (std::size_t i = 0; i < 16; ++i) {
        REQUIRE(tm.getResolvedType(T.at(i)).has_raw());
    }
    CHECK(tm.getResolvedType(T.at(0)).raw().name()  == "void");
    CHECK(tm.getResolvedType(T.at(1)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(2)).raw().name()  == "void");
    CHECK(tm.getResolvedType(T.at(3)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(4)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(5)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(6)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(7)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(8)).raw().name()  == "bool");
    CHECK(tm.getResolvedType(T.at(9)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(10)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(11)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(12)).raw().name()  == "void");
    CHECK(tm.getResolvedType(T.at(13)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(14)).raw().name()  == "int");
    CHECK(tm.getResolvedType(T.at(15)).raw().name()  == "void");
}
