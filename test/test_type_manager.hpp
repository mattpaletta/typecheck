//
//  test_type_manager.hpp
//  test_typecheck
//
//  Created by Matthew Paletta on 2020-07-24.
//
#pragma once
#include "utils.hpp"

TEST_CASE("test resolve bindto conflicting full", "[type_manager]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateBindToConstraint(T1, tm.getRegisteredType("float"));

    REQUIRE(!tm.solve());
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
