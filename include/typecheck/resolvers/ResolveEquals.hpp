#pragma once

#include "typecheck/resolver.hpp"
#include <google/protobuf/util/message_differencer.h>

namespace typecheck {
	class ResolveEquals : public Resolver {
	private:
		bool has_gotten_resolve = false;
	public:
		ResolveEquals(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) : Resolver(ConstraintKind::Equal, pass, id) {}

		virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) const override {
			return std::make_unique<ResolveEquals>(pass, id);
		}

		bool is_valid_constraint(const Constraint& constraint) const {
			return constraint.has_types() && constraint.types().has_first() && constraint.types().has_second();
		}
		
		virtual bool hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) override {
			return this->pass && !has_gotten_resolve && this->is_valid_constraint(constraint);
		}

		virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override {
			this->has_gotten_resolve = true;
			const auto T0 = constraint.types().first();
			const auto T1 = constraint.types().second();

			if (this->pass) {
				const bool hasT0 = this->pass->hasResolvedType(T0);
				const bool hasT1 = this->pass->hasResolvedType(T1);

                const bool hasT0Permission = this->pass->HasPermission(constraint, T0, manager);
                const bool hasT1Permission = this->pass->HasPermission(constraint, T1, manager);

				if (hasT0 && hasT1) {
					// We will determine if they are the same in the 'score'
//                    std::cout << T0.symbol() << ":" << this->pass->getResolvedType(T0).raw().name() << " " << T1.symbol() << ":" << this->pass->getResolvedType(T1).raw().name() << std::endl;
                    return true;

				} else if (hasT0 && hasT1Permission) {
					// Don't have T1
					this->pass->setResolvedType(T1, this->pass->getResolvedType(T0));
//                    std::cout << T0.symbol() << ":" << this->pass->getResolvedType(T0).raw().name() << " " << T1.symbol() << ":" << this->pass->getResolvedType(T1).raw().name() << std::endl;

					return true;

				} else if (hasT1 && hasT0Permission) {
					// Don't have T0
					this->pass->setResolvedType(T0, this->pass->getResolvedType(T1));
//                    std::cout << T0.symbol() << ":" << this->pass->getResolvedType(T0).raw().name() << " " << T1.symbol() << ":" << this->pass->getResolvedType(T1).raw().name() << std::endl;

					return true;

				} else {
					// Don't have either, can't resolve
                    // Or don't have permission to set the other
					return false;
				}

			} else {
				// Don't have a pass, can't resolve.
				return false;
			}
		}

		virtual std::size_t score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const override {
			if (!this->is_valid_constraint(constraint)) {
				return std::numeric_limits<std::size_t>::max();
			}

			const auto T0Var = constraint.types().first();
			const auto T1Var = constraint.types().second();

			if (this->pass && this->pass->hasResolvedType(T0Var) && this->pass->hasResolvedType(T1Var)) {
				const auto T0 = this->pass->getResolvedType(T0Var);
				const auto T1 = this->pass->getResolvedType(T1Var);

//                std::cout << "Score Equals: " << constraint.id() << " " << T0.raw().name() << " == " << T1.raw().name() << std::endl;

                if (google::protobuf::util::MessageDifferencer::Equals(T0, T1)) {
					// Perfect score or not.
					return 0;
				}
			}

			return std::numeric_limits<std::size_t>::max();
		}
		
	};
}
