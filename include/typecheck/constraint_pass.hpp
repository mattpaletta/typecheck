#pragma once
#include <stddef.h>                          // for size_t
#include <typecheck_protos/constraint.pb.h>  // for ConstraintKind
#include <iosfwd>                            // for string
#include <limits>                            // for numeric_limits
#include <map>                               // for map
#include <memory>                            // for unique_ptr
#include <string>                            // for operator<
#include "typecheck_protos/type.pb.h"        // for Type
#include <deque>                             // for deque
namespace typecheck { class Resolver; }  // lines 11-11
namespace typecheck { class TypeManager; }  // lines 12-12
namespace typecheck { class TypeSolver; }  // lines 13-13

namespace typecheck {
	class TypeSolver;
	class TypeManager;
	class ConstraintPass {
		friend TypeSolver;

		// Holds pointer to previous pass (if applicable)
		ConstraintPass* prev = nullptr;
	private:
		std::map<std::size_t, std::size_t> scores;
        std::size_t score = std::numeric_limits<std::size_t>::max();
        
        // The key must be string, because 'typeVar' not comparable.
		std::map<std::string, Type> resolvedTypes;

		mutable std::map<std::size_t, std::unique_ptr<Resolver>> resolvers;
		Resolver* GetResolver(const Constraint& constraint, const TypeManager* manager);
		Resolver* GetResolverRec(const Constraint& constraint, const TypeManager* manager) const;
        void ResetResolver(const typecheck::Constraint& constraint, const TypeManager* manager);

    public:
		ConstraintPass() = default;
		~ConstraintPass() = default;

		// Passes are not copyable
		ConstraintPass(ConstraintPass&&) = default;
		ConstraintPass& operator=(ConstraintPass&&) = default;
        ConstraintPass(const ConstraintPass&) = delete;
        ConstraintPass& operator=(const ConstraintPass&) = delete;

		ConstraintPass CreateCopy();
		void CopyToExisting(ConstraintPass* dest) const;

		std::size_t CalcScore(const std::deque<std::size_t>& indices, const TypeManager* manager, const bool cached = false);

        // Calculated based on score
		bool IsValid(const TypeManager* manager);

		Type getResolvedType(const TypeVar& var) const;
		bool hasResolvedType(const TypeVar& var) const;
		void setResolvedType(const TypeVar& var, const Type& type);
	};
}
