#pragma once

#include <typecheck_protos/type.pb.h>
#include <vector>

namespace typecheck {
	class LiteralProtocol {
	protected:
		Type ty(const std::string& _ty) const noexcept {
			Type ty;
            ty.mutable_raw()->set_name(_ty);
			return ty;
		}

	public:
		LiteralProtocol() {}
		virtual ~LiteralProtocol() = default;

        // LCOV_EXCL_START
		virtual std::vector<Type> getPreferredTypes() const noexcept { return {}; }
		virtual std::vector<Type> getOtherTypes() const noexcept { return {}; }
        // LCOV_EXCL_STOP
	};
}
