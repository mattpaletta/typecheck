//
//  ResolveConformsTo.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include <typecheck/resolvers/ResolveConformsTo.hpp>
#include <typecheck/debug.hpp>
#include <typecheck/constraint.hpp>

#include <typecheck/protocols/ExpressibleByDoubleLiteral.hpp>
#include <typecheck/protocols/ExpressibleByFloatLiteral.hpp>
#include <typecheck/protocols/ExpressibleByIntegerLiteral.hpp>

#include <iostream>
#include <memory>

using namespace typecheck;

ResolveConformsTo::ResolveConformsTo(const ConstraintPass::IDType _id) : Resolver(ConstraintKind::ConformsTo, _id), is_preferred(true) {}

auto ResolveConformsTo::clone(const ConstraintPass::IDType _id) const -> std::unique_ptr<Resolver> {
    return std::make_unique<ResolveConformsTo>(_id);
}

auto ResolveConformsTo::doInitialIterationSetup(const Constraint& constraint, [[maybe_unused]] ConstraintPass* pass) -> bool {
    if (!constraint.has_conforms() || !constraint.conforms().has_protocol() || !constraint.conforms().has_type()) {
        std::cout << "Malformed ResolveConformsTo Constraint, missing conforms, protocol or type." << std::endl;
        return false;
    }

    // Reset internal state
    this->is_preferred = true;

    const auto T1 = constraint.conforms().type();
    bool did_find_protocol = false;
    switch (constraint.conforms().protocol().literal()) {
		case KnownProtocolKind::LiteralProtocol::ExpressibleByFloat:
            this->currLiteralProtocol = std::make_unique<ExpressibleByFloatLiteral>();
            did_find_protocol = true;
            break;
		case KnownProtocolKind::LiteralProtocol::ExpressibleByInteger:
            this->currLiteralProtocol = std::make_unique<ExpressibleByIntegerLiteral>();
            did_find_protocol = true;
            break;
		case KnownProtocolKind::LiteralProtocol::ExpressibleByArray:
		case KnownProtocolKind::LiteralProtocol::ExpressibleByBoolean:
		case KnownProtocolKind::LiteralProtocol::ExpressibleByDictionary:
		case KnownProtocolKind::LiteralProtocol::ExpressibleByString:
		case KnownProtocolKind::LiteralProtocol::ExpressibleByNil:
            throw std::runtime_error("Failed to find protocol");
    }

    if (did_find_protocol) {
        this->state = this->currLiteralProtocol->getPreferredTypes();
    }

    return did_find_protocol;
}

auto ResolveConformsTo::hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) -> bool {
    // This will be called every time

    if (!this->currLiteralProtocol) {
        // The first time do setup
        return this->doInitialIterationSetup(constraint, pass);
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

auto ResolveConformsTo::resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) -> bool {
    if (this->currLiteralProtocol) {
        const auto typeVar = constraint.conforms().type();

        // TODO: Switch to list, so we can pop_front
        const auto nextType = this->state.back();
        this->state.pop_back();

        return pass->setResolvedType(constraint, typeVar, nextType, manager);
    }
    return false;
}

auto ResolveConformsTo::score(const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    const auto& typeVar = constraint.conforms().type();
    TYPECHECK_ASSERT(this->currLiteralProtocol.operator bool(), "Must set protocol before calling score, call this->hasMoreSolutions(...) first");
    TYPECHECK_ASSERT(pass, "Must set pass object before calling score.");

    if (pass && this->currLiteralProtocol && pass->hasResolvedType(typeVar)) {
        const auto& resolvedType = pass->getResolvedType(typeVar);
        // Is it a preferred type or other type?
        for (auto& pref : this->currLiteralProtocol->getPreferredTypes()) {
            if (proto_equal(pref, resolvedType)) {
                // It's preferred! Perfect score
                return 0;
            }
        }

        for (auto& other : this->currLiteralProtocol->getOtherTypes()) {
            if (proto_equal(other, resolvedType)) {
                // It's other! Resolved, but not perfect score
                return 1;
            }
        }
    }

    // This solution is not valid
    return std::numeric_limits<std::size_t>::max();
}
