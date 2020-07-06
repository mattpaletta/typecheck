#pragma once
#include <memory>
#include <string>
#include <vector>

#include "constraint.hpp"
#include "ortools/linear_solver/linear_solver.h"

struct Expr {
	TypeSymbol type;
	std::unique_ptr<Constraint> constraint;

	Expr() = default;
	~Expr() = default;
};

struct Integer final : public Expr {
	Integer() : Expr() {
		this->type = TypeSymbol::Integer;
	}
	~Integer() = default;
};

struct Float final : public Expr {
	Float() : Expr() {
		this->type = TypeSymbol::Float;
	}
	~Float() = default;
};

struct Double final : public Expr {
	Double() : Expr() {
		this->type = TypeSymbol::Double;
	}
	~Double() = default;
};

struct Boolean final : public Expr {
	Boolean() : Expr() {
		this->type = TypeSymbol::Boolean;
	}
	~Boolean() = default;
};


class TypeManager {
private:
	struct GenericTypeGenerator {
		std::size_t curr_num;
		std::string next();
	};

	std::vector<Constraint*> constraints;

	operations_research::MPSolver solver;

public:
	TypeManager();
	~TypeManager() = default;

	void add(Constraint* constraint);
	void solve();
	TypeSymbol getSymbol(const std::string& name) const;
};
