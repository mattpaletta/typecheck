//
//  ResolveApplicableFunction.hpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-13.
//
#pragma once

#include "typecheck/resolver.hpp"
#include "typecheck/debug.hpp"

#include <google/protobuf/util/message_differencer.h>

namespace typecheck {
    class ResolveApplicableFunction : public Resolver {
    private:
        bool has_gotten_resolve = false;
    public:
        ResolveApplicableFunction(ConstraintPass* pass, const std::size_t id) : Resolver(ConstraintKind::ApplicableFunction, pass, id) {}

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const std::size_t id) const override {
            return std::make_unique<ResolveApplicableFunction>(pass, id);
        }

        bool is_valid_constraint(const Constraint& constraint) const {
            return constraint.has_explicit_() && constraint.explicit_().has_var() && constraint.explicit_().has_type() && constraint.explicit_().type().has_func();
        }

        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override {
            return this->pass && !has_gotten_resolve && this->is_valid_constraint(constraint);
        }

        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override {
            this->has_gotten_resolve = true;

            // See bindOverload for selecting the correct implementation

            // Don't have a pass, can't resolve.
            return true;
        }

        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override {
            if (!this->is_valid_constraint(constraint)) {
                return std::numeric_limits<std::size_t>::max();
            }

            // This is pretty simple, used to define functions, so it passes!

            return 0;
        }

    };
}
