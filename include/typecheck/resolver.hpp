#pragma once

#include "constraint_pass.hpp"
#include "constraint.hpp"

#include <memory>

namespace typecheck {
	class TypeManager;

    class Resolver {
    public:
        friend ConstraintPass;

        // These are implicitly deleted.
        Resolver(const Resolver&) = delete;
        Resolver& operator=(const Resolver&) = delete;

        Resolver(ConstraintKind _kind, const ConstraintPass::IDType _id);
		virtual ~Resolver() = default;

        virtual std::unique_ptr<Resolver> clone(const ConstraintPass::IDType _id) const;

        virtual bool hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager);
        virtual bool readyToResolve(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const;
        virtual bool resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager);
        virtual std::size_t score(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) const;

        // Once set, can't ever be changed
        const ConstraintKind kind;
        const ConstraintPass::IDType id;
	};
}
