#pragma once
#include "utils.hpp"

#include <typecheck/resolvers/ResolveConformsTo.hpp>
#include <typecheck/resolvers/ResolveEquals.hpp>
#include <typecheck/resolvers/ResolveBindTo.hpp>

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
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("int"), &tm);
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
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("string"), &tm);
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
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("float"), &tm);
    auto resolver = typecheck::ResolveConformsTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 1);
}

TEST_CASE("test resolve equals have both", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T = CreatMultipleSymbols(tm, 2);

    auto constraintID = tm.CreateEqualsConstraint(T.at(0), T.at(1));

    typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    typecheck::Constraint constraint1;
    constraint1.set_kind(typecheck::ConstraintKind::Bind);
    constraint1.set_id(2);
    pass.setResolvedType(constraint, T.at(0), tm.getRegisteredType("int"), &tm);
    pass.setResolvedType(constraint1, T.at(1), tm.getRegisteredType("int"), &tm);
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
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("int"), &tm);
    auto resolver = typecheck::ResolveBindTo(&pass, constraintID);
    CHECK(resolver.hasMoreSolutions(*tm.getConstraint(constraintID), &tm));
    CHECK(resolver.resolveNext(*tm.getConstraint(constraintID), &tm));

    CHECK(resolver.score(*tm.getConstraint(constraintID), &tm) == 0);
}

TEST_CASE("test resolve bindto conflicting", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    auto constraint1 = tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    auto constraint2 = tm.CreateBindToConstraint(T1, tm.getRegisteredType("float"));

    typecheck::ConstraintPass pass;
    auto resolver1 = typecheck::ResolveBindTo(&pass, constraint1);
    auto resolver2 = typecheck::ResolveBindTo(&pass, constraint2);

    CHECK(resolver1.hasMoreSolutions(*tm.getConstraint(constraint1), &tm));
    CHECK(resolver2.hasMoreSolutions(*tm.getConstraint(constraint2), &tm));

    CHECK( resolver1.resolveNext(*tm.getConstraint(constraint1), &tm));
    CHECK(!resolver2.resolveNext(*tm.getConstraint(constraint2), &tm));

    CHECK(resolver1.score(*tm.getConstraint(constraint1), &tm) == 0);
    CHECK(resolver2.score(*tm.getConstraint(constraint2), &tm) > 0);
}

TEST_CASE("test resolve equals have both not equal", "[resolver]") {
    getDefaultTypeManager(tm);
    auto T = CreatMultipleSymbols(tm, 2);

    auto constraintID = tm.CreateEqualsConstraint(T.at(0), T.at(1));

    typecheck::ConstraintPass pass;
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    typecheck::Constraint constraint1;
    constraint1.set_kind(typecheck::ConstraintKind::Bind);
    constraint1.set_id(2);
    pass.setResolvedType(constraint, T.at(0), tm.getRegisteredType("int"), &tm);
    pass.setResolvedType(constraint1, T.at(1), tm.getRegisteredType("float"), &tm);
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
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T1, tm.getRegisteredType("int"), &tm);
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
    typecheck::Constraint constraint;
    constraint.set_kind(typecheck::ConstraintKind::Bind);
    constraint.set_id(1);
    pass.setResolvedType(constraint, T2, tm.getRegisteredType("int"), &tm);
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
