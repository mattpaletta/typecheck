//
//  ResolveEquals.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveEquals.hpp"
#include "typecheck/debug.hpp"

using namespace typecheck;

ResolveEquals::ResolveEquals(ConstraintPass* _pass, const ConstraintPass::IDType _id) : Resolver(ConstraintKind::Equal, _pass, _id) {}

auto ResolveEquals::clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const -> std::unique_ptr<Resolver> {
    return std::make_unique<ResolveEquals>(_pass, _id);
}

auto ResolveEquals::is_valid_constraint(const Constraint& constraint) const -> bool {
    return constraint.has_types() && constraint.types().has_first() && constraint.types().has_second();
}

auto ResolveEquals::hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    return this->pass && !this->has_gotten_resolve && this->is_valid_constraint(constraint);
}

auto ResolveEquals::readyToResolve([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> bool {
    if (!this->pass) {
        return false;
    }

    const auto T0 = constraint.types().first();
    const auto T1 = constraint.types().second();

    const bool hasT0 = this->pass->hasResolvedType(T0);
    const bool hasT1 = this->pass->hasResolvedType(T1);

    return hasT0 || hasT1;
}

auto ResolveEquals::resolveNext(const Constraint& constraint, const TypeManager* manager) -> bool {
    this->has_gotten_resolve = true;
    const auto T0 = constraint.types().first();
    const auto T1 = constraint.types().second();

    if (this->pass) {
        const bool hasT0 = this->pass->hasResolvedType(T0);
        const bool hasT1 = this->pass->hasResolvedType(T1);

        const bool hasT0Permission = this->pass->HasPermission(constraint, T0, manager);
        const bool hasT1Permission = this->pass->HasPermission(constraint, T1, manager);

        if (hasT0 && hasT1) {
            // We will determine if they are the same in the 'score'
            return true;

        } else if (hasT0 && hasT1Permission) {
            // Don't have T1
            return this->pass->setResolvedType(constraint, T1, this->pass->getResolvedType(T0), manager);

        } else if (hasT1 && hasT0Permission) {
            // Don't have T0
            return this->pass->setResolvedType(constraint, T0, this->pass->getResolvedType(T1), manager);

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

auto ResolveEquals::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    if (!this->is_valid_constraint(constraint)) {
        return std::numeric_limits<std::size_t>::max();
    }

    const auto T0Var = constraint.types().first();
    const auto T1Var = constraint.types().second();

    if (this->pass && this->pass->hasResolvedType(T0Var) && this->pass->hasResolvedType(T1Var)) {
        const auto T0 = this->pass->getResolvedType(T0Var);
        const auto T1 = this->pass->getResolvedType(T1Var);

        //                std::cout << "Score Equals: " << constraint.id() << " " << T0.raw().name() << " == " << T1.raw().name() << std::endl;

        if (T0 == T1) {
            // Perfect score or not.
            return 0;
        }
    }

    return std::numeric_limits<std::size_t>::max();
}
