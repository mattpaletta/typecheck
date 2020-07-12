#include "typecheck/protocols/ExpressibleByDoubleLiteral.hpp"

std::vector<typecheck::Type> typecheck::ExpressibleByDoubleLiteral::getPreferredTypes() const {
	return { ty("double") };
}

std::vector<typecheck::Type> typecheck::ExpressibleByDoubleLiteral::getOtherTypes() const {
	return {};
}


#include "typecheck/protocols/ExpressibleByFloatLiteral.hpp"

std::vector<typecheck::Type> typecheck::ExpressibleByFloatLiteral::getPreferredTypes() const {
	return { ty("float") };
}

std::vector<typecheck::Type> typecheck::ExpressibleByFloatLiteral::getOtherTypes() const {
	return { ty("double") };
}

#include "typecheck/protocols/ExpressibleByIntegerLiteral.hpp"

std::vector<typecheck::Type> typecheck::ExpressibleByIntegerLiteral::getPreferredTypes() const {
	return { ty("int") };
}

std::vector<typecheck::Type> typecheck::ExpressibleByIntegerLiteral::getOtherTypes() const {
	return { ty("double"), ty("float") };
}
