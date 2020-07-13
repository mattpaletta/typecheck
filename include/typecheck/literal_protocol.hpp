#pragma once

#include <typecheck_protos/type.pb.h>
#include <vector>

namespace typecheck {
	class LiteralProtocol {
	protected:
		Type ty(const std::string& _ty) const {
			Type ty;
            ty.mutable_raw()->set_name(_ty);
			return ty;
		}

	public:
		LiteralProtocol() {}
		~LiteralProtocol() = default;

		virtual std::vector<Type> getPreferredTypes() const { return {}; }
		virtual std::vector<Type> getOtherTypes() const { return {}; }
	};
}
