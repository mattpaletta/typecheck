//
//  ResolveBindTo.hpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-13.
//
#pragma once

#include "typecheck/resolver.hpp"
#include "typecheck/debug.hpp"

#include <google/protobuf/util/message_differencer.h>

namespace typecheck {
    class ResolveBindOverload : public Resolver {
    private:
        bool did_find_overloads = false;
        std::vector<Type> overloads;
        std::size_t current_overload_i = std::numeric_limits<std::size_t>::max();
    public:
        ResolveBindOverload(ConstraintPass* pass, const std::size_t id) : Resolver(ConstraintKind::BindOverload, pass, id) {}

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const std::size_t id) const override {
            return std::make_unique<ResolveBindOverload>(pass, id);
        }

        bool is_valid_constraint(const Constraint& constraint) const {
            return constraint.has_overload() && constraint.overload().has_type();
        }

        void doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager) {
            if (this->pass && this->is_valid_constraint(constraint)) {
                // Try and get registered overloads
                this->overloads = manager->getFunctionOverloads(constraint.overload().type());
                // Reset index to 0.
                this->current_overload_i = 0;
                this->did_find_overloads = true;
            }
        }

        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override {
            if (!this->did_find_overloads) {
                // The first time do setup
                this->doInitialIterationSetup(constraint, manager);
            } else {
                this->current_overload_i++;
            }

            return this->is_valid_constraint(constraint) && this->overloads.size() > 0 && this->current_overload_i < this->overloads.size();
        }

        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override {
            if (this->did_find_overloads) {
                typecheck::Type nextOverload = this->overloads.at(this->current_overload_i);

                while (nextOverload.has_func() && nextOverload.func().args_size() != constraint.overload().argvars_size() && this->current_overload_i < this->overloads.size()) {
                    nextOverload = this->overloads.at(this->current_overload_i++);
                    // Skip over any that don't have the same number of arguments.
                }

                if (nextOverload.has_func() && nextOverload.func().args_size() == constraint.overload().argvars_size()) {
                    // Only proceed if we found an overload with the same number of arguments

                    const auto typeVar = constraint.overload().type();
                    this->pass->setResolvedType(typeVar, nextOverload);

                    // try and fill in vars with overload type
                    for (std::size_t i = 0; i < constraint.overload().argvars_size(); ++i) {
                        const auto arg = constraint.overload().argvars(i);
                        if (!this->pass->hasResolvedType(arg)) {
                            // Don't override already resolved types, they will fail in score
                            this->pass->setResolvedType(arg, nextOverload.func().args(i));
                        }
                    }

                    if (!this->pass->hasResolvedType(constraint.overload().returnvar())) {
                        this->pass->setResolvedType(constraint.overload().returnvar(), nextOverload.func().returntype());
                    }

                    return true;
                }
            }

            return false;
        }

        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override {
            if (!this->is_valid_constraint(constraint)) {
                return std::numeric_limits<std::size_t>::max();
            }

            if (this->pass && this->pass->hasResolvedType(constraint.overload().type()) && this->current_overload_i < this->overloads.size()) {
                const auto currentOverload = this->overloads.at(this->current_overload_i);

                for (std::size_t i = 0; i < constraint.overload().argvars_size(); ++i) {
                    const auto arg = constraint.overload().argvars(i);
                    if (this->pass->hasResolvedType(arg)) {
                        // Make sure the arg types match up
                        if (!google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(arg), currentOverload.func().args(i))) {
                            return std::numeric_limits<std::size_t>::max();
                        }
                    }
                }

                if (this->pass->hasResolvedType(constraint.overload().returnvar())) {
                    // Make sure the return types match up
                    if (!google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(constraint.overload().returnvar()), currentOverload.func().returntype())) {
                        return std::numeric_limits<std::size_t>::max();
                    }
                }

                // All args found, and matched up, and return types found and match up.
                return 0;
            }

            return std::numeric_limits<std::size_t>::max();
        }

    };
}
