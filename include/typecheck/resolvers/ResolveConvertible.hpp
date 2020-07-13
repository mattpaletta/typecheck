//
//  ResolveConvertible.hpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-13.
//

#pragma once

#include "typecheck/resolver.hpp"
#include "typecheck/type_manager.hpp"

#include <iostream>
#include <memory>
#include <typecheck_protos/constraint.pb.h>

namespace typecheck {
    class ResolveConvertible : public Resolver {
    private:
        bool did_find_convertible = false;
        std::vector<Type> options;

    public:
        ResolveConvertible(ConstraintPass* pass, const std::size_t id) : Resolver(ConstraintKind::Conversion, pass, id) {}

        virtual std::unique_ptr<Resolver> clone(ConstraintPass* pass, const std::size_t id) const override {
            return std::make_unique<ResolveConvertible>(pass, id);
        }

        bool doInitialIterationSetup(const Constraint& constraint, const TypeManager* manager) {
            if (!constraint.has_types() || !constraint.types().has_first() || !constraint.types().has_second()) {
                std::cout << "Malformed ResolveConformsTo Constraint, missing conforms, protocol or type." << std::endl;
                return false;
            }
            
            if (this->pass && this->pass->hasResolvedType(constraint.types().first())) {
                this->options = manager->getConvertible( this->pass->getResolvedType(constraint.types().first()) );
            }

            did_find_convertible = this->options.size() > 0;
            return did_find_convertible;
        }

        virtual bool hasMoreSolutions(const Constraint& constraint, const TypeManager* manager) override {
            // This will be called every time

            if (!this->did_find_convertible) {
                // The first time do setup
                return this->doInitialIterationSetup(constraint, manager);
            } else {
                return this->options.size() > 0;
            }
        }

        virtual bool resolveNext(const Constraint& constraint, const TypeManager* manager) override {
            if (this->options.size() > 0) {
                auto nextType = this->options.back();
                this->options.pop_back();

                auto typeVar = constraint.types().second();
                this->pass->setResolvedType(typeVar, nextType);
                return true;
            }
            return false;
        }

        virtual std::size_t score(const Constraint& constraint, const TypeManager* manager) const override {
            const auto T0 = constraint.types().first();
            const auto T1 = constraint.types().second();

            if (this->pass && this->pass->hasResolvedType(T0) && this->pass->hasResolvedType(T1)) {
                if (manager->isConvertible(this->pass->getResolvedType(T0), this->pass->getResolvedType(T1))) {
                    // Make sure they are resolved and convertible, otherwise, invalid.
                    return 0;
                }
            }

            // This solution is not valid
            return std::numeric_limits<std::size_t>::max();
        }
    };
}
