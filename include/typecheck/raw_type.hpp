#pragma once

#include <string>
namespace typecheck {
	class RawType {
	public:
		RawType(std::string  n = "");
		~RawType() = default;

		bool operator==(const RawType& other) const noexcept;

		std::string name() const;
		void set_name(const std::string& name);
	private:
		std::string _name;
	};
}
