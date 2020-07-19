# Getting Started

To include this library in your project, link to `typecheck` in CMake, and include the following file:
```cpp
#include <typecheck/type_manager.hpp>
```
Everything in this library is in the `::typecheck` namespace.

The 'main' object you interact with is the `TypeManager`.  It has a default constructor and destructor, so you can create one with:
```cpp
typecheck::TypeManager tm;
```

The TypeManager has a few concepts I want to cover to get you started.
- Registered Types
- Type Variables
- Constraints
- Resolvers
- Protocols
- Resolved Types

## Registered Types
Because this library is language agnostic, it does not assume any types in the host-language.  For this reason, types must be 'registered' before use.  They can be registered using: `tm.registerType("<type_name>")`

Once the type has been registered, the language-implementer can then define which types are convertible to each other:
```cpp
tm.registerType("int");
tm.registerType("float");
tm.registerType("double");
tm.setConvertible("int", "float");
tm.setConvertible("int", "double");
tm.setConvertible("float", "float");
tm.setConvertible("float", "double");
```
* Note: this section is most-likely to change, see `Protocols`.

## Type Variables
Type variables to used as a symbol representing a final type.  Some examples of type variables are: `T0`, `T2`, `T4`, etc.
You can create a new type symbol using:
```cpp
tm.CreateTypeVar();
```
This will return the created variable (type: `TypeSymbol`, defined in `protos/type.proto`).  Although not enforced, type variables should not be changed after being returned.  The return type is not `const` to allow late-binding.

## Constraints
Constraints are where most of the logic takes place.  Constraints are constructed between 1 or more type variables, and define a set of rules a final output of *resolved types* must meet to be considered valid.  The types of conversions supported currently include:
- Equal
- Conversion
- ConformsTo
- ApplicableFunction
- BindOverload

The Constraint type is defined in `protos/constraint.proto`.

The Type Manager provides helper-methods to create constraints of various types and has a number of checks to make sure constraints are created properly.
```cpp
ConstraintPass::IDType CreateLiteralConformsToConstraint(const TypeVar& t0, const KnownProtocolKind_LiteralProtocol& protocol);
ConstraintPass::IDType CreateEqualsConstraint(const TypeVar& t0, const TypeVar& t1);
ConstraintPass::IDType CreateConvertibleConstraint(const TypeVar& T0, const TypeVar& T1);
ConstraintPass::IDType CreateApplicableFunctionConstraint(const TypeVar& T0, const std::vector<Type>& args, const Type& return_type);
ConstraintPass::IDType CreateApplicableFunctionConstraint(const TypeVar& T0, const Type& type);
ConstraintPass::IDType CreateBindFunctionConstraint(const TypeVar& T0, const std::vector<TypeVar>& args, const TypeVar& returnType);
ConstraintPass::IDType CreateBindToConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type);
```

At the time of creation, all constraints are created independently and are allocated interally to the Type Manager.
### Example
```
T0 = (T1, T2) -> T3
Overloads:
  - (int, float) -> double
```
We use the `ApplicableFunction` to define all defined overloads of a given function, and `BindFunction` to select a particular overload.  It was decided to use a type variable to define a function instead of the function name to allow for support for lambda functions in end-applications.  The `ApplicableFunction` and `BindFunction` point to the same function, because of their TypeVar being equal.  If you want to use the same `ApplicableFunction` definition multiple times with different type variables.  You have to setup the definitions multiple types and create a bind constraint for each caller.
```cpp
typecheck::TypeManager tm = CreateAndSetupTypeManager(/* Custom function for your application */);
const auto T0 = tm.CreateTypeVar();
const auto T1 = tm.CreateTypeVar();
const auto T2 = tm.CreateTypeVar();
const auto T3 = tm.CreateTypeVar();

tm.CreateApplicableFunctionConstraint(T0, {tm.getRegisteredType("int"), tm.getRegisteredType("float")}, tm.getRegisteredType("double"));
tm.CreateBindFunctionConstraint(T0, {T1, T2}, T3);
```

```cpp
T1 = Int Literal
T2 = Float Literal
T1 == T2
```
Here we have an integer literal and a float literal, and we are told they must be the same type.  We can setup these constraints as shown below.
```cpp
typecheck::TypeManager tm = CreateAndSetupTypeManager(/* Custom function for your application */);
const auto T1 = tm.CreateTypeVar();
const auto T2 = tm.CreateTypeVar();

tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByInteger);
tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::KnownProtocolKind_LiteralProtocol_ExpressibleByFloat);
tm.CreateEqualsConstraint(T1, T2);
```

Constraints can be created in any order, however all of the type variables used in a given constraint must be created before creating the constraint.  Once we have created all of our constraints, we can solve the system of constraints:
```cpp
// ...
const bool did_solve = tm.solve()
```
This returns a boolean indicating if we could a solution.  More information about why a program failed to typecheck is on the roadmap but not implemented at this time.

Assuming we did find a solution, we can get the final resolved types for each variable.  See `Resolved Types`.

## Resolvers
Resolvers are a type of class defined in TypeCheck used to resolve a particular type of constraint.  These were implemented as abstract classes to allow for more extendability.

To create a new resolver:
```cpp
#include <typecheck/resolver.hpp>
```

In your application, you can then create a class that extends `typecheck::Resolver` with the following methods:
```cpp
class ResolveCustomConstraint : public typecheck::Resolver {
    public:
        ResolveCustomConstraint(typecheck::ConstraintPass* pass, const typecheck::ConstraintPass::IDType id);

        virtual std::unique_ptr<typecheck::Resolver> clone(typecheck::ConstraintPass* pass, const typecheck::ConstraintPass::IDType id) const override;

        virtual bool hasMoreSolutions(const typecheck::Constraint& constraint, const typecheck::TypeManager* manager) override;
        virtual bool resolveNext(const typecheck::Constraint& constraint, const typecheck::TypeManager* manager) override;
        virtual std::size_t score(const typecheck::Constraint& constraint, const typecheck::TypeManager* manager) const override;
};
```

The constructor should call the `typecheck::Resolver` Constructor.  Here is a sample constructor for one of the built-in resolvers.
```cpp
ResolveApplicableFunction::ResolveApplicableFunction(typecheck::ConstraintPass* _pass, const typecheck::ConstraintPass::IDType _id) : typecheck::Resolver(typecheck::ConstraintKind::ApplicableFunction, _pass, _id) {}
```
The first parameter for the resolver constructor defines what kind of constraint this resolver is responsible for.  Only one resolver of each kind should be registered.  If no custom resolvers are defined, the library has default resolvers it will use.  The user can replace the built-in resolvers using:
```cpp
tm.registerResolver(std::make_unique<ResolveCustomConstraint>(nullptr, 0));
```

The clone method is pretty simple, but needed to do a polymorphic copy.  Here is a sample implementation:
```cpp
std::unique_ptr<typecheck::Resolver> ResolveApplicableFunction::clone(typecheck::ConstraintPass* _pass, const typecheck::ConstraintPass::IDType _id) const {
    return std::make_unique<ResolveApplicableFunction>(_pass, _id);
}
```
The clone method, along with the constructor should create a new resolver of the given type with **no state** passed from the original object to the new object.

The `hasMoreSolutions` should setup any internal state from a given constraint in the resolver.  For example, this might include creating the list of options to iterate through.  This function should return true while there are more solutions to try, and return false otherwise.  For some of my resolvers, I only have 1 solution to try, so I have a flag I set to `false` in the constructor, and to `true` in `resolveNext`.  My `hasMoreSolutions` therefore returns if that flag is set to `false`.

The `resolveNext` function is where the majority of the logic should reside.  It will only be falled if `hasMoreSolutions` returned `true`.  The class is allowed to maintain internal state between these two functions as the same object will be used for each, however multiple objects of a given type will be created within an application.  Within the function, you can query the `TypeManager` for information or to see the current state of types, you can use `this->pass` (type: `typecheck::ConstraintPass`, defined in: `<typecheck/constraint_pass.hpp>`).

Some useful methods on a `ConstraintPass`:
```cpp
pass->HasPermission(const typecheck::Constraint&, const typecheck::TypeVar&, TypeManager*);

pass->setResolvedType(const typecheck::TypeVar&, const typecheck::Type&);
pass->hasResolvedType(const typecheck::TypeVar&);
pass->getResolvedType(const typecheck::TypeVar&);
```

A resolver should only change a resolved type with `setResolvedType` if it has permission.  This ensures that constraints don't overwrite each other during execution.

The final required method is `score`.  This is used to determine a ranking of the possible solutions.  With a given state, (checked with `pass->getResolvedType` and `pass->hasResolvedType`), if the resolver deems this state to be optimal, it should return 0.  If it is an invalid solution, it should return: `std::numeric_limits<std::size_t>::max()`.  If it is acceptable, but not optimal, it should return a small number greater than 0.  Some of the interal resolvers return 1 in this case as all non-optimal solutions are considered equal.

## Protocols
TODO: This is prone to change soon, so I will wait to write this section.  In the meantime, if you have questions, please submit an issue and I will try to explain it in the current implementation.

## Resolved Types
After calling `tm.solve()`, assuming it returned true, you can then ask the `TypeManager` for the final types for each of the variables.
```cpp
// ...
const auto T1 = tm.CreateTypeVar();
// ...
tm.solve();
const typecheck::Type typeT1 = tm.getResolvedType(T1);
// ...
```

This returns (type: typecheck::Type, defined in `protos/type.proto`.  There are two types of `Type`: `raw` and `func`.  A raw type is a basic type: `float`, `int`, `bool`, etc.  A function type is just a function.  It has arguments, an optional name, and a return type.  The arguments and return types are all `Type`.  This allows for support for functions that take in or return functions as parameters.  I'd imagine the function type will be made up of raw-types for the arguments and return type for most applications.  It is up to the language implementer to decide what *kind* of types they wish to use.

You can check if a given type is raw or func:
```cpp
typeT1.has_raw();
typeT1.has_func();
```
If it's a raw type, you can get it's name:
```cpp
typeT1.raw().name();
```

The function type is a `FunctionDefinition`, also defined in `type.proto`.
Some methods available:
```cpp
typeT1.func().args_size(); // returns long long
typeT1.func().args(i); // returns Type
typeT1.func().args(); // returns iterable<Type>
typeT1.func().name(); // returns optional function name
typeT1.func().returntype(); // returns Type
```
