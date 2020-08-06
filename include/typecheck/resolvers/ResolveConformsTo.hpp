#pragma once

#include "typecheck/resolver.hpp"
#include "typecheck/literal_protocol.hpp"

namespace typecheck {
	class ResolveConformsTo : public Resolver {
	private:
        std::unique_ptr<LiteralProtocol> currLiteralProtocol; // = nullptr;
		bool is_preferred;
		std::vector<Type> state;
		
	public:
        ResolveConformsTo(ConstraintPass* pass, const ConstraintPass::IDType id);

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const ConstraintPass::IDType id) const override;

        bool doInitialIterationSetup(const Constraint& constraint);
        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override;
        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override;
	};
}
