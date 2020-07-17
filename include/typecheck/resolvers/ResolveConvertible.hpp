//
//  ResolveConvertible.hpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-13.
//

#pragma once

#include "typecheck/resolver.hpp"

namespace typecheck {
    class ResolveConvertible : public Resolver {
    private:
        bool did_find_convertible = false;
        std::vector<Type> options;

    public:
        ResolveConvertible(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id);

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) const override;

        bool doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager);
        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override;
        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override;
    };
}
