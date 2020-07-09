#pragma once
#include <typecheck_protos/type.pb.h>

/*
namespace typecheck {
	class Type {
	private:
		std::string name;
	public:
		Type(const std::string& _name) : name(_name) {}
		~Type() {}

		Type(const Type& other) {
			this->name = other.name;
		}
		
		Type& operator=(const Type& other) {
			this->name = other.name;
			return *this;
		}

		std::string getName() const noexcept {
			return this->name;
		}

		bool operator==(const Type& other) const noexcept {
			return this->name == other.name;
		}

		bool operator!=(const Type& other) const noexcept {
			return !this->operator==(other);
		}
	};
}
*/