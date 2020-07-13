#pragma once
#include <stddef.h>                          // for size_t
#include <typecheck_protos/constraint.pb.h>  // for ConstraintKind
#include <iosfwd>                            // for string
#include <limits>                            // for numeric_limits
#include <map>                               // for map
#include <memory>                            // for unique_ptr
#include <string>                            // for operator<
#include "typecheck_protos/type.pb.h"        // for Type
namespace typecheck { class Resolver; }  // lines 11-11
namespace typecheck { class TypeManager; }  // lines 12-12
namespace typecheck { class TypeSolver; }  // lines 13-13

namespace typecheck {
	class TypeSolver;
	class TypeManager;
	class ConstraintPass {
		friend TypeSolver;

		// Not publicly copyable
		ConstraintPass(const ConstraintPass&) = default;
		ConstraintPass& operator=(const ConstraintPass&) = default;

		// Holds pointer to previous pass (if applicable)
		const ConstraintPass* prev = nullptr;
	private:
		std::size_t score = std::numeric_limits<std::size_t>::max();
		std::map<std::string, std::string> resolvedTypes;

		mutable std::map<ConstraintKind, std::unique_ptr<Resolver>> resolvers;
		Resolver* GetResolver(const Constraint& constraint, const TypeManager* manager);
		Resolver* GetResolverRec(const Constraint& constraint, const TypeManager* manager) const;

		bool is_valid;
	public:
		ConstraintPass() = default;
		~ConstraintPass() = default;

		// Passes are not copyable
		ConstraintPass(ConstraintPass&&) = default;
		ConstraintPass& operator=(ConstraintPass&&) = default;

		ConstraintPass CreateCopy() const;
		void CopyToExisting(ConstraintPass* dest) const;

		std::size_t CalcScore(const TypeManager* manager);
		bool IsValid(const TypeManager* manager);

		Type getResolvedType(const TypeVar& var) const;
		bool hasResolvedType(const TypeVar& var) const;
		void setResolvedType(const TypeVar& var, const Type& type);
	};
}
