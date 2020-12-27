#pragma once

#include <string>

namespace typecheck {
	class TypeVar {
	public:
		TypeVar(std::string s = "");
		~TypeVar() = default;

		bool operator==(const TypeVar& other) const noexcept;
		bool operator!=(const TypeVar& other) const noexcept;
		bool operator<(const TypeVar& other) const noexcept;

		void CopyFrom(const TypeVar& other);

		std::string symbol() const;
		void set_symbol(const std::string& s);

		std::string ShortDebugString() const;
	private:
		std::string _symbol;
	};
}
