#pragma once
#include <memory>
#include "constraint_pass.hpp"
#include <typecheck_protos/constraint.pb.h>

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
		const ConstraintPass::ConstraintIDType id;

		Resolver(ConstraintKind _kind, ConstraintPass* _pass, const ConstraintPass::ConstraintIDType _id) : pass(_pass), kind(_kind), id(_id) {}
		virtual ~Resolver() = default;

		virtual std::unique_ptr<Resolver> clone(ConstraintPass* _pass, const ConstraintPass::ConstraintIDType _id) const {
			return std::make_unique<Resolver>(this->kind, _pass, _id);
		}

		virtual bool hasMoreSolutions([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) {
			// Setup method
			return false;
		}

		virtual bool resolveNext([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) {
			// Called subsequent times.
			return false;
		}

		virtual std::size_t score([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const {
			return 0;
		}
	};
}
