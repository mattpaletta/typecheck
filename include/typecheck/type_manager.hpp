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
	class ConstraintPass;
	class TypeManager {
		friend ConstraintPass;
		friend TypeSolver;
    public:
        using IDType = long long;
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

        void SortConstraints();
        int getConstraintKindScore(const typecheck::ConstraintKind& kind) const;

	public:
        bool use_reverse_sort = false;

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
        std::vector<Type> getFunctionOverloads(const TypeVar& var) const;

		// `Register` resolvers
		bool registerResolver(std::unique_ptr<Resolver>&& resolver);

		typecheck::TypeVar CreateTypeVar();
		IDType CreateLiteralConformsToConstraint(const TypeVar& t0, const KnownProtocolKind_LiteralProtocol& protocol);
		IDType CreateEqualsConstraint(const TypeVar& t0, const TypeVar& t1);
        IDType CreateConvertibleConstraint(const TypeVar& T0, const TypeVar& T1);
        IDType CreateApplicableFunctionConstraint(const TypeVar& T0, const std::vector<Type>& args, const Type& return_type);
        IDType CreateApplicableFunctionConstraint(const TypeVar& T0, const Type& type);
        IDType CreateBindFunctionConstraint(const TypeVar& T0, const std::vector<TypeVar>& args, const TypeVar& returnType);
        IDType CreateBindToConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type);

        const Constraint* getConstraint(const IDType id) const;

		bool solve();
		Type getResolvedType(const TypeVar& type) const;
	};
}
