#pragma once

#include "type_var.hpp"
#include "type.hpp"

#include <unordered_map>                     // for unordered_map
#include <memory>                            // for unique_ptr
#include <string>

namespace typecheck {
	class ConstraintPass {
    public:
		ConstraintPass() = default;
		~ConstraintPass() = default;

		Type getResolvedType(const TypeVar& var) const;
		bool hasResolvedType(const TypeVar& var) const;
		bool setResolvedType(const TypeVar& var, const Type& type);

	private:
        // The key must be string, because 'typeVar' not comparable.
        std::unordered_map<std::string, Type> resolvedTypes;
	};
}
