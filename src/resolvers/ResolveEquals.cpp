//
//  ResolveEquals.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveEquals.hpp"

typecheck::ResolveEquals::ResolveEquals(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) : Resolver(ConstraintKind::Equal, pass, id) {}

std::unique_ptr<typecheck::Resolver> typecheck::ResolveEquals::clone(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) const {
    return std::make_unique<ResolveEquals>(pass, id);
}

bool typecheck::ResolveEquals::is_valid_constraint(const Constraint& constraint) const {
    return constraint.has_types() && constraint.types().has_first() && constraint.types().has_second();
}

bool typecheck::ResolveEquals::hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) {
    return this->pass && !has_gotten_resolve && this->is_valid_constraint(constraint);
}

bool typecheck::ResolveEquals::resolveNext(const Constraint& constraint, const TypeManager* manager) {
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
            //                    std::cout << T0.symbol() << ":" << this->pass->getResolvedType(T0).raw().name() << " " << T1.symbol() << ":" << this->pass->getResolvedType(T1).raw().name() << std::endl;
            return true;

        } else if (hasT0 && hasT1Permission) {
            // Don't have T1
            this->pass->setResolvedType(T1, this->pass->getResolvedType(T0));
            //                    std::cout << T0.symbol() << ":" << this->pass->getResolvedType(T0).raw().name() << " " << T1.symbol() << ":" << this->pass->getResolvedType(T1).raw().name() << std::endl;

            return true;

        } else if (hasT1 && hasT0Permission) {
            // Don't have T0
            this->pass->setResolvedType(T0, this->pass->getResolvedType(T1));
            //                    std::cout << T0.symbol() << ":" << this->pass->getResolvedType(T0).raw().name() << " " << T1.symbol() << ":" << this->pass->getResolvedType(T1).raw().name() << std::endl;

            return true;

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

std::size_t typecheck::ResolveEquals::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const {
    if (!this->is_valid_constraint(constraint)) {
        return std::numeric_limits<std::size_t>::max();
    }

    const auto T0Var = constraint.types().first();
    const auto T1Var = constraint.types().second();

    if (this->pass && this->pass->hasResolvedType(T0Var) && this->pass->hasResolvedType(T1Var)) {
        const auto T0 = this->pass->getResolvedType(T0Var);
        const auto T1 = this->pass->getResolvedType(T1Var);

        //                std::cout << "Score Equals: " << constraint.id() << " " << T0.raw().name() << " == " << T1.raw().name() << std::endl;

        if (google::protobuf::util::MessageDifferencer::Equals(T0, T1)) {
            // Perfect score or not.
            return 0;
        }
    }

    return std::numeric_limits<std::size_t>::max();
}
