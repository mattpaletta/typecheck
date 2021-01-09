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
    public:
        ResolveBindTo(const ConstraintPass::IDType id);
        virtual ~ResolveBindTo() = default;

        virtual std::unique_ptr<Resolver> clone(const ConstraintPass::IDType id) const override;

        bool is_valid_constraint(const Constraint& constraint, ConstraintPass* pass) const;
        virtual bool hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override ;
        virtual bool resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const override;

    private:
        bool has_gotten_resolve = false;
    };
}
