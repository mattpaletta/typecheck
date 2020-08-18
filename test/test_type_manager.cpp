//
//  test_type_manager.hpp
//  test_typecheck
//
//  Created by Matthew Paletta on 2020-07-24.
//
#include "test_include_catch.hpp"

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
