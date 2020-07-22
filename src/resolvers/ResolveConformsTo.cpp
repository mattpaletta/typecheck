//
//  ResolveConformsTo.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveConformsTo.hpp"
#include "typecheck/debug.hpp"

#include "typecheck/protocols/ExpressibleByDoubleLiteral.hpp"
#include "typecheck/protocols/ExpressibleByFloatLiteral.hpp"
#include "typecheck/protocols/ExpressibleByIntegerLiteral.hpp"

#include <google/protobuf/util/message_differencer.h>

#include <iostream>
#include <memory>
#include <typecheck_protos/constraint.pb.h>

typecheck::ResolveConformsTo::ResolveConformsTo(ConstraintPass* _pass, const ConstraintPass::IDType _id) : Resolver(ConstraintKind::ConformsTo, _pass, _id), is_preferred(true) {}

auto typecheck::ResolveConformsTo::clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const -> std::unique_ptr<typecheck::Resolver> {
    return std::make_unique<ResolveConformsTo>(_pass, _id);
}

auto typecheck::ResolveConformsTo::doInitialIterationSetup(const Constraint& constraint) -> bool {
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
        case KnownProtocolKind_LiteralProtocol_ExpressibleByArray:
        case KnownProtocolKind_LiteralProtocol_ExpressibleByBoolean:
        case KnownProtocolKind_LiteralProtocol_ExpressibleByDictionary:
        case KnownProtocolKind_LiteralProtocol_ExpressibleByString:
        case KnownProtocolKind_LiteralProtocol_ExpressibleByNil:
        case KnownProtocolKind_LiteralProtocol_KnownProtocolKind_LiteralProtocol_INT_MIN_SENTINEL_DO_NOT_USE_:
        case KnownProtocolKind_LiteralProtocol_KnownProtocolKind_LiteralProtocol_INT_MAX_SENTINEL_DO_NOT_USE_:
            throw std::runtime_error("Failed to find protocol");
    }

    if (did_find_protocol) {
        this->state = this->currLiteralProtocol->getPreferredTypes();
    }

    return did_find_protocol;
}

auto typecheck::ResolveConformsTo::hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    // This will be called every time

    if (!this->currLiteralProtocol) {
        // The first time do setup
        return this->doInitialIterationSetup(constraint);
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

auto typecheck::ResolveConformsTo::resolveNext(const Constraint& constraint, const TypeManager* manager) -> bool {
    if (this->currLiteralProtocol) {
        // TODO: Switch to list, so we can pop_front
        auto nextType = this->state.back();
        this->state.pop_back();

        auto typeVar = constraint.conforms().type();
        if (this->pass->RequestPermission(constraint, typeVar, manager)) {
            this->pass->setResolvedType(typeVar, nextType);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

auto typecheck::ResolveConformsTo::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    const auto typeVar = constraint.conforms().type();
    TYPECHECK_ASSERT(this->currLiteralProtocol.operator bool(), "Must set protocol before calling score, call this->hasMoreSolutions(...) first");
    TYPECHECK_ASSERT(this->pass, "Must set pass object before calling score.");

    if (this->pass && this->currLiteralProtocol && this->pass->hasResolvedType(typeVar)) {
        const auto resolvedType = this->pass->getResolvedType(typeVar);
        // Is it a preferred type or other type?
        for (auto& pref : this->currLiteralProtocol->getPreferredTypes()) {
            if (google::protobuf::util::MessageDifferencer::Equals(pref, resolvedType)) {
                // It's preferred! Perfect score
                return 0;
            }
        }

        for (auto& other : this->currLiteralProtocol->getOtherTypes()) {
            if (google::protobuf::util::MessageDifferencer::Equals(other, resolvedType)) {
                // It's other! Resolved, but not perfect score
                return 1;
            }
        }
    }

    // This solution is not valid
    return std::numeric_limits<std::size_t>::max();
}
