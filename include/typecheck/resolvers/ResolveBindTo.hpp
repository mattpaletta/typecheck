//
//  ResolveBindTo.hpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-15.
//

#pragma once

#include "typecheck/resolver.hpp"
#include "typecheck/debug.hpp"

#include <google/protobuf/util/message_differencer.h>

namespace typecheck {
    class ResolveBindTo : public Resolver {
    private:
        bool has_gotten_resolve = false;
    public:
        ResolveBindTo(ConstraintPass* pass, const std::size_t id) : Resolver(ConstraintKind::Bind, pass, id) {}

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const std::size_t id) const override {
            return std::make_unique<ResolveBindTo>(pass, id);
        }

        bool is_valid_constraint(const Constraint& constraint) const {
            return constraint.has_explicit_() && constraint.explicit_().has_type() && constraint.explicit_().has_var();
        }

        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override {
            return !this->has_gotten_resolve && this->is_valid_constraint(constraint);
        }

        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override {
            this->has_gotten_resolve = true;
            if (this->is_valid_constraint(constraint)) {
                if (!this->pass->hasResolvedType(constraint.explicit_().var())) {
                    this->pass->setResolvedType(constraint.explicit_().var(), constraint.explicit_().type());
                }
            }
            return true;
        }

        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override {
            if (!this->is_valid_constraint(constraint)) {
                return std::numeric_limits<std::size_t>::max();
            }

            if (this->pass->hasResolvedType(constraint.explicit_().var())) {
                // If var is resolved, and it's type equals what it's supposed to be.
                if (google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(constraint.explicit_().var()), constraint.explicit_().type())) {
                    // All args found, and matched up, and return types found and match up.
                    return 0;
                }
            }

            return std::numeric_limits<std::size_t>::max();
        }

    };
}
