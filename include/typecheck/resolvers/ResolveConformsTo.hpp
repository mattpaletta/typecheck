#pragma once

#include "typecheck/resolver.hpp"
#include "typecheck/literal_protocol.hpp"

namespace typecheck {
	class ResolveConformsTo : public Resolver {
	public:
        ResolveConformsTo(const ConstraintPass::IDType id);
        virtual ~ResolveConformsTo() = default;

        virtual std::unique_ptr<Resolver> clone(const ConstraintPass::IDType id) const override;

        bool doInitialIterationSetup(const Constraint& constraint, ConstraintPass* pass);
        virtual bool hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual bool resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) override;
        virtual std::size_t score(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const override;

    private:
        std::unique_ptr<LiteralProtocol> currLiteralProtocol; // = nullptr;
        bool is_preferred;
        std::vector<Type> state;
    };
}
