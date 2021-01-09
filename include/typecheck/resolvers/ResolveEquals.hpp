#pragma once

#include "typecheck/resolver.hpp"

namespace typecheck {
	class ResolveEquals : public Resolver {
	public:
        ResolveEquals(const ConstraintPass::IDType id);
        virtual ~ResolveEquals() = default;

        virtual std::unique_ptr<Resolver> clone(const ConstraintPass::IDType id) const override;

        bool is_valid_constraint(const Constraint& constraint, ConstraintPass* pass) const;
        virtual bool hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual bool readyToResolve(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const override;
        virtual bool resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const override;

    private:
        bool has_gotten_resolve = false;
	};
}
