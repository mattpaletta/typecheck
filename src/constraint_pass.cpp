#include "typecheck/constraint_pass.hpp"

#include <stdexcept>                         // for runtime_error
#include <type_traits>                       // for remove_reference<>::type
#include <utility>                           // for make_pair
#include <vector>                            // for vector
#include "typecheck/resolver.hpp"            // for Resolver
#include "typecheck_protos/constraint.pb.h"  // for Constraint, ConstraintKind
#include "typecheck/type_manager.hpp"        // for TypeManager

typecheck::ConstraintPass typecheck::ConstraintPass::CreateCopy() const {
	typecheck::ConstraintPass new_pass;
	new_pass.resolvedTypes = this->resolvedTypes;
	new_pass.prev = this;
	new_pass.score = score;
	// Don't copy resolvers, reference old resolvers through ptr (so they reset their state)
	return new_pass;
}

void typecheck::ConstraintPass::CopyToExisting(ConstraintPass* dest) const {
	dest->resolvedTypes = this->resolvedTypes;
	dest->score = this->score;
	dest->is_valid = this->is_valid;
}

std::size_t typecheck::ConstraintPass::CalcScore(const TypeManager* manager) {
	std::size_t new_score = 0;
	this->is_valid = true;
	for (auto& constraint : manager->constraints) {
		if (new_score < std::numeric_limits<std::size_t>::max()) {
			// Here we call GetResolverRec, because we expect it to already exist.
			auto* storedResolver = this->GetResolverRec(constraint, manager);
			if (!storedResolver) {
				// This will create one if it doesn't exist.
				storedResolver = this->GetResolver(constraint, manager);
			}
			const auto constraint_score = storedResolver->score(constraint, manager);
			if (constraint_score == std::numeric_limits<std::size_t>::max()) {
				new_score = std::numeric_limits<std::size_t>::max();
				this->is_valid = false;
				break;
			} else if (constraint_score + new_score >= new_score) {
				// Make sure we didn't overflow
				new_score += constraint_score;
			}
		} else {
			this->is_valid = false;
			break;
		}
	}
	this->score = new_score;
	return this->score;
}

typecheck::Type typecheck::ConstraintPass::getResolvedType(const std::string& var) const {
	Type type;
	type.set_name(this->resolvedTypes.at(var));
	return type;
}

bool typecheck::ConstraintPass::hasResolvedType(const std::string& var) const {
	return this->resolvedTypes.find(var) != this->resolvedTypes.end();
}

void typecheck::ConstraintPass::setResolvedType(const std::string& var, const typecheck::Type& type) {
	if (!type.name().empty() && !var.empty()) {
		this->resolvedTypes[var] = type.name();
	}
}

bool typecheck::ConstraintPass::IsValid(const TypeManager* manager) {
	return this->is_valid;
}

typecheck::Resolver* typecheck::ConstraintPass::GetResolverRec(const Constraint& constraint, const TypeManager* manager) const {
	// Recursively traverses
	if (this->resolvers.find(constraint.kind()) != this->resolvers.end()) {
		auto& stored_resolver = this->resolvers.at(constraint.kind());
		// We need the resolver for the parent
		if (stored_resolver->id == constraint.id()) {
			return stored_resolver.get();
		}
	}

	if (this->prev) {
		// This won't create a 'new' one, caller will have to do that.
		return this->prev->GetResolverRec(constraint, manager);
	}

	return nullptr;
}

typecheck::Resolver* typecheck::ConstraintPass::GetResolver(const typecheck::Constraint& constraint, const TypeManager* manager) {
	if (this->resolvers.find(constraint.kind()) != this->resolvers.end()) {
		// We already have the correct resolver in this pass
		auto& stored_resolver = this->resolvers.at(constraint.kind());
		// We need the resolver for the parent
		if (stored_resolver->id == constraint.id()) {
			return stored_resolver.get();
		}
	}

	if (manager->registeredResolvers.find(constraint.kind()) == manager->registeredResolvers.end()) {
		// It's not in the manager either
		throw std::runtime_error("Constraint resolver not registered: " + std::to_string(constraint.kind()));
	}
	// Make a `copy` of the resolver
	// taken from: https://www.fluentcpp.com/2017/09/08/make-polymorphic-copy-modern-cpp/
	this->resolvers.emplace(std::make_pair(constraint.kind(), std::move(manager->registeredResolvers.at(constraint.kind())->clone(this, constraint.id()))));
	return this->resolvers.at(constraint.kind()).get();
}
