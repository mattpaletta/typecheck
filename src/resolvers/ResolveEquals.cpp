//
//  ResolveEquals.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveEquals.hpp"
#include "typecheck/debug.hpp"

using namespace typecheck;

ResolveEquals::ResolveEquals(const ConstraintPass::IDType _id) : Resolver(ConstraintKind::Equal, _id) {}

auto ResolveEquals::clone(const ConstraintPass::IDType _id) const -> std::unique_ptr<Resolver> {
    return std::make_unique<ResolveEquals>(_id);
}

auto ResolveEquals::is_valid_constraint(const Constraint& constraint, [[maybe_unused]] ConstraintPass* pass) const -> bool {
    return constraint.has_types() && constraint.types().has_first() && constraint.types().has_second();
}

auto ResolveEquals::hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) -> bool {
    return pass && !this->has_gotten_resolve && this->is_valid_constraint(constraint, pass);
}

auto ResolveEquals::readyToResolve([[maybe_unused]] const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) const -> bool {
    if (!pass) {
        return false;
    }

    const auto& T0 = constraint.types().first();
    const auto& T1 = constraint.types().second();

    const bool hasT0 = pass->hasResolvedType(T0);
    const bool hasT1 = pass->hasResolvedType(T1);

    return hasT0 || hasT1;
}

auto ResolveEquals::resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) -> bool {
    this->has_gotten_resolve = true;
    const auto& T0 = constraint.types().first();
    const auto& T1 = constraint.types().second();

    if (pass) {
        const bool hasT0 = pass->hasResolvedType(T0);
        const bool hasT1 = pass->hasResolvedType(T1);

        const bool hasT0Permission = pass->HasPermission(constraint, T0, manager);
        const bool hasT1Permission = pass->HasPermission(constraint, T1, manager);

        if (hasT0 && hasT1) {
            // We will determine if they are the same in the 'score'
            return true;

        } else if (hasT0 && hasT1Permission) {
            // Don't have T1
            return pass->setResolvedType(constraint, T1, pass->getResolvedType(T0), manager);

        } else if (hasT1 && hasT0Permission) {
            // Don't have T0
            return pass->setResolvedType(constraint, T0, pass->getResolvedType(T1), manager);

        } else {
            // Don't have either, can't resolve
            // Or don't have permission to set the other
            return false;
        }

    } else {
        // Don't have a pass, can't resolve.
        return false;
    }
}

auto ResolveEquals::score(const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    if (!this->is_valid_constraint(constraint, pass)) {
        return std::numeric_limits<std::size_t>::max();
    }

    const auto& T0Var = constraint.types().first();
    const auto& T1Var = constraint.types().second();

    if (pass && pass->hasResolvedType(T0Var) && pass->hasResolvedType(T1Var)) {
        const auto& T0 = pass->getResolvedType(T0Var);
        const auto& T1 = pass->getResolvedType(T1Var);

        //                std::cout << "Score Equals: " << constraint.id() << " " << T0.raw().name() << " == " << T1.raw().name() << std::endl;

        if (proto_equal(T0, T1)) {
            // Perfect score or not.
            return 0;
        }
    }

    return std::numeric_limits<std::size_t>::max();
}
