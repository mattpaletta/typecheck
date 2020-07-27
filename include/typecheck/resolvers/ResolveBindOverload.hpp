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
    private:
        bool did_find_overloads = false;
        std::vector<FunctionDefinition> overloads;
        std::size_t current_overload_i = std::numeric_limits<std::size_t>::max();
    public:
        ResolveBindOverload(ConstraintPass* pass, const ConstraintPass::IDType id);

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const ConstraintPass::IDType id) const override;

        bool is_valid_constraint(const Constraint& constraint) const;
        bool doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager);
        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override;
        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override;
    };
}
