#pragma once

#include "type.hpp"
#include "type_var.hpp"
#include "known_protocol_kind.hpp"

#include <optional>

namespace typecheck {
	enum ConstraintKind {
		Bind = 0,
		Equal,
		BindParam,
		Conversion,
		ConformsTo,
		ApplicableFunction,
		BindOverload,
	};

	enum ConstraintRestrictionKind {
		DeepEquality = 0,
		Superclass,
		ArrayToPointer,
		StringToPointer,
		ValueToOptional,
	};


	// Taken from https://github.com/apple/swift/lib/Sema/Constraint.h
	enum ConstraintClassification {
		/// A relational constraint, which relates two types.
		Relational = 0,

		/// A member constraint, which names a member of a type and assigns it a reference type.
		Member,

		/// A property of a single type, such as whether it is defaultable to a particular type.
		TypeProperty,

		 /// A disjunction constraint.
		Disjunction,
	};

	class Constraint {
	public:
        using IDType = long long;

		class Types {
		public:
			Types();
			Types(const Types& other);
			Types& operator=(const Types& other);
			Types(Types&& other) noexcept;
			Types& operator=(Types&& other) noexcept;

			bool has_first() const;
			const TypeVar& first() const;
			TypeVar* mutable_first();

			bool has_second() const;
			const TypeVar& second() const;
			TypeVar* mutable_second();

			bool has_third() const;
			const TypeVar& third() const;
			TypeVar* mutable_third();

			std::string ShortDebugString() const;

		private:
			mutable std::optional<TypeVar> _first;
			mutable std::optional<TypeVar> _second;
			mutable std::optional<TypeVar> _third;
		};

		class ExplicitType {
		public:
			ExplicitType();
			ExplicitType(const ExplicitType& other);
			ExplicitType(ExplicitType&& other) noexcept;
			ExplicitType& operator=(ExplicitType&& other) noexcept;

			bool has_type() const;
			Type* mutable_type();
			const Type& type() const;

			bool has_var() const;
			TypeVar* mutable_var();
			const TypeVar& var() const;

			std::string ShortDebugString() const;

		private:
			mutable std::optional<TypeVar> _var;
			mutable std::optional<Type> _type;
		};

		class Member {
		public:
			const TypeVar& first() const;
			const TypeVar& second() const;

			std::string ShortDebugString() const;

		private:
			TypeVar _first;
			TypeVar _second;

			// Pointer to the declared context
			// std::int64_t declContextPtr;
		};

		class Overload {
		public:
			Overload();
			Overload(const Overload& other);
			Overload& operator=(const Overload& other);
			Overload(Overload&& other) noexcept;
			Overload& operator=(Overload&& other) noexcept;

			long long functionid() const;
			void set_functionid(const long long functionid);

			std::size_t argvars_size() const;
			const TypeVar& argvars(std::size_t i) const;
			TypeVar* add_argvars();

			bool has_type() const;
			const TypeVar& type() const;
			TypeVar* mutable_type();

			const TypeVar& returnvar() const;
			TypeVar* mutable_returnvar();

			std::string ShortDebugString() const;

		private:
			mutable std::optional<TypeVar> _type;
			long long _functionID{};
			std::vector<TypeVar> _argVars;
			TypeVar _returnVar;

			// Pointer to the declared context
			//std::int64_t declContextPtr;
		};

		class Conforms {
		public:
			Conforms();
			Conforms(const Conforms& other);
			Conforms& operator=(const Conforms& other);
			Conforms(Conforms&& other) noexcept;
			Conforms& operator=(Conforms&& other) noexcept;

			bool has_protocol() const;
			const KnownProtocolKind& protocol() const;
			KnownProtocolKind* mutable_protocol();

			bool has_type() const;
			const TypeVar& type() const;
			TypeVar* mutable_type();
			std::string ShortDebugString() const;
		private:
			mutable std::optional<KnownProtocolKind> _protocol;
			mutable std::optional<TypeVar> _type;
		};

		Constraint();
		bool operator==(const Constraint& other) const;

		IDType id() const;
		void set_id(const IDType id);

		const ConstraintKind& kind() const;
		void set_kind(const ConstraintKind& kind);

		bool has_explicit_() const;
		ExplicitType* mutable_explicit_();
		const ExplicitType& explicit_() const;

		bool has_overload() const;
		Overload* mutable_overload();
		const Overload& overload() const;

		bool has_conforms() const;
		Conforms* mutable_conforms();
		const Conforms& conforms() const;

		bool has_types() const;
		Types* mutable_types();
		const Types& types() const;

		std::string ShortDebugString() const;
	private:
		ConstraintKind _kind;
		/*
		ConstraintRestrictionKind restriction;

		bool hasRestriction;
		bool isActive;
		bool isDisabled;
		bool isFavoured;
		*/
		long long _id;

		// ID's of nested constraints
		std::vector<std::int64_t> nested;

		mutable std::variant<bool, Types, Member, Overload, Conforms, ExplicitType> data;
	};
}
