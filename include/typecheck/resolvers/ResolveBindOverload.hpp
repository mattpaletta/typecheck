//
//  ResolveBindTo.hpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-13.
//
#pragma once

#include "typecheck/resolver.hpp"

namespace typecheck {
    class ResolveBindOverload : public Resolver {
    public:
        ResolveBindOverload(const ConstraintPass::IDType id);

        virtual std::unique_ptr<Resolver> clone(const ConstraintPass::IDType id) const override;

        bool is_valid_constraint(const Constraint& constraint, ConstraintPass* pass) const;
        bool doInitialIterationSetup(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager);
        virtual bool hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual bool readyToResolve(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const override;
        virtual bool resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const override;

    private:
        bool did_find_overloads = false;
        bool waitingForResolve = true;
        std::vector<FunctionDefinition> overloads;
        std::size_t current_overload_i = std::numeric_limits<std::size_t>::max();
        
        bool hasPermissionIfDifferent(const TypeVar& from, const Type& to, const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const;
    };
}
