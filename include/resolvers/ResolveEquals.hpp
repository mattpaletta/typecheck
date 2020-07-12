#pragma once

#include "resolver.hpp"

namespace typecheck {
	class ResolveEquals : public Resolver {
	private:
		bool has_gotten_resolve = false;
	public:
		ResolveEquals(ConstraintPass* pass, const std::size_t id) : Resolver(ConstraintKind::Equal, pass, id) {}

		virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const std::size_t id) const override {
			return std::make_unique<ResolveEquals>(pass, id);
		}

		bool is_valid_constraint(const Constraint& constraint) const {
			return constraint.has_types() && constraint.types().has_first() && constraint.types().has_second();
		}
		
		virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override {
			return this->pass && !has_gotten_resolve && this->is_valid_constraint(constraint);
		}

		virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override {
			this->has_gotten_resolve = true;
			const auto T0 = constraint.types().first();
			const auto T1 = constraint.types().second();

			if (this->pass) {
				const bool hasT0 = this->pass->hasResolvedType(T0.name());
				const bool hasT1 = this->pass->hasResolvedType(T1.name());
				if (hasT0 && hasT1) {
					// We will determine if they are the same in the 'score'
					return true;

				} else if (hasT0) {
					// Don't have T1
					this->pass->setResolvedType(T1.name(), this->pass->getResolvedType(T0.name()));
					return true;

				} else if (hasT1) {
					// Don't have T0
					this->pass->setResolvedType(T0.name(), this->pass->getResolvedType(T1.name()));
					return true;

				} else {
					// Don't have either, can't resolve
					return false;
				}

			} else {
				// Don't have a pass, can't resolve.
				return false;
			}
		}

		virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override {
			if (!this->is_valid_constraint(constraint)) {
				return std::numeric_limits<std::size_t>::max();
			}

			const auto T0Var = constraint.types().first().name();
			const auto T1Var = constraint.types().second().name();

			if (this->pass && this->pass->hasResolvedType(T0Var) && this->pass->hasResolvedType(T1Var)) {
				const auto T0 = this->pass->getResolvedType(T0Var);
				const auto T1 = this->pass->getResolvedType(T1Var);

				// std::cout << "Score Equals: " << T0.name() << " == " << T1.name() << std::endl;

				if (T0.name() == T1.name()) {
					// Perfect score or not.
					return 0;
				}
			}

			return std::numeric_limits<std::size_t>::max();
		}
		
	};
}