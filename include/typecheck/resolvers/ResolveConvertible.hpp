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
    public:
        ResolveConvertible(const ConstraintPass::IDType id);
        virtual ~ResolveConvertible() = default;

        virtual std::unique_ptr<Resolver> clone(const ConstraintPass::IDType id) const override;

        bool doInitialIterationSetup(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager);
        virtual bool hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual bool resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const override;

    private:
        bool did_find_convertible = false;
        std::vector<Type> options;
    };
}
