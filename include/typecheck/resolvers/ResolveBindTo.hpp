//
//  ResolveBindTo.hpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-15.
//

#pragma once

#include "typecheck/resolver.hpp"


namespace typecheck {
    class ResolveBindTo : public Resolver {
    private:
        bool has_gotten_resolve = false;
    public:
        ResolveBindTo(ConstraintPass* pass, const ConstraintPass::IDType id);

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const ConstraintPass::IDType id) const override;

        bool is_valid_constraint(const Constraint& constraint) const;
        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override ;
        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override;

    };
}
