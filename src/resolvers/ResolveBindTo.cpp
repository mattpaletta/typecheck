//
//  ResolveBindTo.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveBindTo.hpp"
#include "typecheck/debug.hpp"

#include <google/protobuf/util/message_differencer.h>

typecheck::ResolveBindTo::ResolveBindTo(ConstraintPass* _pass, const ConstraintPass::IDType _id) : Resolver(ConstraintKind::Bind, _pass, _id) {}

auto typecheck::ResolveBindTo::clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const -> std::unique_ptr<typecheck::Resolver> {
    return std::make_unique<ResolveBindTo>(_pass, _id);
}

auto typecheck::ResolveBindTo::is_valid_constraint(const Constraint& constraint) const -> bool {
    return constraint.has_explicit_() && constraint.explicit_().has_type() && constraint.explicit_().has_var();
}

auto typecheck::ResolveBindTo::hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    return !this->has_gotten_resolve && this->is_valid_constraint(constraint);
}

auto typecheck::ResolveBindTo::resolveNext(const Constraint& constraint, const TypeManager* manager) -> bool {
    this->has_gotten_resolve = true;
    if (this->is_valid_constraint(constraint)) {
        if (this->pass->HasPermission(constraint, constraint.explicit_().var(), manager)) {
            this->pass->setResolvedType(constraint.explicit_().var(), constraint.explicit_().type());
        } else {
            return false;
        }
    }
    return true;
}

auto typecheck::ResolveBindTo::score(const Constraint& constraint, const TypeManager* manager) const -> std::size_t {
    if (!this->is_valid_constraint(constraint)) {
        return std::numeric_limits<std::size_t>::max();
    }

    if (this->pass->HasPermission(constraint, constraint.explicit_().var(), manager) && this->pass->hasResolvedType(constraint.explicit_().var())) {
        // If var is resolved, and it's type equals what it's supposed to be.
        if (google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(constraint.explicit_().var()), constraint.explicit_().type())) {
            // All args found, and matched up, and return types found and match up.
            return 0;
        }
    }

    return std::numeric_limits<std::size_t>::max();
}
