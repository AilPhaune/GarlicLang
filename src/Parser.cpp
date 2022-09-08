#include <sstream>
#include "Parser.h"
#include <iostream>

#define NOT_IMPLEMENTED res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::NOT_IMPLEMENTED, "Not implemented !", this->token->pos)))

GParsingResult::GParsingResult() : error(nullptr), node(nullptr) {}
GParsingResult::~GParsingResult() {
	this->error = nullptr;
	this->node = nullptr;
}
std::shared_ptr<GParsingResult> GParsingResult::create() {
	return std::shared_ptr<GParsingResult>(new GParsingResult());
}
std::shared_ptr<GNode> GParsingResult::reg(std::shared_ptr<GParsingResult> res) {
	if (this->error == nullptr) {
		this->error = res->error;
	}
	return res->node;
}
std::shared_ptr<GParsingResult> GParsingResult::success(std::shared_ptr<GNode> node) {
	this->node = node;
	return shared_from_this();
}
std::shared_ptr<GParsingResult> GParsingResult::failure(std::shared_ptr<GarlicError> err) {
	this->error = err;
	return shared_from_this();
}

GParser::GParser(std::vector<std::shared_ptr<GToken>> tokens): tokens(tokens), history(std::stack<int>()) {
	this->refresh();
}
GParser::~GParser() {
	this->tokens.clear();
}
void GParser::refresh() {
	this->token = this->tokens.at(this->pos);
}
void GParser::advance(int i) {
	this->pos += i;
	this->refresh();
}
void GParser::push() {
	this->history.push(this->pos);
}
void GParser::pop() {
	this->pos = this->history.top();
	this->history.pop();
	this->refresh();
}
std::string GParser::genUnexpected(std::shared_ptr<GToken> token) {
	std::stringstream ret;
	std::string tokval = token->value;
	if (tokval.length()) {
		tokval = tokval.substr(0, 17) + "...";
	}
	ret << "Unexpected token '" << tokval << "'";
	return ret.str();
}
std::shared_ptr<GParsingResult> GParser::parseAST() {
	return this->makeStatement();
}
std::shared_ptr<GParsingResult> GParser::makeStatements() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	if (this->token->type == GTokenType::LBRACE) {
		std::shared_ptr<GToken> start = this->token;
		this->advance();
		if (this->token->type == GTokenType::RBRACE) {
			std::shared_ptr<GPosition> pos = GPosition::endsAt(start->pos, this->token->pos);
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GNodeList(std::vector<std::shared_ptr<GNode>>(), pos)));
		}
		std::shared_ptr<GNode> statement = res->reg(this->makeStatement());
		if (res->error != nullptr) {
			return res;
		}
		if (this->token->type != GTokenType::RBRACE) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_STATEMENT_MISSING_RBRACE, "Expected '}' to terminate statement, got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
		}
		this->advance();
		return res->success(statement);
	}
	std::shared_ptr<GNode> node = res->reg(this->makeStatementBase());
	if (res->error != nullptr) {
		return res;
	}
	return res->success(node);
}
std::shared_ptr<GParsingResult> GParser::makeStatement() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GToken> start = this->token;
	while (this->token->type == GTokenType::SEMICOLON) {
		this->advance();
	}
	if (this->token->type == GTokenType::RBRACE) {
		return res->success(std::shared_ptr<GNode>(new GNodeList(std::vector<std::shared_ptr<GNode>>(), GPosition::endsAt(start->pos, this->token->pos))));
	}
	std::vector<std::shared_ptr<GNode>> nodes;
	std::shared_ptr<GNode> node = res->reg(this->makeStatementBase());
	if (res->error != nullptr) {
		return res;
	}
	nodes.push_back(node);
	if (this->token->type != GTokenType::SEMICOLON && node->needsSemicolon()) {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UNEXPECTED_TOKEN, "Unexpected token '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	std::shared_ptr<GPosition> pos = node->pos;
	while (this->token->type == GTokenType::SEMICOLON || !node->needsSemicolon()) {
		if (this->token->type == GTokenType::SEMICOLON) {
			this->advance();
		}
		pos = this->token->pos;
		if (this->token->type == GTokenType::END_OF_FILE) {
			break;
		}
		if (this->token->type == GTokenType::SEMICOLON) {
			continue;
		}
		if (this->token->type == GTokenType::RBRACE) {
			break;
		}
		node = res->reg(this->makeStatementBase());
		if (res->error != nullptr) {
			return res;
		}
		nodes.push_back(node);
		if (!node->needsSemicolon()) {
			continue;
		}
		if (this->token->type != GTokenType::SEMICOLON) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UNEXPECTED_TOKEN, "Unexpected token '" + GToken::safeValue(this->token) + "'", this->token->pos)));
		}
	}
	return res->success(std::shared_ptr<GNodeList>(new GNodeList(nodes, GPosition::endsAt(start->pos, pos))));
}
std::shared_ptr<GParsingResult> GParser::makeStatementBase() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	if (this->token->type == GTokenType::NON_GARLIC) {
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		return res->success(std::shared_ptr<GNode>(new GNonGarlicNode(tok->value, tok->pos)));
	}
	if (this->token->type == GTokenType::COMM_AT) {
		// TODO
		return NOT_IMPLEMENTED;
	}
	if (this->token->type == GTokenType::KEYWORD) {
		if (this->token->value == "if") {
			return this->makeIfStatement();
		}
		if (this->token->value == "for") {
			return this->makeForLoop();
		}
		if (this->token->value == "while") {
			return this->makeWhileLoop();
		}
		if (this->token->value == "do") {
			return this->makeDoWhileLoop();
		}
		std::vector<std::string> declarationKeywords = { "def", "let", "const", "public", "private", "protected", "static", "class", "interface", "enum", "annotation" };
		if (std::find(declarationKeywords.begin(), declarationKeywords.end(), this->token->value) != declarationKeywords.end()) {
			return this->makeDeclaration();
		}
	}
	if (this->token->type == GTokenType::LBRACE) {
		std::shared_ptr<GNode> node = res->reg(this->makeStatement());
		if (res->error != nullptr) {
			return res;
		}
		return res->success(node);
	}
	std::shared_ptr<GNode> node = res->reg(this->makeComplexExpression());
	if (res->error != nullptr) {
		return res;
	}
	return res->success(node);
}
std::shared_ptr<GParsingResult> GParser::makeComplexExpression() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	this->push();
	/*
	std::shared_ptr<GNode> maybeScope = res->reg(this->makeScope());
	if (maybeScope->getType() == GNodeType::NODE_SCOPE && res->error == nullptr) {
		// TODO scope
	}
	*/
	this->pop();
	std::shared_ptr<GNode> node = res->reg(this->makeBooleanExpression());
	if (res->error != nullptr) {
		return res;
	}
	return res->success(node);
}
std::shared_ptr<GParsingResult> GParser::makeBooleanExpression() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GNode> left = res->reg(this->makeComparisonExpression());
	if (res->error != nullptr) {
		return res;
	}
	std::vector<std::pair<GTokenType, uint8_t>> map = {
		{ GTokenType::PIPE2, GOperator::BOOL_OR },
		{ GTokenType::AMPERSAND2, GOperator::BOOL_AND }
	};
	std::vector<std::pair<GTokenType, uint8_t>>::iterator it = map.begin();
	while (it != map.end()) {
		if (this->token->type != it->first) {
			it++;
			continue;
		}
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		std::shared_ptr<GNode> right = res->reg(this->makeComparisonExpression());
		if (res->error != nullptr) {
			return res;
		}
		left = std::shared_ptr<GNode>(new GBinOpNode(left, right, it->second, GPosition::endsAt(left->pos, right->pos)));
		it++;
	}
	return res->success(left);
}
std::shared_ptr<GParsingResult> GParser::makeComparisonExpression() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GNode> left = res->reg(this->makeSimpleExpression());
	if (res->error != nullptr) {
		return res;
	}
	std::vector<std::pair<GTokenType, uint8_t>> map = {
		{ GTokenType::LT, GOperator::LT },
		{ GTokenType::LT_EQ, GOperator::LTE },
		{ GTokenType::GT, GOperator::GT },
		{ GTokenType::GT_EQ, GOperator::GTE },
		{ GTokenType::EQ2, GOperator::EQ },
		{ GTokenType::EXCLAMATION_EQ, GOperator::NEQ }
	};
	std::vector<std::pair<GTokenType, uint8_t>>::iterator it = map.begin();
	while (it != map.end()) {
		if (this->token->type != it->first) {
			it++;
			continue;
		}
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		std::shared_ptr<GNode> right = res->reg(this->makeSimpleExpression());
		if (res->error != nullptr) {
			return res;
		}
		left = std::shared_ptr<GNode>(new GCompOpNode(left, right, it->second, GPosition::endsAt(left->pos, right->pos)));
		it++;
	}
	return res->success(left);
}
std::shared_ptr<GParsingResult> GParser::makeSimpleExpression() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GNode> left = res->reg(this->makeTerm());
	if (res->error != nullptr) {
		return res;
	}
	std::vector<std::pair<GTokenType, uint8_t>> map = {
		{ GTokenType::PLUS, GOperator::ADD },
		{ GTokenType::MINUS, GOperator::SUBTRACT },
		{ GTokenType::LT2, GOperator::LSHIFT },
		{ GTokenType::GT2, GOperator::RSHIFT },
		{ GTokenType::LT3, GOperator::LSHIFT_ROT },
		{ GTokenType::GT3, GOperator::RSHIFT_ROT }
	};
	std::vector<std::pair<GTokenType, uint8_t>>::iterator it = map.begin();
	while (it != map.end()) {
		if (this->token->type != it->first) {
			it++;
			continue;
		}
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		std::shared_ptr<GNode> right = res->reg(this->makeTerm());
		if (res->error != nullptr) {
			return res;
		}
		left = std::shared_ptr<GNode>(new GBinOpNode(left, right, it->second, GPosition::endsAt(left->pos, right->pos)));
		it++;
	}
	return res->success(left);
}
std::shared_ptr<GParsingResult> GParser::makeTerm() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GNode> left = res->reg(this->makeFactor());
	if (res->error != nullptr) {
		return res;
	}
	std::vector<std::pair<GTokenType, uint8_t>> map = {
		{ GTokenType::STAR, GOperator::MULTIPLY },
		{ GTokenType::SLASH, GOperator::DIVIDE },
		{ GTokenType::AMPERSAND, GOperator::AND },
		{ GTokenType::PIPE, GOperator::OR },
		{ GTokenType::CARET, GOperator::XOR },
		{ GTokenType::PERCENT, GOperator::MODULO }
	};
	std::vector<std::pair<GTokenType, uint8_t>>::iterator it = map.begin();
	while (it != map.end()) {
		if (this->token->type != it->first) {
			it++;
			continue;
		}
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		std::shared_ptr<GNode> right = res->reg(this->makeFactor());
		if (res->error != nullptr) {
			return res;
		}
		left = std::shared_ptr<GNode>(new GBinOpNode(left, right, it->second, GPosition::endsAt(left->pos, right->pos)));
		it++;
	}
	return res->success(left);
}
std::shared_ptr<GParsingResult> GParser::makeFactor() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::vector<std::pair<GTokenType, uint8_t>> map = {
		{ GTokenType::EXCLAMATION, GOperator::BOOL_NOT },
		{ GTokenType::TILDA, GOperator::BITWISE_NOT },
		{ GTokenType::PLUS, GOperator::UNARY_PLUS },
		{ GTokenType::MINUS, GOperator::UNARY_MINUS },
		{ GTokenType::STAR, GOperator::UNARY_STAR }
	};
	std::vector<std::pair<GTokenType, uint8_t>>::iterator it = map.begin();
	while (it != map.end()) {
		if (this->token->type != it->first) {
			it++;
			continue;
		}
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		std::shared_ptr<GNode> node = res->reg(this->makePower());
		if (res->error != nullptr) {
			return res;
		}
		return res->success(std::shared_ptr<GNode>(new GUnaryOpNode(node, it->second, GPosition::endsAt(tok->pos, node->pos))));
	}
	std::shared_ptr<GNode> node = res->reg(this->makePower());
	if (res->error != nullptr) {
		return res;
	}
	return res->success(node);
}
std::shared_ptr<GParsingResult> GParser::makePower() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GNode> left = res->reg(this->makeCall());
	if (res->error != nullptr) {
		return res;
	}
	while (this->token->type == GTokenType::STAR2) {
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		std::shared_ptr<GNode> right = res->reg(this->makeFactor());
		if (res->error != nullptr) {
			return res;
		}
		left = std::shared_ptr<GNode>(new GBinOpNode(left, right, GOperator::EXPONENTIATION, GPosition::endsAt(left->pos, right->pos)));
	}
	return res->success(left);
}
std::shared_ptr<GParsingResult> GParser::makeCall() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GNode> atom = res->reg(this->makeAtom());
	if (res->error != nullptr) {
		return res;
	}
	if (this->token->type == GTokenType::QUESTION) {
		std::shared_ptr<GNode> ternary = res->reg(this->makeTernary(atom));
		if (res->error != nullptr) {
			return res;
		}
		return res->success(ternary);
	}
	while (true) {
		if (this->token->type == GTokenType::LPAREN) {
			atom = res->reg(this->makeParenthesisOperator(atom));
		}
		else if (this->token->type == GTokenType::LBRACK) {
			atom = res->reg(this->makeBracketOperator(atom));
		}
		else if (this->token->type == GTokenType::DOT) {
			atom = res->reg(this->makeDotAccessor(atom));
		}
		else {
			break;
		}
		if (res->error != nullptr) {
			return res;
		}
	}
	return res->success(atom);
}
std::shared_ptr<GParsingResult> GParser::makeAtom() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GPosition> tokPos = this->token->pos;

	// Generate GValueNode for token of type int8
	if (this->token->type == GTokenType::LITTERAL_INT8) {
		try {
			int64_t longval = std::stoll(this->token->value);
			if (longval > INT8_MAX || longval < INT8_MIN) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT8_OUT_OF_RANGE, "Byte/int8 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new int8_t(static_cast<int8_t>(longval)));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::INT8, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_INT8, "Invalid byte/int8", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT8_OUT_OF_RANGE, "Byte/int8 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type int16
	if (this->token->type == GTokenType::LITTERAL_INT16) {
		try {
			int64_t longval = std::stoll(this->token->value);
			if (longval > INT16_MAX || longval < INT16_MIN) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT16_OUT_OF_RANGE, "Short/int16 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new int16_t(static_cast<int16_t>(longval)));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::INT16, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_INT16, "Invalid short/int16", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT16_OUT_OF_RANGE, "Short/int16 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type int32
	if (this->token->type == GTokenType::LITTERAL_INT32) {
		try {
			int64_t longval = std::stoll(this->token->value);
			if (longval > INT32_MAX || longval < INT32_MIN) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT32_OUT_OF_RANGE, "Integer/int32 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new int32_t(static_cast<int32_t>(longval)));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::INT32, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_INT32, "Invalid int32", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT32_OUT_OF_RANGE, "Integer/int32 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type int64
	if (this->token->type == GTokenType::LITTERAL_INT64) {
		try {
			int64_t longval = std::stoll(this->token->value);
			if (longval > INT64_MAX || longval < INT64_MIN) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT64_OUT_OF_RANGE, "Long/int64 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new int64_t(longval));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::INT64, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_INT64, "Invalid long/int64", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INT64_OUT_OF_RANGE, "Long/int64 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type uint8
	if (this->token->type == GTokenType::LITTERAL_UINT8) {
		try {
			uint64_t longval = std::stoull(this->token->value);
			if (longval > UINT8_MAX) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT8_OUT_OF_RANGE, "Uint8 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new uint8_t(static_cast<uint8_t>(longval)));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::UINT8, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_UINT8, "Invalid uint8", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT8_OUT_OF_RANGE, "Uint8 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type uint16
	if (this->token->type == GTokenType::LITTERAL_UINT16) {
		try {
			uint64_t longval = std::stoull(this->token->value);
			if (longval > UINT16_MAX) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT16_OUT_OF_RANGE, "Uint16 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new uint16_t(static_cast<uint16_t>(longval)));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::UINT16, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_UINT16, "Invalid uint16", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT16_OUT_OF_RANGE, "Uint16 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type uint32
	if (this->token->type == GTokenType::LITTERAL_UINT32) {
		try {
			uint64_t longval = std::stoull(this->token->value);
			if (longval > UINT32_MAX) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT32_OUT_OF_RANGE, "Uint32 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new uint32_t(static_cast<uint32_t>(longval)));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::UINT32, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_UINT32, "Invalid uint32", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT32_OUT_OF_RANGE, "Uint32 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type uint64
	if (this->token->type == GTokenType::LITTERAL_UINT64) {
		try {
			uint64_t longval = std::stoull(this->token->value);
			if (longval > UINT64_MAX) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT64_OUT_OF_RANGE, "Uint64 value out of range", this->token->pos)));
			}
			std::shared_ptr<void> value = std::shared_ptr<void>(new uint8_t(longval));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::UINT64, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_UINT64, "Invalid uint64", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UINT64_OUT_OF_RANGE, "Uint64 value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type float
	if (this->token->type == GTokenType::LITTERAL_FLOAT) {
		try {
			float val = std::stof(this->token->value);
			std::shared_ptr<void> value = std::shared_ptr<void>(new float_t(val));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::FLOAT, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_FLOAT, "Invalid float", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_FLOAT_OUT_OF_RANGE, "Float value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type double
	if (this->token->type == GTokenType::LITTERAL_DOUBLE) {
		try {
			double val = std::stod(this->token->value);
			std::shared_ptr<void> value = std::shared_ptr<void>(new double_t(val));
			this->advance();
			return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::DOUBLE, tokPos)));
		}
		catch (const std::invalid_argument) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_INVALID_DOUBLE, "Invalid double", this->token->pos)));
		}
		catch (const std::out_of_range) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_DOUBLE_OUT_OF_RANGE, "Double value out of range", this->token->pos)));
		}
	}

	// Generate GValueNode for token of type string
	if (this->token->type == GTokenType::D_QUOTE_STRING || this->token->type == GTokenType::S_QUOTE_STRING) {
		std::shared_ptr<void> value = std::shared_ptr<void>(new std::string(this->token->value));
		this->advance();
		return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::STRING, tokPos)));
	}

	// Generate GValueNode for token of type char
	if (this->token->type == GTokenType::LITTERAL_CHAR) {
		std::shared_ptr<void> value = std::shared_ptr<void>(new char(this->token->value.at(0)));
		this->advance();
		return res->success(std::shared_ptr<GNode>(new GValueNode(value, GValueType::CHAR, tokPos)));
	}

	// Generate GValueNode for token of type identifier
	if (this->token->type == GTokenType::IDENTIFIER) {
		std::string str = this->token->value;
		this->advance();
		return res->success(std::shared_ptr<GNode>(new GIdentifierNode(str, tokPos)));
	}

	if (this->token->type == GTokenType::LPAREN) {
		this->advance();
		std::pair<std::vector<std::shared_ptr<GNode>>, std::shared_ptr<GarlicError>> nodes = this->makeCommaSeparatedValues();
		if (nodes.second != nullptr) {
			return res->failure(nodes.second);
		}
		if (this->token->type != GTokenType::RPAREN) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_TUPLE_VALUE_EXPECTED_RPAREN, "Expected ')' or ', value' but got'" + GToken::safeValue(this->token) + "'", this->token->pos)));
		}
		std::shared_ptr<GToken> rparen = this->token;
		this->advance();
		if (nodes.first.size() == 1) {
			return res->success(nodes.first.at(0));
		}
		return res->success(std::shared_ptr<GNode>(new GTupleNode(nodes.first, GPosition::endsAt(tokPos, rparen->pos))));
	}

	return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UNEXPECTED_TOKEN, "Unexpected token '" + GToken::safeValue(this->token) + "'", this->token->pos)));
}
std::pair<std::vector<std::shared_ptr<GNode>>, std::shared_ptr<GarlicError>> GParser::makeCommaSeparatedValues() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::vector<std::shared_ptr<GNode>> nodes;
	std::shared_ptr<GNode> node = res->reg(this->makeComplexExpression());
	if (res->error != nullptr) {
		return std::pair<std::vector<std::shared_ptr<GNode>>, std::shared_ptr<GarlicError>>(std::vector<std::shared_ptr<GNode>>(), res->error);
	}
	nodes.push_back(node);
	while (this->token->type == GTokenType::COMMA) {
		this->advance();
		node = res->reg(this->makeComplexExpression());
		if (res->error != nullptr) {
			return std::pair<std::vector<std::shared_ptr<GNode>>, std::shared_ptr<GarlicError>>(std::vector<std::shared_ptr<GNode>>(), res->error);
		}
		nodes.push_back(node);
	}
	return std::pair<std::vector<std::shared_ptr<GNode>>, std::shared_ptr<GarlicError>>(nodes, nullptr);
}
std::shared_ptr<GParsingResult> GParser::makeTernary(std::shared_ptr<GNode> atom) {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	this->advance();
	std::shared_ptr<GNode> whenTrue = res->reg(this->makeComplexExpression());
	if (res->error != nullptr) {
		return res;
	}
	if (this->token->type != GTokenType::COLON) {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_TERNARY_EXPECT_COLON, "Expected ': expression' to complete ternary expression, got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	this->advance();
	std::shared_ptr<GNode> whenFalse = res->reg(this->makeComplexExpression());
	if (res->error != nullptr) {
		return res;
	}
	return res->success(std::shared_ptr<GNode>(new GTernaryNode(atom, whenTrue, whenFalse, GPosition::endsAt(atom->pos, whenFalse->pos))));
}
std::shared_ptr<GParsingResult> GParser::makeParenthesisOperator(std::shared_ptr<GNode> atom) {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GToken> lparen = this->token;
	this->advance();
	if (this->token->type == GTokenType::RPAREN) {
		std::shared_ptr<GToken> rparen = this->token;
		this->advance();
		return res->success(std::shared_ptr<GNode>(new GParenthesisOperatorNode(atom, std::vector<std::shared_ptr<GNode>>(), GPosition::endsAt(atom->pos, rparen->pos))));
	}
	std::pair<std::vector<std::shared_ptr<GNode>>, std::shared_ptr<GarlicError>> params = this->makeCommaSeparatedValues();
	if (params.second != nullptr) {
		return res->failure(params.second);
	}
	if (this->token->type != GTokenType::RPAREN) {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_PARENTHESIS_OPERATOR_CALL_EXPECT_RPAREN, "Expected ')', got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	std::shared_ptr<GToken> rparen = this->token;
	this->advance();
	return res->success(std::shared_ptr<GNode>(new GParenthesisOperatorNode(atom, params.first, GPosition::endsAt(atom->pos, rparen->pos))));
}
std::shared_ptr<GParsingResult> GParser::makeBracketOperator(std::shared_ptr<GNode> atom) {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::shared_ptr<GToken> lbarack = this->token;
	this->advance();
	std::shared_ptr<GNode> accessor = res->reg(this->makeComplexExpression());
	if (res->error != nullptr) {
		return res;
	}
	if (this->token->type != GTokenType::RBRACK) {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_PARENTHESIS_OPERATOR_CALL_EXPECT_RPAREN, "Expected ']', got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	std::shared_ptr<GToken> rbrack = this->token;
	this->advance();
	std::shared_ptr<GBracketOperatorNode> ret = std::shared_ptr<GBracketOperatorNode>(new GBracketOperatorNode(atom, accessor, GPosition::endsAt(atom->pos, rbrack->pos)));
	std::vector<GTokenType> setter_toks = {EQ, PLUS_EQ, MINUS_EQ, PERCENT_EQ, SLASH_EQ, STAR_EQ, CARET_EQ, PIPE_EQ, AMPERSAND_EQ, LT2_EQ, GT2_EQ, LT3_EQ, GT3_EQ};
	if (std::find(setter_toks.begin(), setter_toks.end(), this->token->type) != setter_toks.end()) {
		std::shared_ptr<GNode> node = res->reg(this->makeSetter(ret));
		if (res->error != nullptr) {
			return res;
		}
		return res->success(node);
	}
	return res->success(ret);
}
std::shared_ptr<GParsingResult> GParser::makeDotAccessor(std::shared_ptr<GNode> atom) {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	this->advance();
	if (this->token->type != GTokenType::IDENTIFIER) {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_DOT_ACCESSOR_MISSING_IDENTIFIER, "Expected identifier, got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	std::string accessor = this->token->value;
	std::shared_ptr<GPosition> pos = this->token->pos;
	this->advance();
	std::shared_ptr<GDotAccessorNode> ret = std::shared_ptr<GDotAccessorNode>(new GDotAccessorNode(atom, accessor, GPosition::endsAt(atom->pos, pos)));
	std::vector<GTokenType> setter_toks = { EQ, PLUS_EQ, MINUS_EQ, PERCENT_EQ, SLASH_EQ, STAR_EQ, CARET_EQ, PIPE_EQ, AMPERSAND_EQ, LT2_EQ, GT2_EQ, LT3_EQ, GT3_EQ };
	if (std::find(setter_toks.begin(), setter_toks.end(), this->token->type) != setter_toks.end()) {
		std::shared_ptr<GNode> node = res->reg(this->makeSetter(ret));
		if (res->error != nullptr) {
			return res;
		}
		return res->success(node);
	}
	return res->success(ret);
}
std::shared_ptr<GParsingResult> GParser::makeSetter(std::shared_ptr<GNode> node) {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	std::vector<std::pair<GTokenType, uint8_t>> map = {
		{ GTokenType::EQ, GOperator::SET_EQ },
		{ GTokenType::PLUS_EQ, GOperator::PLUS_EQ },
		{ GTokenType::MINUS_EQ, GOperator::MINUS_EQ },
		{ GTokenType::STAR_EQ, GOperator::TIMES_EQ },
		{ GTokenType::SLASH_EQ, GOperator::DIVIDED_EQ },
		{ GTokenType::PERCENT_EQ, GOperator::MODULO_EQ },
		{ GTokenType::AMPERSAND_EQ , GOperator::AND_EQ },
		{ GTokenType::PIPE_EQ, GOperator::OR_EQ },
		{ GTokenType::CARET_EQ, GOperator::XOR_EQ },
		{ GTokenType::LT2, GOperator::LSHIFT_EQ },
		{ GTokenType::GT2, GOperator::RSHIFT_EQ },
		{ GTokenType::LT3, GOperator::LSHIFT_ROT_EQ },
		{ GTokenType::GT3, GOperator::RSHIFT_ROT_EQ }
	};
	std::vector<std::pair<GTokenType, uint8_t>>::iterator it = map.begin();
	while(it != map.end()) {
		if (it->first != this->token->type) {
			it++;
			continue;
		}
		std::shared_ptr<GToken> tok = this->token;
		this->advance();
		std::shared_ptr<GNode> value = res->reg(this->makeComplexExpression());
		if (res->error != nullptr) {
			return res;
		}
		std::shared_ptr<GPosition> pos = GPosition::endsAt(node->pos, value->pos);
		return res->success(std::shared_ptr<GNode>(new GSetterNode(node, value, it->second, pos)));
	}
	return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UNEXPECTED_ASSIGNMENT_TOKEN, "Expected an assignment token ('=', '+=', '-=', ...), gor'" + GToken::safeValue(this->token) + "'", this->token->pos)));
}
std::shared_ptr<GParsingResult> GParser::makeIfStatement() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	if (this->token->value != "if") {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UNEXPECTED_TOKEN, "Expected 'if(condition) expr [else if(cond) expr]... [else expr]' but got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	std::shared_ptr<GPosition> startPos = this->token->pos;
	this->advance();
	if (this->token->type != GTokenType::LPAREN) {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UNEXPECTED_TOKEN, "Expected '(' but got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	this->advance();
	std::shared_ptr<GNode> condition = res->reg(this->makeComplexExpression());
	if (res->error != nullptr) {
		return res;
	}
	if (this->token->type != GTokenType::RPAREN) {
		return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::PARSER_UNEXPECTED_TOKEN, "Expected ')' but got '" + GToken::safeValue(this->token) + "'", this->token->pos)));
	}
	this->advance();
	std::shared_ptr<GNode> conditionTrue = res->reg(this->makeStatements());
	if (res->error != nullptr) {
		return res;
	}
	if (this->token->type != KEYWORD || this->token->value != "else") {
		return res->success(std::shared_ptr<GNode>(new GIfStatementNode(condition, conditionTrue, nullptr, GPosition::endsAt(startPos, conditionTrue->pos))));
	}
	this->advance();
	std::shared_ptr<GNode> conditionFalse = res->reg(this->makeStatements());
	if (res->error != nullptr) {
		return res;
	}
	return res->success(std::shared_ptr<GNode>(new GIfStatementNode(condition, conditionTrue, conditionFalse, GPosition::endsAt(startPos, conditionFalse->pos))));
}
std::shared_ptr<GParsingResult> GParser::makeForLoop() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	return NOT_IMPLEMENTED;
}
std::shared_ptr<GParsingResult> GParser::makeWhileLoop() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	return NOT_IMPLEMENTED;
}
std::shared_ptr<GParsingResult> GParser::makeDoWhileLoop() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	return NOT_IMPLEMENTED;
}
std::shared_ptr<GParsingResult> GParser::makeDeclaration() {
	std::shared_ptr<GParsingResult> res = GParsingResult::create();
	return NOT_IMPLEMENTED;
}