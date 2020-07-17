//
//  ResolveBindOverload.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveBindOverload.hpp"
#include "typecheck/type_manager.hpp"
#include "typecheck/debug.hpp"

#include <google/protobuf/util/message_differencer.h>

typecheck::ResolveBindOverload::ResolveBindOverload(ConstraintPass* pass, const ConstraintPass::IDType _id) : Resolver(ConstraintKind::BindOverload, pass, _id) {}

std::unique_ptr<typecheck::Resolver> typecheck::ResolveBindOverload::clone(ConstraintPass* pass, const ConstraintPass::IDType _id) const {
    return std::make_unique<ResolveBindOverload>(pass, _id);
}

bool typecheck::ResolveBindOverload::is_valid_constraint(const Constraint& constraint) const {
    return constraint.has_overload() && constraint.overload().has_type();
}

void typecheck::ResolveBindOverload::doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager) {
    if (this->pass && this->is_valid_constraint(constraint)) {
        // Try and get registered overloads
        this->overloads = manager->getFunctionOverloads(constraint.overload().type());
        // Reset index to 0.
        this->current_overload_i = 0;
        this->did_find_overloads = true;
    }
}

bool typecheck::ResolveBindOverload::hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) {
    if (!this->did_find_overloads) {
        // The first time do setup
        this->doInitialIterationSetup(constraint, manager);
    } else {
        this->current_overload_i++;
    }

    return this->is_valid_constraint(constraint) && this->overloads.size() > 0 && this->current_overload_i < this->overloads.size();
}

bool typecheck::ResolveBindOverload::resolveNext(const Constraint& constraint, const TypeManager* manager) {
    if (this->did_find_overloads) {
        typecheck::Type nextOverload = this->overloads.at(this->current_overload_i);

        while (nextOverload.has_func() && nextOverload.func().args_size() != constraint.overload().argvars_size() && this->current_overload_i < this->overloads.size()) {
            nextOverload = this->overloads.at(this->current_overload_i++);
            // Skip over any that don't have the same number of arguments.
        }

        if (nextOverload.has_func() && nextOverload.func().args_size() == constraint.overload().argvars_size()) {
            // Only proceed if we found an overload with the same number of arguments

            const auto typeVar = constraint.overload().type();
            if (this->pass->HasPermission(constraint, typeVar, manager)) {
                // Only proceed if we have permission to do so.
                this->pass->setResolvedType(typeVar, nextOverload);
            } else {
                return false;
            }

            // try and fill in vars with overload type
            for (int i = 0; i < constraint.overload().argvars_size(); ++i) {
                const auto arg = constraint.overload().argvars(i);
                if (this->pass->HasPermission(constraint, arg, manager)) {
                    // Don't override already resolved types, they will fail in score
                    this->pass->setResolvedType(arg, nextOverload.func().args(i));
                } else {
                    return false;
                }
            }

            if (this->pass->HasPermission(constraint, constraint.overload().returnvar(), manager)) {
                this->pass->setResolvedType(constraint.overload().returnvar(), nextOverload.func().returntype());
            } else {
                return false;
            }

            return true;
        }
    }

    return false;
}

std::size_t typecheck::ResolveBindOverload::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const {
    if (!this->is_valid_constraint(constraint)) {
        return std::numeric_limits<std::size_t>::max();
    }

    if (this->pass && this->pass->hasResolvedType(constraint.overload().type()) && this->current_overload_i < this->overloads.size()) {
        const auto currentOverload = this->overloads.at(this->current_overload_i);

        for (int i = 0; i < constraint.overload().argvars_size(); ++i) {
            const auto arg = constraint.overload().argvars(i);
            if (this->pass->hasResolvedType(arg)) {
                // Make sure the arg types match up
                if (!google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(arg), currentOverload.func().args(i))) {
                    return std::numeric_limits<std::size_t>::max();
                }
            }
        }

        if (this->pass->hasResolvedType(constraint.overload().returnvar())) {
            // Make sure the return types match up
            if (!google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(constraint.overload().returnvar()), currentOverload.func().returntype())) {
                return std::numeric_limits<std::size_t>::max();
            }
        }

        // All args found, and matched up, and return types found and match up.
        return 0;
    }

    return std::numeric_limits<std::size_t>::max();
}
