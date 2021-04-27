#include <typecheck/type_manager.hpp>
#include <typecheck/constraint.hpp>           // for ConstraintKind
#include <typecheck/generic_type_generator.hpp>       // for GenericTypeGene...
#include <typecheck/debug.hpp>
#include <typecheck/type.hpp>                 // for Type, TypeVar

#include <typecheck/protocols/ExpressibleByFloatLiteral.hpp>
#include <typecheck/protocols/ExpressibleByIntegerLiteral.hpp>
#include <typecheck/protocols/ExpressibleByDoubleLiteral.hpp>

#include <constraint/solver.hpp>
#include <constraint/env.hpp>
#include <constraint/state.hpp>
#include <constraint/node.hpp>
#include <constraint/state_query.hpp>
#include <constraint/internal/utils.hpp>

#include <cppnotstdlib/strings.hpp>

#include <cassert>
#include <optional>
#include <list>
#include <queue>
#include <limits>                                     // for numeric_limits
#include <type_traits>                                // for move
#include <utility>                                    // for make_pair
#include <sstream>                                    // for std::stringstream
#include <string>                                     // for std::string

using namespace typecheck;

TypeManager::TypeManager() = default;

auto TypeManager::registerType(const std::string& name) -> bool {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->registerType(ty);
}

auto TypeManager::registerType(const Type& name) -> bool {
	// Determine if has type
	const auto alreadyHasType = this->hasRegisteredType(name);
	if (!alreadyHasType) {
		Type type;
		type.CopyFrom(name);
		this->registeredTypes.emplace_back(type);
	}
	return !alreadyHasType;
}

auto TypeManager::hasRegisteredType(const std::string& name) const noexcept -> bool {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

auto TypeManager::hasRegisteredType(const Type& name) const noexcept -> bool {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

auto TypeManager::getRegisteredType(const std::string& name) const noexcept -> Type {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->getRegisteredType(ty);
}

auto TypeManager::getRegisteredType(const Type& name) const noexcept -> Type {
	for (auto& type : this->registeredTypes) {
        if (type == name) {
			return type;
		}
	}

	return {};
}

auto TypeManager::getFunctionOverloads(const Constraint::IDType& funcID) const -> std::vector<FunctionVar> {
    std::vector<FunctionVar> overloads;
    for (const auto& overload : this->functions) {
        // Lookup by 'var', to deal with anonymous functions.
        if (overload.id() == funcID) {
            // Copy it over, and hand it over a 'function definition'.

            overloads.push_back(overload);
        }
    }

    return overloads;
}

auto TypeManager::setConvertible(const std::string& T0, const std::string& T1) -> bool {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);

    return this->setConvertible(t0, t1);
}

auto TypeManager::CreateFunctionHash(const std::string& name, const std::vector<std::string>& argNames) const -> Constraint::IDType {
    return static_cast<Constraint::IDType>(std::hash<std::string>()(name + cppnotstdlib::join(argNames, ":")));
}

auto TypeManager::CreateLambdaFunctionHash(const std::vector<std::string>& argNames) const -> Constraint::IDType {
    // Lambda functions use the address of the arguments as part of the name
    const void* address = static_cast<const void*>(&argNames);
    std::stringstream ss;
    ss << address;
    std::string lambdaAddress = ss.str();

    return this->CreateFunctionHash("lambda" + lambdaAddress, argNames);
}

auto TypeManager::setConvertible(const Type& T0, const Type& T1) -> bool {
    if (T0 == T1) {
		return true;
	}

	const auto& t0_ptr = this->getRegisteredType(T0);
	const auto& t1_ptr = this->getRegisteredType(T1);

    // Function types not convertible
    if (t0_ptr.has_func() || t1_ptr.has_func()) {
        // Functions not convertible to each other
        return false;
    } else if (!t0_ptr.raw().name().empty() && !t1_ptr.raw().name().empty() && this->convertible[t0_ptr.raw().name()].find(t1_ptr.raw().name()) == this->convertible[t0_ptr.raw().name()].end()) {
		// Convertible from T0 -> T1
        this->convertible[t0_ptr.raw().name()].insert(t1_ptr.raw().name());
		return true;
	}
	return false;
}

auto TypeManager::isConvertible(const std::string& T0, const std::string& T1) const noexcept -> bool {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);
    return this->isConvertible(t0, t1);
}

auto TypeManager::isConvertible(const Type& T0, const Type& T1) const noexcept -> bool {
    if (T0.raw().name().empty() || T1.raw().name().empty()) {
        // Undefined types, stop here.
        return false;
    }

    // Function types not convertible
    if (T0.has_func() || T1.has_func()) {
        return false;
    }

    if (T0 == T1) {
		return true;
	}

    // Because they're not functions, they must both be raw.
    if (this->convertible.find(T0.raw().name()) == this->convertible.end()) {
		// T0 is not in the map, meaning the conversion won't be there.
		return false;
	}

    if (this->convertible.at(T0.raw().name()).find(T1.raw().name()) != this->convertible.at(T0.raw().name()).end()) {
		// Convertible from T0 -> T1
		return true;
	}
	return false;
}

auto TypeManager::getConvertible(const Type& T0) const -> std::vector<Type> {
    std::vector<Type> out;

    // Function types not convertible
    if (T0.has_func()) {
        return out;
    }

    if (this->convertible.find(T0.raw().name()) != this->convertible.end()) {
        // Load into vector
        for (auto& convert : this->convertible.at(T0.raw().name())) {
            Type type;
            type.mutable_raw()->set_name(convert);
            out.emplace_back(std::move(type));
        }
    }

    return out;
}

auto TypeManager::CreateTypeVar() -> const TypeVar {
	const auto var = this->type_generator.next();

	this->registeredTypeVars.insert(var);

	TypeVar type;
	type.set_symbol(var);
	return type;
}

auto TypeManager::getConstraintInternal(const Constraint::IDType id) -> Constraint* {
    for (auto& constraint : this->constraints) {
        if (constraint.id() == id) {
            return &constraint;
        }
    }

    return nullptr;
}

auto TypeManager::getConstraint(const Constraint::IDType id) const -> const Constraint* {
	for (auto& constraint : this->constraints) {
		if (constraint.id() == id) {
			return &constraint;
		}
	}

	return nullptr;
}

namespace {
    typecheck::Type TypeFromString(const std::string& val, const constraint::Solution& sol) {
        if (cppnotstdlib::explode(val, '|').size() == 1) {
            return Type(RawType(val));
        } else {
            auto fvar = typecheck::FunctionVar::unserialize(val);
            typecheck::FunctionDefinition funcDef;
            funcDef.set_name(fvar.name());
            funcDef.set_id(fvar.id());
            funcDef.mutable_returntype()->CopyFrom(TypeFromString(sol.at(fvar.returnvar().symbol()).to_string(), sol));
            for (const auto& a : fvar.args()) {
                funcDef.add_args()->CopyFrom(TypeFromString(sol.at(a.symbol()).to_string(), sol));
            }
            return Type(funcDef);
        }
    }

    void AddTypeToDomain(constraint::Domain::data_type& domain, const typecheck::Type& type) {
        if (type.has_raw()) {
            domain.emplace_back(type.raw().name());
        } else if (type.has_func()) {
            domain.emplace_back(type.func().name());
        }
    }

    void AddTypeToDomain(constraint::Domain::data_type& domain, const typecheck::FunctionVar& type) {
        domain.emplace_back(type.serialize());
    }

    using distance_type = std::function<std::size_t(const constraint::State&)>;

    template<typename T>
    void AddLiteralProtocolTypes(constraint::Domain::data_type& domain) {
        T protocol;
        for (const auto& ty : protocol.getPreferredTypes()) {
            AddTypeToDomain(domain, ty);
        }

        for (const auto& ty : protocol.getOtherTypes()) {
            AddTypeToDomain(domain, ty);
        }
    }

    template<typename T>
    void AddHeuristicProtocolFuncs(std::vector<constraint::Solver::DistanceFunc>& heuristics, std::vector<constraint::Solver::DistanceFunc>& actuals, const std::string& var) {
        heuristics.emplace_back([var](const constraint::StateQuery& state) {
            T protocol;
            if (state.isAssigned(var)) {
                // Check if in preferred list or not.
                for (const auto& ty : protocol.getPreferredTypes()) {
                    if (ty.raw().name() == state.variable_map.at(var).to_string()) {
                        return 0;
                    }
                }

                return 1;
            }

            // Unknown.
            return 0;
        });

        // Use the same one for the actual solution, except punish more for not assigned.
        actuals.emplace_back([var](const constraint::StateQuery& state) {
            T protocol;
            if (state.isAssigned(var)) {
                // Check if in preferred list or not.
                for (const auto& ty : protocol.getPreferredTypes()) {
                    if (ty.raw().name() == state.variable_map.at(var).to_string()) {
                        return 0;
                    }
                }

                return 1;
            }

            // Unknown.
            return 0;
        });
    }
}

auto TypeManager::solve() -> std::optional<ConstraintPass> {
    constraint::Solver constraint_solver;
    std::set<std::string> all_variable_names;

    std::vector<constraint::Solver::DistanceFunc> heuristcFuncs;
    std::vector<constraint::Solver::DistanceFunc> distanceFuncs;

#pragma mark - Gather All Data
    auto insert_if_not_exists = [&constraint_solver, &all_variable_names](const std::string& var, const constraint::Domain& domain) {
        if (domain.size() == 0) {
            std::cout << "Warning: Domain Empty for variable: " << var << std::endl;
        }
        
        if (all_variable_names.find(var) == all_variable_names.end()) {
            constraint_solver.addVariable(var, domain);
            all_variable_names.insert(var);
        }
    };

    // Var Domain
    const auto varDomain = [this] {
        constraint::Domain::data_type domain;
        for (const auto& ty : this->registeredTypes) {
            AddTypeToDomain(domain, ty);
        }

        for (const auto& func : this->functions) {
            AddTypeToDomain(domain, func);
        }

        return constraint::Domain(domain);
    }();

    for (const auto& constraint : this->constraints) {
        if (constraint.has_conforms()) {
            const auto conforms = constraint.conforms();
            if (conforms.has_type() && conforms.has_protocol()) {
                const auto var = conforms.type().symbol();
                const auto protocol = conforms.protocol();
                constraint::Domain::data_type domain;
                switch (protocol.literal()) {
                case KnownProtocolKind::ExpressibleByFloat:
                    AddLiteralProtocolTypes<ExpressibleByFloatLiteral>(domain);
                    AddHeuristicProtocolFuncs<ExpressibleByFloatLiteral>(heuristcFuncs, distanceFuncs, var);
                    break;
                case KnownProtocolKind::ExpressibleByDouble:
                    AddLiteralProtocolTypes<ExpressibleByDoubleLiteral>(domain);
                    AddHeuristicProtocolFuncs<ExpressibleByDoubleLiteral>(heuristcFuncs, distanceFuncs, var);
                    break;
                case KnownProtocolKind::ExpressibleByInteger:
                    AddLiteralProtocolTypes<ExpressibleByIntegerLiteral>(domain);
                    AddHeuristicProtocolFuncs<ExpressibleByIntegerLiteral>(heuristcFuncs, distanceFuncs, var);
                    break;
                default:
                    std::cout << "Unsupported Literal" << std::endl;
                    return std::nullopt;
                    break;
                }
                insert_if_not_exists(var, varDomain);

                // conforms literal is implied by its domain.
                constraint_solver.addConstraint(std::vector{var}, [var, domain](const constraint::Env& env) {
                    for (const auto& ty : domain) {
                        if (env.at(var) == ty) {
                            return true;
                        }
                    }

                    return false;
                });
            } else {
                std::cout << "Malformed Conforms Constraint" << std::endl;
                return std::nullopt;
            }
        } else if (constraint.has_types()) {
            const auto types = constraint.types();
            std::vector<std::string> type_names;
            if (types.has_first()) {
                type_names.push_back(types.first().symbol());
            }
            if (types.has_second()) {
                type_names.push_back(types.second().symbol());
            }
            if (types.has_third()) {
                type_names.push_back(types.third().symbol());
            }

            if (type_names.empty()) {
                std::cout << "Malformed Types Constraint" << std::endl;
            } else {
                for (const auto& ty : type_names) {
                    insert_if_not_exists(ty, varDomain);
                }

                switch (constraint.kind()) {
                case Conversion:
                    constraint_solver.addConstraint(std::vector{type_names}, [type_names, C = &convertible](const constraint::Env& env) {
                        const auto firstVarValue = env.at(type_names.at(0));
                        const auto secondVarValue = env.at(type_names.at(1));

                        if (firstVarValue == secondVarValue) {
                            return true;
                        }

                        const auto it = C->find(firstVarValue.to_string());
                        if (it == C->end()) {
                            return false;
                        }

                        return it->second.find(secondVarValue.to_string()) != it->second.end();
                    });
                    break;
                case Equal:
                    constraint_solver.addConstraint(std::vector{type_names}, [type_names](const constraint::Env& env) {
                        const auto firstVar = env.at(type_names.at(0));
                        for (const auto& ty : type_names) {
                            if (firstVar != env.at(ty)) {
                                return false;
                            }
                        }

                        return true;
                    });
                    break;
                default:
                    std::cout << "Unimplemented Constraint Kind: " << constraint.kind() << std::endl;
                    assert(false);
                    break;
                }
            }
        } else if (constraint.has_overload()) {
            const auto overload = constraint.overload();

            std::vector<std::string> overloadVariables;
            overloadVariables.emplace_back(overload.type().symbol());
            overloadVariables.emplace_back(overload.returnvar().symbol());
            for (auto i = 0; i < overload.argvars_size(); ++i) {
                overloadVariables.emplace_back(overload.argvars(i).symbol());
            }

            // Gather all overloads.
            const auto funcFamily = this->getFunctionOverloads(overload.functionid());
            std::vector<std::vector<std::string>> all_func_dependant_variables;
            constraint::Domain::data_type typeDomain;
            for (const auto& func : funcFamily) {
                std::vector<std::string> funcDependantVariables;
                funcDependantVariables.emplace_back(func.returnvar().symbol());
                for (const auto& arg : func.args()) {
                    funcDependantVariables.emplace_back(arg.symbol());
                }

                all_func_dependant_variables.emplace_back(funcDependantVariables);
                typeDomain.emplace_back(func.serialize());
            }

            insert_if_not_exists(overload.type().symbol(), typeDomain);
            for (const auto& a : overloadVariables) {
                insert_if_not_exists(a, varDomain);
            }
            for (const auto& a : all_func_dependant_variables) {
                for (const auto& b : a) {
                    insert_if_not_exists(b, varDomain);
                }
            }


            for (auto i = 0; i < funcFamily.size(); ++i) {
                const auto& vars = all_func_dependant_variables.at(i);
                const auto& func = funcFamily.at(i);

                std::vector<std::string> overloadConstraintVars;

                // Copy the variables from the overload constraint
                std::copy(overloadVariables.begin(), overloadVariables.end(), std::back_inserter(overloadConstraintVars));

                // Copy the variables from the function definition
                std::copy(vars.begin(), vars.end(), std::back_inserter(overloadConstraintVars));

                auto allFuncDefinitionVariablesAssigned = [vars](const constraint::Env& env) {
                    for (const auto& a : vars) {
                        if (!env.isAssigned(a)) {
                            return false;
                        }
                    }
                    return true;
                };

                constraint_solver.addConstraint(overloadConstraintVars, [overload, funcDefinition = func, check = std::move(allFuncDefinitionVariablesAssigned)](const constraint::Env& env) {
                    if (!check(env)) {
                        // If not all the variables of the function are assigned, say it's fine, and the other one will pick it up.
                        return true;
                    }

                    if (env.at(overload.type().symbol()).to_string() != funcDefinition.serialize()) {
                        // This is not the overload we are looking for.
                        return true;
                    }

                    auto compare_vars = [&env](const typecheck::TypeVar& vA, const typecheck::TypeVar& vB) {
                        return env.at(vA.symbol()).to_string() == env.at(vB.symbol()).to_string();
                    };

                    // This is the the overload, check everything matches up.
                    if (overload.argvars_size() != funcDefinition.args().size()) {
                        return false;
                    }

                    if (!compare_vars(overload.returnvar(), funcDefinition.returnvar())) {
                        return false;
                    }

                    for (auto i = 0; i < funcDefinition.args().size(); ++i) {
                        if (!compare_vars(overload.argvars(i), funcDefinition.args().at(i))) {
                            return false;
                        }
                    }

                    return true;
                });
            }

        } else if (constraint.has_explicit_()) {
            const auto explicit_ = constraint.explicit_();
            if (explicit_.has_var() && explicit_.has_type()) {
                const auto var = explicit_.var();
                const auto type = explicit_.type();

                insert_if_not_exists(var.symbol(), varDomain);
                constraint_solver.addConstraint(std::vector{var.symbol()}, [var, type](const constraint::Env& env) {
                    if (type.has_raw()) {
                        return env.at(var.symbol()).to_string() == type.raw().name();
                    } else {
                        return false;
                    }
                });
            } else {
                std::cout << "Malformed Explicit Constraint" << std::endl;
                return std::nullopt;
            }
        } else {
            std::cout << "Unknown Constraint Type" << std::endl;
            return std::nullopt;
        }
    }


    const auto numVariables = all_variable_names.size();
    auto heuristic = [heuristics = std::move(heuristcFuncs), numVariables](const constraint::StateQuery& state) {
        // Calculate the difference, allows us to measure meaningful progress
        std::size_t sum = numVariables + state.numConstraints() - state.numSatisfied();
        for (const auto& H : heuristics) {
            sum += H(state);
        }
        return sum;
    };

    auto actualDistance = [actual = std::move(distanceFuncs), numVariables](const constraint::StateQuery& state) {
        // Calculate the difference, allows us to measure meaningful progress
        std::size_t sum = numVariables + state.numConstraints();
        for (const auto& G : actual) {
            sum += G(state);
        }
        return sum;
    };

    const auto solution = constraint_solver.getOptimizedSolution(std::move(heuristic), std::move(actualDistance));
    const auto hasSolution = solution.has_value();
    if (!hasSolution) {
        return std::nullopt;
    }

    ConstraintPass pass;
    for (const auto& var : all_variable_names) {
        const auto val = solution->at(var);
        pass.setResolvedType(var, TypeFromString(val.to_string(), *solution));
    }
    return pass;
}
