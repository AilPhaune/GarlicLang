#pragma once
#include <memory>
#include <sstream>
#include "Nodes.h"

class Symbol;
class SymbolBranch;
class SymbolTable;
class SymbolResult;
class SymbolAnalyzer;

enum SymbolType {
	ROOT, SCOPE, CLASS, ENUM, INTERFACE, ANNOTATION, FIELD, METHOD, FUNCTION, VARIABLE, OPERATOR_OVERLOAD, CONSTANT
};

class Symbol {
	public:
		std::string name;
		size_t id;
		SymbolType type;
		std::shared_ptr<SymbolTable> table;
		std::shared_ptr<SymbolBranch> branch;
		std::shared_ptr<SymbolBranch> innerBranch;

		Symbol(std::string name, size_t id, SymbolType type, std::shared_ptr<SymbolBranch> branch, std::shared_ptr<SymbolBranch> innerBranch, std::shared_ptr<SymbolTable> table);
		~Symbol();
};

class SymbolBranch {
	public:
		std::vector<std::shared_ptr<Symbol>> symbols, scopes, types, fields, constants, methods, functions, operatorOverloads;
		std::shared_ptr<Symbol> parentSymbol;
		std::shared_ptr<SymbolBranch> parentBranch;
		std::shared_ptr<SymbolTable> table;

		SymbolBranch(std::shared_ptr<Symbol> parentSymbol, std::shared_ptr<SymbolBranch> parentBranch, std::shared_ptr<SymbolTable> table);
		SymbolBranch(std::shared_ptr<Symbol> parentSymbol, std::shared_ptr<SymbolBranch> parentBranch, std::shared_ptr<SymbolTable> table, std::vector<std::shared_ptr<Symbol>> symbols, std::vector<std::shared_ptr<Symbol>> scopes, std::vector<std::shared_ptr<Symbol>> types, std::vector<std::shared_ptr<Symbol>> fields, std::vector<std::shared_ptr<Symbol>> constants, std::vector<std::shared_ptr<Symbol>> methods, std::vector<std::shared_ptr<Symbol>> functions, std::vector<std::shared_ptr<Symbol>> operatorOverloads);
		~SymbolBranch();
};

class SymbolTable : std::enable_shared_from_this<SymbolTable> {
	public:
		std::shared_ptr<SymbolBranch> root;
		size_t counter;

		SymbolTable();
		~SymbolTable();
};

class SymbolResult : public std::enable_shared_from_this<SymbolResult> {
	public:
		std::shared_ptr<GarlicError> error;
		std::vector<std::shared_ptr<GarlicError>> warnings;

		SymbolResult();
		~SymbolResult();

		bool reg(std::shared_ptr<SymbolResult> res);
		std::shared_ptr<SymbolResult> failure(std::shared_ptr<GarlicError> err);
		std::shared_ptr<SymbolResult> warn(std::shared_ptr<GarlicError> warning);

		static std::shared_ptr<SymbolResult> create();
};

class SymbolAnalyzer {
	private:
		std::vector<std::shared_ptr<GNode>> m_Queue;
	public:
		std::shared_ptr<SymbolTable> table;

		SymbolAnalyzer();
		~SymbolAnalyzer();

		std::shared_ptr<SymbolResult> analyze(std::shared_ptr<GNode> ast);
		void queue(std::shared_ptr<GNode> ast);
		std::shared_ptr<Symbol> define(const char* path, std::string name, SymbolType type, std::shared_ptr<SymbolBranch> innerBranch);
		std::shared_ptr<SymbolResult> findSymbols();
	private:
		std::shared_ptr<SymbolResult> analyzeDeclarations(std::shared_ptr<GNode> ast, std::shared_ptr<SymbolBranch> parent);
		std::shared_ptr<SymbolResult> analyzeScopes(std::shared_ptr<GNode> ast, std::shared_ptr<SymbolBranch> parent);
		std::shared_ptr<SymbolResult> analyzeReferences(std::shared_ptr<GNode> ast, std::shared_ptr<SymbolBranch> parent);
};