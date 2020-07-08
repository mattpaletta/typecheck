#pragma once
#include <vector>
#include <map>

#include "type.hpp"
#include <typecheck_protos/constraint.pb.h>
#include "constraint_pass.hpp"
#include <typecheck_protos/function_definition.pb.h>

namespace typecheck {
	class TypeSolver {
	private:
		std::vector<Constraint> constraints;
		std::vector<Type> types;
		std::vector<FunctionDefinition> functions;

		std::map<std::size_t, std::vector<std::size_t>> typeRefGraph;
		std::map<std::string, std::vector<FunctionDefinition>> funcOverloads;
		std::map<std::string, std::set<std::string>> conversions;

		ConstraintPass last_pass;

		// Implementations found in `type_solver+init_pass.cpp`
		void InitPasses();
		void RemoveDuplicates();
		void FindOverloads();
		void BuildRefGraph();

		// Implementations found in `type_solver+solver.cpp`
		void DoPass(ConstraintPass* pass);
		bool HasMorePasses(const ConstraintPass& pass);
		std::vector<Type> getAllConforms(const Constraint& constraint) const;
		Type getPreferredConforms(const Constraint& constraint) const;
		bool ResolveConformsTo(const Constraint& constraint, ConstraintPass* pass);
		bool ResolveEquals(const typecheck::Constraint& constraint, typecheck::ConstraintPass* pass);
	public:
		TypeSolver();
		~TypeSolver() = default;

		void setConstraints(const std::vector<Constraint>& _constraints);
		void setTypes(const std::vector<Type> _types);
		void setFunctions(const std::vector<FunctionDefinition>& funcDef);
		void setConversions(const std::map<std::string, std::set<std::string>>& _conversions);
		bool solve();
		Type getResolvedType(const Type& _typeVar) const;
	};
}