#pragma once
#include <memory>
#include "constraint_pass.hpp"
#include "constraint.hpp"

namespace typecheck {
//	class ConstraintPass;
	class TypeManager;
	class Resolver {
    public:
        friend ConstraintPass;

        // These are implicitly deleted.
        Resolver(const Resolver&) = delete;
        Resolver& operator=(const Resolver&) = delete;
	protected:
		// WeakPtr.
		ConstraintPass* pass;

	public:
		// Once set, can't ever be changed
		const ConstraintKind kind;
		const ConstraintPass::IDType id;

        Resolver(ConstraintKind _kind, ConstraintPass* _pass, const ConstraintPass::IDType _id);
		virtual ~Resolver() = default;

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const;

        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager);
        virtual bool readyToResolve(const Constraint& constraint, const TypeManager* manager) const;
        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager);
        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const;
	};
}
