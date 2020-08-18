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

using namespace typecheck;

ResolveBindOverload::ResolveBindOverload(ConstraintPass* _pass, const ConstraintPass::IDType _id) : Resolver(ConstraintKind::BindOverload, _pass, _id) {}

auto ResolveBindOverload::clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const -> std::unique_ptr<Resolver> {
    return std::make_unique<ResolveBindOverload>(_pass, _id);
}

auto ResolveBindOverload::is_valid_constraint(const Constraint& constraint) const -> bool {
    return constraint.has_overload() && constraint.overload().has_type();
}

auto ResolveBindOverload::doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager) -> bool {
    if (this->pass && this->is_valid_constraint(constraint) && manager->canGetFunctionOverloads(constraint.overload().functionid(), this->pass)) {
        // Try and get registered overloads
        this->overloads = manager->getFunctionOverloads(constraint.overload().functionid(), this->pass);
        // Reset index to 0.
        this->current_overload_i = 0;
        this->did_find_overloads = true;
        return this->overloads.size() > 0;
    }

    return false;
}

auto ResolveBindOverload::hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) -> bool {
    if (!this->did_find_overloads) {
        // The first time do setup
        this->waitingForResolve = true;
        return this->doInitialIterationSetup(constraint, manager);
    } else if (!this->waitingForResolve) {
        this->current_overload_i++;
        this->waitingForResolve = true;
    }
    return this->is_valid_constraint(constraint) && this->overloads.size() > 0 && this->current_overload_i < this->overloads.size();
}

auto ResolveBindOverload::readyToResolve(const Constraint& constraint, const TypeManager* manager) const -> bool {
    return this->did_find_overloads && manager->canGetFunctionOverloads(constraint.overload().functionid(), this->pass);
}

auto ResolveBindOverload::hasPermissionIfDifferent(const TypeVar& from, const Type& to, const Constraint& constraint, const TypeManager* manager) const -> bool {
    if (!this->pass->hasResolvedType(from)) {
        return true;
    }
    const auto isDifferent = !google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(from), to);

    // Either they're the same, or we have permission, and they're different
    return !isDifferent || (isDifferent && this->pass->HasPermission(constraint, from, manager));
}

auto ResolveBindOverload::resolveNext(const Constraint& constraint, const TypeManager* manager) -> bool {
    this->waitingForResolve = false;

    if (this->did_find_overloads) {
        FunctionDefinition nextOverload = this->overloads.at(this->current_overload_i);

        while (nextOverload.args_size() != constraint.overload().argvars_size() && this->current_overload_i < this->overloads.size()) {
            nextOverload = this->overloads.at(this->current_overload_i++);
            // Skip over any that don't have the same number of arguments.
        }
        
        // Only proceed if we found an overload with the same number of arguments
        if (nextOverload.args_size() == constraint.overload().argvars_size()) {
            const auto typeVar = constraint.overload().type();
            Type typeVarTy;
            typeVarTy.mutable_func()->CopyFrom(nextOverload);
            if (!this->hasPermissionIfDifferent(typeVar, typeVarTy, constraint, manager)) {
                return false;
            }

            // try and fill in vars with overload type
            for (int i = 0; i < constraint.overload().argvars_size(); ++i) {
                const auto arg = constraint.overload().argvars(i);

                // We need to change it, but can't.
                if (!this->hasPermissionIfDifferent(arg, nextOverload.args(i), constraint, manager)) {
                    return false;
                }
            }

            if (!this->hasPermissionIfDifferent(constraint.overload().returnvar(), nextOverload.returntype(), constraint, manager)) {
                return false;
            }

            // Repeat steps, actually applying work, this is because we don't want partial application

            // These will fail if we don't have permission, but that's already accounted for above
            this->pass->setResolvedType(constraint, typeVar, typeVarTy, manager);

            for (int i = 0; i < constraint.overload().argvars_size(); ++i) {
                const auto arg = constraint.overload().argvars(i);
                this->pass->setResolvedType(constraint, arg, nextOverload.args(i), manager);
            }

            this->pass->setResolvedType(constraint, constraint.overload().returnvar(), nextOverload.returntype(), manager);

            return true;
        }
    }

    return false;
}

auto ResolveBindOverload::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    if (!this->is_valid_constraint(constraint)) {
        return std::numeric_limits<std::size_t>::max();
    }

    if (this->pass && this->pass->hasResolvedType(constraint.overload().type()) && this->current_overload_i < this->overloads.size()) {
        const auto currentOverload = this->overloads.at(this->current_overload_i);

        for (int i = 0; i < constraint.overload().argvars_size(); ++i) {
            const auto arg = constraint.overload().argvars(i);
            if (this->pass->hasResolvedType(arg)) {
                // Make sure the arg types match up
                if (!google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(arg), currentOverload.args(i))) {
                    return std::numeric_limits<std::size_t>::max();
                }
            }
        }

        if (this->pass->hasResolvedType(constraint.overload().returnvar())) {
            // Make sure the return types match up
            if (!google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(constraint.overload().returnvar()), currentOverload.returntype())) {
                return std::numeric_limits<std::size_t>::max();
            }
        }

        // All args found, and matched up, and return types found and match up.
        return 0;
    }

    return std::numeric_limits<std::size_t>::max();
}
