#pragma once
#include <numeric>
#include <vector>
#include <map>
#include <typecheck_protos/constraint.pb.h>

namespace typecheck {
	class ConstraintPass {
	public:
		std::vector<typecheck::Constraint> constraints;
		std::size_t score = std::numeric_limits<std::size_t>::max();
		std::map<std::string, std::string> resolvedTypes;

		ConstraintPass() : constraints() {}
		~ConstraintPass() {}

		static typecheck::ConstraintPass CreateNewPass(const std::vector<typecheck::Constraint>& constraints);
		static std::size_t Score(const ConstraintPass& pass);

		bool hasResolvedType(const typecheck::Type& type) const;
	};
}