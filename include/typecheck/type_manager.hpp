#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <typecheck_protos/constraint.pb.h>
#include <typecheck_protos/type.pb.h>
#include <typecheck_protos/function_definition.pb.h>
#include "type_solver.hpp"
#include "generic_type_generator.hpp"
#include "resolver.hpp"

namespace typecheck {
	class ConstraintPass;
	class TypeManager {
		friend ConstraintPass;
		friend TypeSolver;
	private:
		std::vector<Constraint> constraints;

		std::vector<Type> registeredTypes;
		std::set<std::string> registeredTypeVars;
		std::map<std::string, std::set<std::string>> convertible;
		std::vector<FunctionDefinition> functions;
		std::map<ConstraintKind, std::unique_ptr<Resolver>> registeredResolvers;

		TypeSolver solver;
		GenericTypeGenerator type_generator;
		GenericTypeGenerator constraint_generator;

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
		bool hasRegisteredType(const std::string& name) const noexcept;
		Type getRegisteredType(const std::string& name) const noexcept;

		// 'Register' convertible types
		bool setConvertible(const std::string& T0, const std::string& T1);
		bool isConvertible(const std::string& T0, const std::string& T1) const noexcept;

		// 'Register' function definitions
		void registerFunctionDefinition(const FunctionDefinition& funcDef);

		// `Register` resolvers
		bool registerResolver(std::unique_ptr<Resolver>&& resolver);

		typecheck::TypeVar CreateTypeVar();
		std::size_t CreateLiteralConformsToConstraint(const TypeVar& t0, const typecheck::KnownProtocolKind_LiteralProtocol& protocol);
		std::size_t CreateEqualsConstraint(const typecheck::TypeVar& t0, const typecheck::TypeVar& t1);
		const Constraint* getConstraint(const std::size_t id) const;

		bool solve();
		Type getResolvedType(const TypeVar& type) const;
	};
}
