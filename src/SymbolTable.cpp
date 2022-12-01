#include <memory>
#include <sstream>
#include <iostream>
#include "../SymbolTable.h"

Symbol::Symbol(std::string name, size_t id, SymbolType type, std::shared_ptr<SymbolBranch> branch, std::shared_ptr<SymbolBranch> innerBranch, std::shared_ptr<SymbolTable> table):
	name(name), id(id), type(type), branch(branch), innerBranch(innerBranch), table(table) {}
Symbol::~Symbol() {
	this->branch = nullptr;
	this->innerBranch = nullptr;
}

SymbolBranch::SymbolBranch(std::shared_ptr<Symbol> parentSymbol, std::shared_ptr<SymbolBranch> parentBranch, std::shared_ptr<SymbolTable> table): SymbolBranch(parentSymbol, parentBranch, table, {}, {}, {}, {}, {}, {}, {}, {}) {}
SymbolBranch::SymbolBranch(std::shared_ptr<Symbol> parentSymbol, std::shared_ptr<SymbolBranch> parentBranch, std::shared_ptr<SymbolTable> table, std::vector<std::shared_ptr<Symbol>> symbols, std::vector<std::shared_ptr<Symbol>> scopes, std::vector<std::shared_ptr<Symbol>> types, std::vector<std::shared_ptr<Symbol>> fields, std::vector<std::shared_ptr<Symbol>> constants, std::vector<std::shared_ptr<Symbol>> methods, std::vector<std::shared_ptr<Symbol>> functions, std::vector<std::shared_ptr<Symbol>> operatorOverloads) :
	parentSymbol(parentSymbol), parentBranch(parentBranch), table(table), symbols(symbols), scopes(scopes), types(types), fields(fields), constants(constants), methods(methods), functions(functions), operatorOverloads(operatorOverloads) {}
SymbolBranch::~SymbolBranch() {
	this->symbols.clear();
	this->scopes.clear();
	this->types.clear();
	this->fields.clear();
	this->constants.clear();
	this->methods.clear();
	this->functions.clear();
	this->operatorOverloads.clear();
}

SymbolTable::SymbolTable(): counter(0) {
	this->root = std::shared_ptr<SymbolBranch>(new SymbolBranch(nullptr, nullptr, shared_from_this()));
}
SymbolTable::~SymbolTable() {
	this->root = nullptr;
}

SymbolResult::SymbolResult(): warnings(std::vector<std::shared_ptr<GarlicError>>()) {}
SymbolResult::~SymbolResult() {
	this->error = nullptr;
	this->warnings.clear();
}
bool SymbolResult::reg(std::shared_ptr<SymbolResult> res) {
	if (res == nullptr) {
		return this->error != nullptr;
	}
	if (this->error != nullptr) {
		std::vector<std::shared_ptr<GarlicError>>::iterator warn = res->warnings.begin();
		while (warn != res->warnings.end()) {
			this->warn(*warn);
			warn++;
		}
	}
	if (this->error == nullptr && res->error != nullptr) {
		this->error = res->error;
	}
	return this->error != nullptr;
}
std::shared_ptr<SymbolResult> SymbolResult::failure(std::shared_ptr<GarlicError> err) {
	this->error = this->error == nullptr ? err : this->error;
	return shared_from_this();
}
std::shared_ptr<SymbolResult> SymbolResult::warn(std::shared_ptr<GarlicError> warning) {
	size_t ptr = (size_t)warning.get();
	std::vector<std::shared_ptr<GarlicError>>::iterator finding = this->warnings.begin();
	while (finding != this->warnings.end()) {
		if (ptr == ((size_t)(*finding).get())) {
			return shared_from_this();
		}
		finding++;
	}
	this->warnings.push_back(warning);
	return shared_from_this();
}
std::shared_ptr<SymbolResult> SymbolResult::create() {
	return std::shared_ptr<SymbolResult>(new SymbolResult());
}

SymbolAnalyzer::SymbolAnalyzer(): table(std::shared_ptr<SymbolTable>(new SymbolTable())), m_Queue(std::vector<std::shared_ptr<GNode>>()) {}
SymbolAnalyzer::~SymbolAnalyzer() {
	this->table = nullptr;
}
std::shared_ptr<SymbolResult> SymbolAnalyzer::analyze(std::shared_ptr<GNode> ast) {
	std::shared_ptr<SymbolResult> res = SymbolResult::create();
	if (res->reg(this->analyzeDeclarations(ast, this->table->root))) {
		return res; // error
	}
	if (res->reg(this->analyzeScopes(ast, this->table->root))) {
		return res; // error
	}
	if (res->reg(this->analyzeReferences(ast, this->table->root))) {
		return res; // error
	}
	return res;
}

std::shared_ptr<SymbolResult> SymbolAnalyzer::analyzeDeclarations(std::shared_ptr<GNode> node, std::shared_ptr<SymbolBranch> parent) {
	std::shared_ptr<SymbolResult> res = SymbolResult::create();
	GNode* nde = node.get();
	if (nde->getType() == GNodeType::NODE_BIN_OP) {
		GBinOpNode* n = (GBinOpNode*) nde;

	}
	return res;
}

std::shared_ptr<SymbolResult> SymbolAnalyzer::analyzeScopes(std::shared_ptr<GNode> node, std::shared_ptr<SymbolBranch> parent) {
	std::shared_ptr<SymbolResult> res = SymbolResult::create();
	GNode* nde = node.get();
	if (nde->getType() == GNodeType::NODE_BIN_OP) {
		GBinOpNode* n = (GBinOpNode*) nde;

	}
	return res;
}

std::shared_ptr<SymbolResult> SymbolAnalyzer::analyzeReferences(std::shared_ptr<GNode> node, std::shared_ptr<SymbolBranch> parent) {
	std::shared_ptr<SymbolResult> res = SymbolResult::create();
	GNode* nde = node.get();
	if (nde->getType() == GNodeType::NODE_BIN_OP) {
		GBinOpNode* n = (GBinOpNode*) nde;

	}
	return res;
}
void SymbolAnalyzer::queue(std::shared_ptr<GNode> ast) {
	this->m_Queue.push_back(ast);
}
std::shared_ptr<Symbol> SymbolAnalyzer::define(const char* path, std::string name, SymbolType type, std::shared_ptr<SymbolBranch> innerBranch) {
	std::cout << "SymbolTable::define not implemented" << std::endl;
	return nullptr;
}
std::shared_ptr<SymbolResult> SymbolAnalyzer::findSymbols() {
	auto it = m_Queue.begin();
	std::shared_ptr<SymbolResult> res = std::shared_ptr<SymbolResult>(new SymbolResult());
	while(it != m_Queue.end()) {
		res->reg(this->analyzeDeclarations(*it, this->table->root));
		if(res->error) {
			return res;
		}
		it++;
	}
	it = m_Queue.begin();
	while(it != m_Queue.end()) {
		res->reg(this->analyzeScopes(*it, this->table->root));
		if(res->error) {
			return res;
		}
		it++;
	}
	it = m_Queue.begin();
	while(it != m_Queue.end()) {
		res->reg(this->analyzeReferences(*it, this->table->root));
		if(res->error) {
			return res;
		}
		it++;
	}
	return res;
}