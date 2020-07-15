#include "typecheck/constraint_pass.hpp"

#include <stdexcept>                         // for runtime_error
#include <type_traits>                       // for remove_reference<>::type
#include <utility>                           // for make_pair
#include <vector>                            // for vector
#include "typecheck/resolver.hpp"            // for Resolver
#include "typecheck_protos/constraint.pb.h"  // for Constraint, ConstraintKind
#include "typecheck/type_manager.hpp"        // for TypeManager

typecheck::ConstraintPass typecheck::ConstraintPass::CreateCopy() {
	typecheck::ConstraintPass new_pass;
    new_pass.prev = this;
    this->CopyToExisting(&new_pass);
	return new_pass;
}

void typecheck::ConstraintPass::CopyToExisting(ConstraintPass* dest) const {
	dest->resolvedTypes = this->resolvedTypes;
    dest->score = this->score;
 	dest->scores = this->scores;
}

template<class T>
T safe_add(const T& curr_val, const T& add_val) {
    if (add_val == std::numeric_limits<T>::max()) {
        return std::numeric_limits<T>::max();
    } else if (curr_val <= add_val + curr_val) {
        // Make sure we didn't overflow
        return curr_val + add_val;
    }
}

std::size_t typecheck::ConstraintPass::CalcScore(const std::deque<std::size_t>& indices, const TypeManager* manager, const bool cached) {
    if (this->resolvedTypes.size() == 0) {
        return std::numeric_limits<std::size_t>::max();
    }

	std::size_t new_score = 0;

//    if (cached) {
//        // Calculate sum of scores
//        for (auto& score : this->scores) {
//            new_score = safe_add(new_score, score.second);
//        }
//
//        if (this->scores.empty()) {
//            new_score = std::numeric_limits<std::size_t>::max();
//        }
//
//        if (new_score == std::numeric_limits<std::size_t>::max()) {
//            this->is_valid = false;
//        }
//
//        return this->score; // new_score;
//    }

    // Compute a partial score
	for (const auto i : indices) {
        const auto& constraint = manager->constraints.at(i);
        if (new_score < std::numeric_limits<std::size_t>::max()) {

            std::size_t constraint_score = std::numeric_limits<std::size_t>::max();

            // If is_old_score, use the cached versions (if applicable)
            if (!cached || this->scores.find(constraint.id()) == this->scores.end()) {
                // Compute new score
                // Here we call GetResolverRec, because we expect it to already exist.
                auto* storedResolver = this->GetResolverRec(constraint, manager);
                if (!storedResolver) {
                    // This will create one if it doesn't exist.
                    storedResolver = this->GetResolver(constraint, manager);

                    // Call optional initalization code.
                    storedResolver->hasMoreSolutions(constraint, manager);
                }

                constraint_score = storedResolver->score(constraint, manager);

                // Cache it
                this->scores[constraint.id()] = constraint_score;
            } else {
                // We have a cached version, and we can still use the 'old_score' version, so get it.
                constraint_score = this->scores.at(constraint.id());
            }

            new_score = safe_add(new_score, constraint_score);
        } else {
            // We exceeded max score
            break;
        }
	}
    this->score = new_score;
	return new_score;
}

typecheck::Type typecheck::ConstraintPass::getResolvedType(const TypeVar& var) const {
	Type type;
    type.CopyFrom(this->resolvedTypes.at(var.symbol()));
	return type;
}

bool typecheck::ConstraintPass::hasResolvedType(const TypeVar& var) const {
    return this->resolvedTypes.find(var.symbol()) != this->resolvedTypes.end();
}

void typecheck::ConstraintPass::setResolvedType(const typecheck::TypeVar& var, const typecheck::Type& type) {
    if ((type.has_raw() || type.has_func()) && !var.symbol().empty()) {
        this->resolvedTypes[var.symbol()] = type;
	}
}

bool typecheck::ConstraintPass::IsValid(const TypeManager* manager) {
	return this->score < std::numeric_limits<std::size_t>::max();
}

typecheck::Resolver* typecheck::ConstraintPass::GetResolverRec(const Constraint& constraint, const TypeManager* manager) const {
	// Recursively traverses
	if (this->resolvers.find(constraint.id()) != this->resolvers.end()) {
		auto& stored_resolver = this->resolvers.at(constraint.id());
		// We need the resolver for the parent
        return stored_resolver.get();
	}

	if (this->prev) {
		// This won't create a 'new' one, caller will have to do that.
		return this->prev->GetResolverRec(constraint, manager);
	}

	return nullptr;
}

void typecheck::ConstraintPass::ResetResolver(const typecheck::Constraint& constraint, const TypeManager* manager) {
    if (this->resolvers.find(constraint.id()) != this->resolvers.end()) {
        this->resolvers.erase(constraint.id());
    }
}

typecheck::Resolver* typecheck::ConstraintPass::GetResolver(const typecheck::Constraint& constraint, const TypeManager* manager) {
	if (this->resolvers.find(constraint.id()) != this->resolvers.end()) {
		// We already have the correct resolver in this pass
		auto& stored_resolver = this->resolvers.at(constraint.id());
		// We need the resolver for the parent
        return stored_resolver.get();
    } /*else if (this->prev) {
        // Ask the previous
        return this->prev->GetResolver(constraint, manager);
    } */ else {
        // Did not find, don't have a previous

        if (manager->registeredResolvers.find(constraint.kind()) == manager->registeredResolvers.end()) {
            // It's not in the manager either
            throw std::runtime_error("Constraint resolver not registered: " + std::to_string(constraint.kind()));
        }
        // Make a `copy` of the resolver
        // taken from: https://www.fluentcpp.com/2017/09/08/make-polymorphic-copy-modern-cpp/
        this->resolvers.emplace(std::make_pair(constraint.id(), std::move(manager->registeredResolvers.at(constraint.kind())->clone(this, constraint.id()))));
        return this->resolvers.at(constraint.id()).get();
    }
}
