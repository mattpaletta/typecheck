#include "typecheck/protocols/ExpressibleByDoubleLiteral.hpp"

auto typecheck::ExpressibleByDoubleLiteral::getPreferredTypes() const noexcept -> std::vector<typecheck::Type> {
	return { ty("double") };
}

auto typecheck::ExpressibleByDoubleLiteral::getOtherTypes() const noexcept -> std::vector<typecheck::Type>{
	return {};
}

#include "typecheck/protocols/ExpressibleByFloatLiteral.hpp"

auto typecheck::ExpressibleByFloatLiteral::getPreferredTypes() const noexcept -> std::vector<typecheck::Type> {
	return { ty("float") };
}

auto typecheck::ExpressibleByFloatLiteral::getOtherTypes() const noexcept -> std::vector<typecheck::Type> {
    return { ty("double") };
}

#include "typecheck/protocols/ExpressibleByIntegerLiteral.hpp"

auto typecheck::ExpressibleByIntegerLiteral::getPreferredTypes() const noexcept -> std::vector<typecheck::Type> {
	return { ty("int") };
}

auto typecheck::ExpressibleByIntegerLiteral::getOtherTypes() const noexcept -> std::vector<typecheck::Type> {
    return { ty("double"), ty("float") };
}
