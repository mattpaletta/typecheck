#pragma once

#include "typecheck/resolver.hpp"

namespace typecheck {
	class ResolveEquals : public Resolver {
	private:
		bool has_gotten_resolve = false;
	public:
        ResolveEquals(ConstraintPass* pass, const ConstraintPass::IDType id);

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const ConstraintPass::IDType id) const override;

        bool is_valid_constraint(const Constraint& constraint) const;
        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override;
        virtual bool readyToResolve(const Constraint& constraint, const TypeManager* manager) const override;
        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override;
	};
}
