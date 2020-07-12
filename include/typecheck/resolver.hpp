#pragma once
#include <memory>
#include <typecheck_protos/constraint.pb.h>

namespace typecheck {
	class ConstraintPass;
	class TypeManager;
	class Resolver {
	protected:
		// WeakPtr.
		ConstraintPass* pass;
	public:
		// Once set, can't ever be changed
		const ConstraintKind kind;
		const std::size_t id;

		Resolver(ConstraintKind _kind, ConstraintPass* _pass, const std::size_t _id) : kind(_kind), pass(_pass), id(_id) {}
		virtual ~Resolver() = default;
		Resolver(const Resolver&) = delete;
		Resolver& operator=(const Resolver&) = delete;

		virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const std::size_t id) const {
			return std::make_unique<Resolver>(this->kind, pass, id);
		}

		virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) {
			// Setup method
			return false;
		}

		virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) {
			// Called subsequent times.
			return false;
		}

		virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const {
			return 0;
		}
	};
}
