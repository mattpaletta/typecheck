//
//  test_constraints.hpp
//  test_typecheck
//
//  Created by Matthew Paletta on 2020-07-24.
//
#include "test_include_catch.hpp"

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

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
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

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::ExpressibleByFloat);
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

    REQUIRE(tm.solve());
}

TEST_CASE("solve basic type equals triangle constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 3);

    tm.CreateEqualsConstraint(T.at(2) , T.at(0));
    tm.CreateEqualsConstraint(T.at(0), T.at(1));
    tm.CreateEqualsConstraint(T.at(1), T.at(2));

    REQUIRE(tm.solve());
}

TEST_CASE("solve basic type conforms to triangle solvable constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
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

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByFloat);
    tm.CreateEqualsConstraint(T1, T2);
    tm.CreateEqualsConstraint(T2, T3);
    tm.CreateEqualsConstraint(T3, T1);

    REQUIRE(tm.solve());
}

TEST_CASE("solve convertible constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateConvertibleConstraint(T1, T2);

    REQUIRE(tm.solve());
}

TEST_CASE("solve convertible reverse constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::ExpressibleByFloat);
    tm.CreateConvertibleConstraint(T2, T1);

    REQUIRE(tm.solve());
}

TEST_CASE("solve convertible bind reverse constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::ExpressibleByFloat);
    tm.CreateConvertibleConstraint(T1, T2);

    REQUIRE(tm.solve());
}

TEST_CASE("solve convertible conversion explicit constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::ExpressibleByFloat);
    tm.CreateBindToConstraint(T2, tm.getRegisteredType("float"));
    tm.CreateEqualsConstraint(T2, T1);

    REQUIRE(tm.solve());

    CHECK(tm.getResolvedType(T1).raw().name() == "float");
    CHECK(tm.getResolvedType(T2).raw().name() == "float");
}

TEST_CASE("solve function application constraint", "[constraint]") {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 8);

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
    // (T3) -> T4
    tm.CreateApplicableFunctionConstraint(tm.CreateFunctionHash("foo", {"a"}), { tm.getRegisteredType("int") }, tm.getRegisteredType("double"));

    // func foo(a: Float) -> Double
    // (T5) -> T6
    tm.CreateApplicableFunctionConstraint(tm.CreateFunctionHash("foo", {"a"}), { tm.getRegisteredType("float") }, tm.getRegisteredType("double"));

    tm.CreateBindFunctionConstraint(tm.CreateFunctionHash("foo", {"a"}), T.at(0), { T.at(1) }, T.at(2));
    tm.CreateLiteralConformsToConstraint(T.at(1), typecheck::KnownProtocolKind::ExpressibleByInteger);

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
    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind::ExpressibleByInteger);
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

TEST_CASE("solve for-loop constraints regression", "[constraint]") {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    auto T0 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();
    auto T4 = tm.CreateTypeVar();
    auto T5 = tm.CreateTypeVar();
    auto T7 = tm.CreateTypeVar();
    auto T9 = tm.CreateTypeVar();

    tm.CreateEqualsConstraint(T0, T3);
    tm.CreateEqualsConstraint(T3, T4);
	tm.CreateLiteralConformsToConstraint(T4, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T5, T3);
    tm.CreateEqualsConstraint(T7, T0);
    tm.CreateEqualsConstraint(T9, T7);

    REQUIRE(tm.solve());
    REQUIRE(tm.getResolvedType(T0).has_raw());
    REQUIRE(tm.getResolvedType(T3).has_raw());
    REQUIRE(tm.getResolvedType(T4).has_raw());
    REQUIRE(tm.getResolvedType(T5).has_raw());
    REQUIRE(tm.getResolvedType(T7).has_raw());
    REQUIRE(tm.getResolvedType(T9).has_raw());

    CHECK(tm.getResolvedType(T0).raw().name() == "int");
    CHECK(tm.getResolvedType(T3).raw().name() == "int");
    CHECK(tm.getResolvedType(T4).raw().name() == "int");
    CHECK(tm.getResolvedType(T5).raw().name() == "int");
    CHECK(tm.getResolvedType(T7).raw().name() == "int");
    CHECK(tm.getResolvedType(T9).raw().name() == "int");
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
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T3, T4);
    tm.CreateLiteralConformsToConstraint(T4, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T5, T3);
    tm.CreateBindToConstraint(T6, tm.getRegisteredType("bool"));
    tm.CreateEqualsConstraint(T7, T0);
    tm.CreateLiteralConformsToConstraint(T8, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T9, T7);
    tm.CreateBindToConstraint(T10, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T11, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateBindToConstraint(T12, tm.getRegisteredType("void"));

    REQUIRE(tm.solve());
    REQUIRE(tm.getResolvedType(T0).has_raw());
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

    CHECK(tm.getResolvedType(T0).raw().name() == "int");
    CHECK(tm.getResolvedType(T1).raw().name() == "int");
    CHECK(tm.getResolvedType(T2).raw().name() == "int");
    CHECK(tm.getResolvedType(T3).raw().name() == "int");
    CHECK(tm.getResolvedType(T4).raw().name() == "int");
    CHECK(tm.getResolvedType(T5).raw().name() == "int");
    CHECK(tm.getResolvedType(T6).raw().name() == "bool");
    CHECK(tm.getResolvedType(T7).raw().name() == "int");
    CHECK(tm.getResolvedType(T8).raw().name() == "int");
    CHECK(tm.getResolvedType(T9).raw().name() == "int");
    CHECK(tm.getResolvedType(T10).raw().name() == "int");
    CHECK(tm.getResolvedType(T11).raw().name() == "int");
    CHECK(tm.getResolvedType(T12).raw().name() == "void");
}

TEST_CASE("multiple independent statements", "[constraint]") {
    getDefaultTypeManager(tm);
    const auto T = CreateMultipleSymbols(tm, 12);
    const auto intType = tm.getRegisteredType("int");

    tm.CreateLiteralConformsToConstraint(T.at(3), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(6), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(7), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateBindToConstraint(T.at(0), intType);

    tm.CreateEqualsConstraint(T.at(3), T.at(4));
    tm.CreateEqualsConstraint(T.at(5), T.at(3));
    tm.CreateEqualsConstraint(T.at(6), T.at(7));
    tm.CreateEqualsConstraint(T.at(8), T.at(6));
    tm.CreateEqualsConstraint(T.at(9), T.at(8));
    tm.CreateEqualsConstraint(T.at(10), T.at(9));

    const auto funcHash = tm.CreateFunctionHash("foo", {});

    tm.CreateApplicableFunctionConstraint(funcHash, {}, T.at(0));
    tm.CreateBindFunctionConstraint(funcHash, T.at(11), {}, T.at(10));
    REQUIRE(tm.solve());
}

TEST_CASE("mutually-recursive solve for-loop constraints", "[constraint]") {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 16);
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

    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(6), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(10), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(14), typecheck::KnownProtocolKind::ExpressibleByInteger);

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

TEST_CASE("regression test 1 constraints", "[constraint]") {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 23);
    const auto intType = tm.getRegisteredType("int");
    const auto voidType = tm.getRegisteredType("void");
    const auto boolType = tm.getRegisteredType("bool");

//    T7: int
//    T10 == T7
//    T11: ExpressibleByInt
//    T10 == T11
//    T12 == T10
//    T21 == T7
//    T21 == T22
//    T22: ExpressibleByInteger
    tm.CreateBindToConstraint(T.at(7), intType);
    tm.CreateEqualsConstraint(T.at(10), T.at(7));
    tm.CreateLiteralConformsToConstraint(T.at(11), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T.at(10), T.at(11));
    tm.CreateEqualsConstraint(T.at(12), T.at(10));
    tm.CreateEqualsConstraint(T.at(21), T.at(7));
    tm.CreateEqualsConstraint(T.at(21), T.at(22));
    tm.CreateLiteralConformsToConstraint(T.at(22), typecheck::KnownProtocolKind::ExpressibleByInteger);
    REQUIRE(tm.solve());
    REQUIRE(tm.getResolvedType(T.at(11)).has_raw());
    CHECK(tm.getResolvedType(T.at(11)).raw().name()  == "int");
}

TEST_CASE("regression test 2 constraints (ackerman)", "[constraint]") {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("int");
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 94);
    const auto intType = tm.getRegisteredType("int");
    const auto voidType = tm.getRegisteredType("void");
    const auto boolType = tm.getRegisteredType("bool");

	tm.CreateBindToConstraint(T.at(1), intType);
	tm.CreateBindToConstraint(T.at(2), voidType);
	tm.CreateBindToConstraint(T.at(3), intType);
	tm.CreateBindToConstraint(T.at(4), voidType);
	tm.CreateBindToConstraint(T.at(5), intType);
	tm.CreateEqualsConstraint(T.at(9), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(10), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(9), T.at(10));
	tm.CreateEqualsConstraint(T.at(11), T.at(9));
	tm.CreateBindToConstraint(T.at(12), boolType);
	tm.CreateEqualsConstraint(T.at(9), T.at(10));
	tm.CreateEqualsConstraint(T.at(13), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(14), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(13), T.at(14));
	tm.CreateEqualsConstraint(T.at(15), T.at(13));
	tm.CreateEqualsConstraint(T.at(16), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(17), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(16), T.at(17));
	tm.CreateEqualsConstraint(T.at(18), T.at(16));
	tm.CreateBindToConstraint(T.at(19), boolType);
	tm.CreateEqualsConstraint(T.at(16), T.at(17));
	tm.CreateEqualsConstraint(T.at(22), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(23), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(22), T.at(23));
	tm.CreateEqualsConstraint(T.at(24), T.at(22));
	tm.CreateEqualsConstraint(T.at(25), T.at(24));
	tm.CreateLiteralConformsToConstraint(T.at(26), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(27), T.at(26));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(21), {T.at(25),T.at(27)}, T.at(20));
	tm.CreateEqualsConstraint(T.at(28), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(29), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(28), T.at(29));
	tm.CreateEqualsConstraint(T.at(30), T.at(28));
	tm.CreateEqualsConstraint(T.at(31), T.at(30));
	tm.CreateLiteralConformsToConstraint(T.at(32), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(33), T.at(32));
	tm.CreateEqualsConstraint(T.at(36), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(37), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(36), T.at(37));
	tm.CreateEqualsConstraint(T.at(38), T.at(36));
	tm.CreateEqualsConstraint(T.at(39), T.at(38));
	tm.CreateEqualsConstraint(T.at(42), T.at(3));
	tm.CreateEqualsConstraint(T.at(43), T.at(42));
	tm.CreateEqualsConstraint(T.at(44), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(45), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(44), T.at(45));
	tm.CreateEqualsConstraint(T.at(46), T.at(44));
	tm.CreateEqualsConstraint(T.at(47), T.at(46));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(41), {T.at(43),T.at(47)}, T.at(40));
	tm.CreateEqualsConstraint(T.at(48), T.at(3));
	tm.CreateEqualsConstraint(T.at(49), T.at(48));
	tm.CreateEqualsConstraint(T.at(50), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(51), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(50), T.at(51));
	tm.CreateEqualsConstraint(T.at(52), T.at(50));
	tm.CreateEqualsConstraint(T.at(53), T.at(52));
	tm.CreateEqualsConstraint(T.at(54), T.at(40));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(35), {T.at(39),T.at(54)}, T.at(34));
	tm.CreateEqualsConstraint(T.at(55), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(56), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(55), T.at(56));
	tm.CreateEqualsConstraint(T.at(57), T.at(55));
	tm.CreateEqualsConstraint(T.at(58), T.at(57));
	tm.CreateEqualsConstraint(T.at(61), T.at(3));
	tm.CreateEqualsConstraint(T.at(62), T.at(61));
	tm.CreateEqualsConstraint(T.at(63), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(64), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(63), T.at(64));
	tm.CreateEqualsConstraint(T.at(65), T.at(63));
	tm.CreateEqualsConstraint(T.at(66), T.at(65));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(60), {T.at(62),T.at(66)}, T.at(59));
	tm.CreateEqualsConstraint(T.at(67), T.at(3));
	tm.CreateEqualsConstraint(T.at(68), T.at(67));
	tm.CreateEqualsConstraint(T.at(69), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(70), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(69), T.at(70));
	tm.CreateEqualsConstraint(T.at(71), T.at(69));
	tm.CreateEqualsConstraint(T.at(72), T.at(71));
	tm.CreateEqualsConstraint(T.at(73), T.at(59));
	tm.CreateBindToConstraint(T.at(74), boolType);
	tm.CreateEqualsConstraint(T.at(74), T.at(19));
	tm.CreateBindToConstraint(T.at(75), voidType);
	tm.CreateBindToConstraint(T.at(76), boolType);
	tm.CreateEqualsConstraint(T.at(76), T.at(12));
	tm.CreateBindToConstraint(T.at(77), voidType);
	tm.CreateEqualsConstraint(T.at(8), T.at(77));
	tm.CreateEqualsConstraint(T.at(8), T.at(77));
	tm.CreateEqualsConstraint(T.at(7), T.at(8));
	tm.CreateEqualsConstraint(T.at(1), T.at(7));
	tm.CreateLiteralConformsToConstraint(T.at(83), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(84), T.at(83));
	tm.CreateLiteralConformsToConstraint(T.at(85), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(86), T.at(85));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(82), {T.at(84),T.at(86)}, T.at(81));
	tm.CreateLiteralConformsToConstraint(T.at(87), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(88), T.at(87));
	tm.CreateLiteralConformsToConstraint(T.at(89), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(90), T.at(89));
	tm.CreateEqualsConstraint(T.at(91), T.at(81));
	tm.CreateEqualsConstraint(T.at(79), T.at(91));
	tm.CreateBindFunctionConstraint(-1993622415222145992, T.at(93), {}, T.at(92));

	REQUIRE(tm.solve());
}

void RunStressTest(const std::size_t numSymbols) {
	getDefaultTypeManager(tm);
	tm.registerType("bool");
	tm.registerType("void");

	const auto T = CreateMultipleSymbols(tm, numSymbols);
	const auto intType = tm.getRegisteredType("int");
	const auto voidType = tm.getRegisteredType("void");
	const auto boolType = tm.getRegisteredType("bool");

	for (std::size_t i = 0; i < numSymbols; ++i) {
		if (i % (numSymbols / 5) == 0) {
			tm.CreateBindToConstraint(T.at(i), intType);
		} else if (i % (numSymbols / 3) == 0) {
			tm.CreateLiteralConformsToConstraint(T.at(i), typecheck::KnownProtocolKind::ExpressibleByInteger);
		}

		tm.CreateEqualsConstraint(T.at(i), T.at((i + 1) % numSymbols));
	}
	REQUIRE(tm.solve());
}

#define CREATE_STRESS_TEST(numSymbols) TEST_CASE("stress test " + std::to_string(numSymbols) + " constraints", "[constraint]") { RunStressTest(numSymbols); }


CREATE_STRESS_TEST(100)
CREATE_STRESS_TEST(200)
CREATE_STRESS_TEST(300)
CREATE_STRESS_TEST(400)
CREATE_STRESS_TEST(800)
