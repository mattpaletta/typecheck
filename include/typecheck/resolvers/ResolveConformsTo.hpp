#pragma once

#include "typecheck/resolver.hpp"
#include "typecheck/literal_protocol.hpp"

#include "typecheck/protocols/ExpressibleByDoubleLiteral.hpp"
#include "typecheck/protocols/ExpressibleByFloatLiteral.hpp"
#include "typecheck/protocols/ExpressibleByIntegerLiteral.hpp"

#include <iostream>
#include <memory>
#include <typecheck_protos/constraint.pb.h>

namespace typecheck {
	class ResolveConformsTo : public Resolver {
	private:
		std::unique_ptr<LiteralProtocol> currLiteralProtocol = nullptr;
		bool is_preferred;
		std::vector<Type> state;
		
	public:
		ResolveConformsTo(ConstraintPass* pass, const std::size_t id) : Resolver(ConstraintKind::ConformsTo, pass, id) {}

		virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const std::size_t id) const override {
			return std::make_unique<ResolveConformsTo>(pass, id);
		}

		bool doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager) {
			if (!constraint.has_conforms() || !constraint.conforms().has_protocol() || !constraint.conforms().has_type()) {
				std::cout << "Malformed ResolveConformsTo Constraint, missing conforms, protocol or type." << std::endl;
				return false;
			}

			// Reset internal state
			this->is_preferred = true;

			const auto T1 = constraint.conforms().type();
			bool did_find_protocol = false;
			switch (constraint.conforms().protocol().literal()) {
			case KnownProtocolKind_LiteralProtocol_ExpressibleByFloat:
				this->currLiteralProtocol = std::make_unique<ExpressibleByFloatLiteral>();
				did_find_protocol = true;
				break;
			case KnownProtocolKind_LiteralProtocol_ExpressibleByInteger:
				this->currLiteralProtocol = std::make_unique<ExpressibleByIntegerLiteral>();
				did_find_protocol = true;
				break;
			default:
				throw std::runtime_error("Failed to find protocol");
			}
			
			if (did_find_protocol) {
				this->state = this->currLiteralProtocol->getPreferredTypes();
			}

			return did_find_protocol;
		}

		virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override {
			// This will be called every time
			
			if (!this->currLiteralProtocol) {
				// The first time do setup
				return this->doInitialIterationSetup(constraint, manager);
			} else {
				if (this->is_preferred) {
					// Go through preferred first
					if (this->state.size() == 0) {
						// Try and switch over to the other
						this->state = this->currLiteralProtocol->getOtherTypes();
						this->is_preferred = false;
					} else {
						return true;
					}
				}

				// We're already in the 'other' types
				// We have items as long as we do, and then that's it.
				return this->state.size() > 0;
			}
		}

		virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override {
			if (this->currLiteralProtocol) {
				// TODO: Switch to list, so we can pop_front
				auto nextType = this->state.back();
				this->state.pop_back();

				auto typeVar = constraint.conforms().type();
				this->pass->setResolvedType(typeVar, nextType);
				return true;
			}
			return false;
		}

		virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override {
			const auto typeVar = constraint.conforms().type();

			if (this->pass && this->currLiteralProtocol && this->pass->hasResolvedType(typeVar)) {
				const auto resolvedType = this->pass->getResolvedType(typeVar);
				// Is it a preferred type or other type?
				for (auto& pref : this->currLiteralProtocol->getPreferredTypes()) {
					// std::cout << "Conforms To: " << pref.name() << " == " << resolvedType.name() << std::endl;
					if (pref.name() == resolvedType.name()) {
						// It's preferred! Perfect score
						return 0;
					}
				}

				for (auto& other : this->currLiteralProtocol->getOtherTypes()) {
					// std::cout << "Conforms To: " << other.name() << " == " << resolvedType.name() << std::endl;
					if (other.name() == resolvedType.name()) {
						// It's other! Resolved, but not perfect score
						return 1;
					}
				}
			}

			// This solution is not valid
			return std::numeric_limits<std::size_t>::max();
		}
	};
}
