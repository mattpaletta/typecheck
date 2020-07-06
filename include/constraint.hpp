#pragma once
#include <string>
#include <vector>

enum TypeSymbol { Integer = 0, Float, Double, Boolean };

// Inspired by: github.com/swift/lib/Sema/Constraint.h
enum ConstraintKind { Bind = 0, Equal, BindParam, Conversion, ConformsTo };
enum ConstraintRestrictionKind { DeepEquality = 0, Superclass, ArrayToPointer, StringToPointer, ValueToOptional };

struct Constraint {
	ConstraintKind kind;
	ConstraintRestrictionKind restriction;
	// ConstraintFix* theFix;
	bool hasRestriction;
	bool isActive;
	bool isDisabled;
	bool isFavoured;

	std::string symbol_name;

	union {
		struct {
			TypeSymbol first;
			TypeSymbol second;
			TypeSymbol third;
		} Types;

		struct {
			TypeSymbol first;
			TypeSymbol second;
			union {
				// std::string declName;
				std::string* ref;
				// DeclContext;
			} Member;
		} Member;

		Constraint* nested[];
		struct {
			TypeSymbol type;
			int OverloadChoice;
			// DeclContext;
		} Overload;
	};
};
