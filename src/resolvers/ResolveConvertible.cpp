//
//  ResolveConvertible.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveConvertible.hpp"

#include "typecheck/type_manager.hpp"

#include <iostream>
#include <memory>
#include <typecheck_protos/constraint.pb.h>

typecheck::ResolveConvertible::ResolveConvertible(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) : Resolver(ConstraintKind::Conversion, pass, id) {}

std::unique_ptr<typecheck::Resolver> typecheck::ResolveConvertible::clone(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) const {
    return std::make_unique<ResolveConvertible>(pass, id);
}

bool typecheck::ResolveConvertible::doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager) {
    if (!constraint.has_types() || !constraint.types().has_first() || !constraint.types().has_second()) {
        std::cout << "Malformed ResolveConformsTo Constraint, missing conforms, protocol or type." << std::endl;
        return false;
    }

    if (this->pass && this->pass->hasResolvedType(constraint.types().first())) {
        this->options = manager->getConvertible( this->pass->getResolvedType(constraint.types().first()) );
    }

    did_find_convertible = this->options.size() > 0;
    return did_find_convertible;
}

bool typecheck::ResolveConvertible::hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) {
    // This will be called every time

    if (!this->did_find_convertible) {
        // The first time do setup
        return this->doInitialIterationSetup(constraint, manager);
    } else {
        return this->options.size() > 0;
    }
}

bool typecheck::ResolveConvertible::resolveNext(const Constraint& constraint, const TypeManager* manager) {
    if (this->options.size() > 0) {
        auto nextType = this->options.back();

        auto typeVar = constraint.types().second();
        if (this->pass->HasPermission(constraint, typeVar, manager)) {
            // Only pop if replacing
            this->options.pop_back();

            this->pass->setResolvedType(typeVar, nextType);
            return true;
        }
    }
    return false;
}

std::size_t typecheck::ResolveConvertible::score(const Constraint& constraint, const TypeManager* manager) const {
    const auto T0 = constraint.types().first();
    const auto T1 = constraint.types().second();

    if (this->pass && this->pass->hasResolvedType(T0) && this->pass->hasResolvedType(T1)) {
        if (manager->isConvertible(this->pass->getResolvedType(T0), this->pass->getResolvedType(T1))) {
            // Make sure they are resolved and convertible, otherwise, invalid.
            return 0;
        }
    }

    // This solution is not valid
    return std::numeric_limits<std::size_t>::max();
}