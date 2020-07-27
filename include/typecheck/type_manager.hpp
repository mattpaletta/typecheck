#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <typecheck_protos/constraint.pb.h>
#include <typecheck_protos/type.pb.h>
#include "type_solver.hpp"
#include "generic_type_generator.hpp"
#include "resolver.hpp"

namespace typecheck {
    class Resolver;
	class ConstraintPass;
	class TypeManager {
		friend ConstraintPass;
		friend TypeSolver;
	private:
		std::vector<Constraint> constraints;

		std::vector<Type> registeredTypes;
		std::set<std::string> registeredTypeVars;
		std::map<std::string, std::set<std::string>> convertible;
		std::vector<FunctionVar> functions;
		std::map<ConstraintKind, std::unique_ptr<Resolver>> registeredResolvers;

		TypeSolver solver;
		GenericTypeGenerator type_generator;
		GenericTypeGenerator constraint_generator;

        void SortConstraints();
        int getConstraintKindScore(const typecheck::ConstraintKind& kind) const;

        // Internal helper
        Constraint* getConstraintInternal(const ConstraintPass::IDType id);

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

		// 'Register' function definition using Constraint
        // Warning, this method is slow and should not be used frequently.
        std::vector<FunctionDefinition> getFunctionOverloads(const ConstraintPass::IDType& var, const ConstraintPass* pass) const;
        bool canGetFunctionOverloads(const ConstraintPass::IDType& var, const ConstraintPass* pass) const;

		// `Register` resolvers
		bool registerResolver(std::unique_ptr<Resolver>&& resolver);

		const typecheck::TypeVar CreateTypeVar();
		ConstraintPass::IDType CreateFunctionHash(const std::string& name, const std::vector<std::string>& argNames) const;
		ConstraintPass::IDType CreateLambdaFunctionHash(const std::vector<std::string>& argNames) const;
        ConstraintPass::IDType CreateLiteralConformsToConstraint(const TypeVar& t0, const KnownProtocolKind_LiteralProtocol& protocol);
        ConstraintPass::IDType CreateEqualsConstraint(const TypeVar& t0, const TypeVar& t1);
        ConstraintPass::IDType CreateConvertibleConstraint(const TypeVar& T0, const TypeVar& T1);
        ConstraintPass::IDType CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const std::vector<Type>& args, const Type& return_type);
        ConstraintPass::IDType CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const std::vector<TypeVar>& argVars, const TypeVar& returnTypeVar);
        ConstraintPass::IDType CreateApplicableFunctionConstraint(const ConstraintPass::IDType& functionid, const FunctionVar& type);
        ConstraintPass::IDType CreateBindFunctionConstraint( const ConstraintPass::IDType& functionid, const TypeVar& T0, const std::vector<TypeVar>& args, const TypeVar& returnType);
        ConstraintPass::IDType CreateBindToConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type);

        const Constraint* getConstraint(const ConstraintPass::IDType id) const;

		bool solve();
		const Type getResolvedType(const TypeVar& type) const;
	};
}
