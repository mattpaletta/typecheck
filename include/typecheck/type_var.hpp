#pragma once

#include <string>

namespace typecheck {
	class TypeVar {
	public:
		TypeVar(std::string s = "");
		~TypeVar() = default;

		void CopyFrom(const TypeVar& other);

		std::string symbol() const;
		void set_symbol(const std::string& s);

	private:
		std::string _symbol;
	};
}
