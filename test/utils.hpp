//
//  utils.hpp
//  test_typecheck
//
//  Created by Matthew Paletta on 2020-07-24.
//
#pragma once

#include <typecheck/type_manager.hpp>
#include <typecheck/generic_type_generator.hpp>

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

auto CreateMultipleSymbols(typecheck::TypeManager& tm, const std::size_t& num) -> std::vector<typecheck::TypeVar> {
    std::vector<typecheck::TypeVar> out;
    for (std::size_t i = 0; i < num; ++i) {
        out.push_back(tm.CreateTypeVar());
    }
    return out;
}

#define getDefaultTypeManager(tm) \
typecheck::TypeManager tm; \
setupTypeManager(&tm)
