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

namespace typecheck {
	class TypeManager {
	private:
		std::vector<Constraint> constraints;
	
		std::vector<Type> registeredTypes;
		std::map<std::string, std::set<std::string>> convertible;
		std::vector<FunctionDefinition> functions;

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

		std::string CreateTypeVar();
		std::size_t CreateConformsToConstraint(const Type& t0, const KnownProtocolKind& t1);
		std::size_t CreateEqualsConstraint(const typecheck::Type& t0, const typecheck::Type& t1);

		void solve();
		Type getResolvedType(const Type& type) const;
	};
}