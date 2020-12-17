#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>

namespace typecheck {
	class Type;

	class FunctionDefinition {
	public:
		FunctionDefinition();
		~FunctionDefinition() = default;

		FunctionDefinition(const FunctionDefinition& other);
		FunctionDefinition& operator=(const FunctionDefinition& other);

		bool operator==(const FunctionDefinition& other) const noexcept;
		bool operator!=(const FunctionDefinition& other) const noexcept;

		void CopyFrom(const FunctionDefinition& other);

		int args_size() const;
		const Type& args(const int i) const;
		Type* add_args();

		const Type& returntype() const;
		Type* mutable_returntype();
		bool has_returntype() const;

		std::string name() const;
		int id() const;

		std::string ShortDebugString() const;
	private:
		std::vector<Type> _args;

		// This one is a ptr so we can forward-declare it
		mutable std::unique_ptr<Type> _returnType;

		std::string _name;
		int _id{};
	};
}
