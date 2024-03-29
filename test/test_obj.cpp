#include "test_include_catch.hpp"
#include <typecheck/type.hpp>

TEST_CASE("Check raw type copy constructor", "[raw_type]") {
	typecheck::RawType t;
	t.set_name("Hello World");
	CHECK(t.name() == "Hello World");
	typecheck::RawType g{t};
	CHECK(g.name() == "Hello World");
}

TEST_CASE("Check raw type equals", "[raw_type]") {
	typecheck::RawType t;
	t.set_name("Hello World");
	CHECK(t.name() == "Hello World");
	typecheck::RawType g{t};
	CHECK(g.name() == "Hello World");
	CHECK(g == t);
	g.set_name("Not Hello");
	CHECK(g != t);
}

TEST_CASE("Check add args", "[function definition]") {
	typecheck::FunctionDefinition f;
	CHECK(f.args_size() == 0);
	f.add_args();
	CHECK(f.args_size() == 1);
}

TEST_CASE("Check create return type", "[function definition]") {
	typecheck::FunctionDefinition f;
	CHECK(!f.has_returntype());
	f.returntype();
	CHECK(f.has_returntype());
}

TEST_CASE("Check mutate return type", "[function definition]") {
	typecheck::FunctionDefinition f;
	CHECK(!f.has_returntype());
	f.mutable_returntype()->mutable_raw()->set_name("Hello World");
	CHECK(f.returntype().has_raw());
	CHECK(f.returntype().raw().name() == "Hello World");
}

TEST_CASE("Check Copy From", "[function definition]") {
	typecheck::FunctionDefinition f;

	CHECK(f.args_size() == 0);
	f.add_args();
	CHECK(f.args_size() == 1);

	CHECK(!f.has_returntype());
	f.mutable_returntype()->mutable_raw()->set_name("Hello World");
	CHECK(f.returntype().has_raw());
	CHECK(f.returntype().raw().name() == "Hello World");

	typecheck::FunctionDefinition g;
	g.CopyFrom(f);
	CHECK(g.args_size() == 1);
	CHECK(g.has_returntype());
	CHECK(g.returntype().has_raw());
	CHECK(g.returntype().raw().name() == "Hello World");
}

TEST_CASE("Check Equals", "[function definition]") {
	typecheck::FunctionDefinition f;
	f.set_id(123);
	f.add_args();
	f.mutable_returntype()->mutable_raw()->set_name("Hello World");

	typecheck::FunctionDefinition g;
	g.set_id(123);
	g.add_args();
	g.mutable_returntype()->mutable_raw()->set_name("Hello World");
	CHECK(f == g);

	g.add_args();
	CHECK(f != g);
}

TEST_CASE("Check set string", "[type_var]") {
	typecheck::TypeVar t;
	t.set_symbol("Hello World");
	CHECK(t.symbol() == "Hello World");
}

TEST_CASE("Copy var", "[type_var]") {
	typecheck::TypeVar t;
	t.set_symbol("Hello World");
	typecheck::TypeVar g;
	g.CopyFrom(t);
	CHECK(g.symbol() == t.symbol());
	CHECK(g == t);
}

TEST_CASE("Check has func", "[type]") {
	typecheck::Type t;
	CHECK(!t.has_func());

	// Should create func
	t.func();
	CHECK(t.has_func());
}

TEST_CASE("Check has raw", "[type]") {
	typecheck::Type t;
	CHECK(!t.has_raw());

	t.raw();
	CHECK(t.has_raw());
}

TEST_CASE("Check has mutable func", "[type]") {
	typecheck::Type t;
	CHECK(!t.has_func());

	// Should create func
	t.mutable_func();
	CHECK(t.has_func());
}

TEST_CASE("Check has mutable_raw", "[type]") {
	typecheck::Type t;
	CHECK(!t.has_raw());

	t.mutable_raw();
	CHECK(t.has_raw());
}

TEST_CASE("Mutable Raw Holds", "[type]") {
	typecheck::Type t;
	t.mutable_raw()->set_name("Hello world");
	CHECK(t.raw().name() == "Hello world");
}

TEST_CASE("Mutable Func Holds", "[type]") {
	typecheck::Type t;
	t.mutable_func()->mutable_returntype()->mutable_raw()->set_name("Hello world");
	CHECK(t.has_func());
	CHECK(t.func().has_returntype());
	CHECK(t.func().returntype().has_raw());
	CHECK(t.func().returntype().raw().name() == "Hello world");
}

TEST_CASE("Copy Type Constructor", "[type]") {
	typecheck::Type t;
	t.mutable_func()->mutable_returntype()->mutable_raw()->set_name("Hello world");
	CHECK(t.has_func());
	CHECK(t.func().has_returntype());
	CHECK(t.func().returntype().has_raw());
	CHECK(t.func().returntype().raw().name() == "Hello world");

	typecheck::Type g{t};
	CHECK(g.has_func());
	CHECK(g.func().has_returntype());
	CHECK(g.func().returntype().has_raw());
	CHECK(g.func().returntype().raw().name() == "Hello world");
}
