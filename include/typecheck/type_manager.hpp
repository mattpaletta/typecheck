#pragma once

#include "constraint.hpp"
#include "constraint_pass.hpp"
#include "function_var.hpp"
#include "generic_type_generator.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

namespace typecheck {

	class TypeManager {
	public:
		TypeManager();
		~TypeManager() = default;

		// Not moveable or copyable
		TypeManager(const TypeManager&) = delete;
		TypeManager& operator=(const TypeManager&) = delete;
		TypeManager(TypeManager&&) = delete;
		TypeManager& operator=(TypeManager&&) = delete;

		// 'Register' types
        bool registerType(const std::string& name);
		bool registerType(const Type& name);
        bool hasRegisteredType(const std::string& name) const noexcept;
		bool hasRegisteredType(const Type& name) const noexcept;
        Type getRegisteredType(const std::string& name) const noexcept;
		Type getRegisteredType(const Type& name) const noexcept;

		// 'Register' convertible types
        bool setConvertible(const std::string& T0, const std::string& T1);
		bool setConvertible(const Type& T0, const Type& T1);
        bool isConvertible(const std::string& T0, const std::string& T1) const noexcept;
		bool isConvertible(const Type& T0, const Type& T1) const noexcept;
        std::vector<Type> getConvertible(const Type& T0) const;

		const typecheck::TypeVar CreateTypeVar();
		Constraint::IDType CreateFunctionHash(const std::string& name, const std::vector<std::string>& argNames) const;
		Constraint::IDType CreateLambdaFunctionHash(const std::vector<std::string>& argNames) const;
        Constraint::IDType CreateLiteralConformsToConstraint(const TypeVar& t0, const KnownProtocolKind::LiteralProtocol& protocol);
        Constraint::IDType CreateEqualsConstraint(const TypeVar& t0, const TypeVar& t1);
        Constraint::IDType CreateConvertibleConstraint(const TypeVar& T0, const TypeVar& T1);
        Constraint::IDType CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const std::vector<Type>& args, const Type& return_type);
        Constraint::IDType CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const std::vector<TypeVar>& argVars, const TypeVar& returnTypeVar);
        Constraint::IDType CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const FunctionVar& type);
        Constraint::IDType CreateBindFunctionConstraint( const Constraint::IDType& functionid, const TypeVar& T0, const std::vector<TypeVar>& args, const TypeVar& returnType);
        Constraint::IDType CreateBindToConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type);

        const Constraint* getConstraint(const Constraint::IDType id) const;

		std::optional<ConstraintPass> solve();
		std::vector<Constraint> constraints;

	private:
		std::vector<Type> registeredTypes;
		std::set<std::string> registeredTypeVars;
		std::map<std::string, std::set<std::string>> convertible;
		std::vector<FunctionVar> functions;

		GenericTypeGenerator type_generator;
		GenericTypeGenerator constraint_generator;
        std::vector<FunctionVar> getFunctionOverloads(const Constraint::IDType& funcID) const;

        // Internal helper
        Constraint* getConstraintInternal(const Constraint::IDType id);
	};
}
