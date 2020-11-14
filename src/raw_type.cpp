#include <typecheck/type.hpp>
#include <utility>

using namespace typecheck;

RawType::RawType(std::string n) : _name(std::move(n)) {}

auto RawType::operator==(const RawType& other) const noexcept -> bool {
	return this->name() == other.name();
}

auto RawType::name() const -> std::string {
	return this->_name;
}

void RawType::set_name(const std::string& name) {
	this->_name = name;
}
