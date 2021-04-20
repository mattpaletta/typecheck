#pragma once

#include "type_var.hpp"

#include <string>
#include <vector>

namespace typecheck {
	class FunctionVar {
	public:
		FunctionVar();
		~FunctionVar() = default;

		long long id() const;
		void set_id(const long long id);

		TypeVar* mutable_returnvar();
		const TypeVar& returnvar() const;

		const std::vector<TypeVar>& args() const;
		TypeVar* add_args();

        std::string serialize() const;
        static FunctionVar unserialize(const std::string& str);

        std::string name() const;

	private:
		std::vector<TypeVar> _args;
		TypeVar _returnVar;
		std::string _name;
		long long _id;
	};
}
