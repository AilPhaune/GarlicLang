#include "Tokens.h"
#include <iostream>

GToken::GToken(GTokenType type, std::string value, std::shared_ptr<GPosition> pos): type(type), value(value), pos(pos) {}
GToken::~GToken() {
	this->value.clear();
	this->pos = nullptr;
}
std::string GToken::safeValue(std::shared_ptr<GToken> token) {
	if (token->value.length() > 20) {
		return token->value.substr(0, 17) + "...";
	}
	return token->value;
}

GTokenResult::GTokenResult() : result(nullptr), error(nullptr) {};
GTokenResult::~GTokenResult() {
	this->error = nullptr;
	this->result = nullptr;
}
std::shared_ptr<GTokenResult> GTokenResult::failure(std::shared_ptr<GarlicError> error) {
	this->error = error;
	return shared_from_this();
}
std::shared_ptr<GTokenResult> GTokenResult::success(std::shared_ptr<GToken> token) {
	this->result = token;
	return shared_from_this();
}
bool GTokenResult::hasError() {
	return this->error != nullptr;
}
bool GTokenResult::hasToken() {
	return this->result != nullptr;
}
std::shared_ptr<GarlicError> GTokenResult::getError() {
	return this->error;
}
std::shared_ptr<GToken> GTokenResult::getToken() {
	return this->result;
}

GLexingResult::GLexingResult() : tokens(new std::vector<GTokenResult>()), error(nullptr) {};
GLexingResult::~GLexingResult() {
	this->error = nullptr;
	this->tokens = nullptr;
}
std::shared_ptr<GLexingResult> GLexingResult::failure(std::shared_ptr<GarlicError> error) {
	this->error = error;
	return shared_from_this();
}
std::shared_ptr<GLexingResult> GLexingResult::push(std::shared_ptr<GTokenResult> token) {
	this->tokens->push_back(*token);
	return shared_from_this();
}
bool GLexingResult::hasError() {
	return this->error != nullptr;
}
std::shared_ptr<GarlicError> GLexingResult::getError() {
	return this->error;
}
std::vector<std::shared_ptr<GToken>> GLexingResult::getTokens() {
	std::vector<std::shared_ptr<GToken>> res = std::vector<std::shared_ptr<GToken>>(this->tokens->size());
	for (uint64_t i = 0; i < res.size(); i++) {
		res[i] = this->tokens->at(i).getToken();
	}
	return res;
}

GLexer::GLexer(std::string file, std::string content): file(file), content(content) {
	this->idx = 0;
	this->line = 0;
	this->linePos = 0;
}
GLexer::~GLexer() {
	this->file.clear();
	this->content.clear();
}
std::vector<std::pair<std::string, GTokenType>> GLexer::DEFAULT_TOKEN_MAP = {
		{ "<<<=", GTokenType::LT3_EQ },
		{ ">>>=", GTokenType::GT3_EQ },
		{ "<<<", GTokenType::LT3 },
		{ ">>>", GTokenType::GT3 },
		{ "<<=", GTokenType::LT2_EQ },
		{ ">>=", GTokenType::GT2_EQ },
		{ "<<", GTokenType::LT2 },
		{ ">>", GTokenType::GT2 },
		{ "**", GTokenType::STAR2 },
		{ "==", GTokenType::EQ2 },
		{ "||", GTokenType::PIPE2 },
		{ "&&", GTokenType::AMPERSAND2 },
		{ "++", GTokenType::PLUS2 },
		{ "--", GTokenType::MINUS2 },
		{ "+=", GTokenType::PLUS_EQ },
		{ "-=", GTokenType::MINUS_EQ },
		{ "*=", GTokenType::STAR_EQ },
		{ "/=", GTokenType::SLASH_EQ },
		{ "%=", GTokenType::PERCENT_EQ },
		{ "^=", GTokenType::CARET_EQ },
		{ "|=", GTokenType::PIPE_EQ },
		{ "&=", GTokenType::AMPERSAND_EQ },
		{ "<=", GTokenType::LT_EQ },
		{ ">=", GTokenType::GT_EQ },
		{ "!=", GTokenType::EXCLAMATION_EQ },
		{ "=", GTokenType::EQ },
		{ "+", GTokenType::PLUS },
		{ "-", GTokenType::MINUS },
		{ "*", GTokenType::STAR },
		{ "/", GTokenType::SLASH },
		{ ",", GTokenType::COMMA },
		{ ";", GTokenType::SEMICOLON },
		{ ":", GTokenType::COLON },
		{ "?", GTokenType::QUESTION },
		{ "!", GTokenType::EXCLAMATION },
		{ "%", GTokenType::PERCENT },
		{ "(", GTokenType::LPAREN },
		{ ")", GTokenType::RPAREN },
		{ "[", GTokenType::LBRACK },
		{ "]", GTokenType::RBRACK },
		{ "{", GTokenType::LBRACE },
		{ "}", GTokenType::RBRACE },
		{ "@", GTokenType::COMM_AT },
		{ "~", GTokenType::TILDA },
		{ "&", GTokenType::AMPERSAND },
		{ "|", GTokenType::PIPE },
		{ "^", GTokenType::CARET },
		{ ".", GTokenType::DOT },
		{ "<", GTokenType::LT },
		{ ">", GTokenType::GT }
};
std::string GLexer::NUMBERS = "0123456789fFdDbBsSlLnN";
std::string GLexer::IDENTIFIER_START = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
std::string GLexer::IDENTIFIERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
std::vector<std::string> GLexer::KEYWORDS = {
	"int8", "uint8", "int16", "uint16", "int32", "uint32", "int64", "uint64", "float", "double", "bigint", "complex", "new",
	"for", "while", "do", "if", "else", "break", "continue", "return", "def", "let", "const", "public", "private", "protected", "static",
	"class", "interface", "enum", "annotation",	"write", "html", "void", "true", "false", "this", "char"
};

void GLexer::advance() {
	this->advance(1, true);
}
void GLexer::advance(bool skipNewLines) {
	this->advance(1, skipNewLines);
}
void GLexer::advance(int num) {
	this->advance(num, true);
}
void GLexer::advance(int num, bool skipNewLines) {
	if (this->eof()) {
		return;
	}
	if (skipNewLines) {
		for (int i = 0; i < num; i++) {
			this->idx++;
			this->linePos++;
			while (true) {
				if (this->currChar() == '\r') {
					this->idx++;
					continue;
				}
				if (this->currChar() == '\n') {
					this->idx++;
					this->line++;
					this->linePos = 0;
					continue;
				}
				break;
			}
		}
		return;
	}
	for (int i = 0; i < num; i++) {
		this->idx++;
		this->linePos++;
		if (this->currChar() == '\n') {
			this->line++;
			this->linePos = 0;
		}
	}
}
bool GLexer::eof() {
	return this->content.length() <= this->idx;
}
char GLexer::currChar() {
	if (this->eof()) {
		return 0;
	}
	return this->content.at(this->idx);
}
std::shared_ptr<GPosition> GLexer::pos() {
	return std::shared_ptr<GPosition>(new GPosition(this->file, this->idx, this->idx, this->line, this->line, this->linePos, this->linePos));
}
void GLexer::removeSpaces() {
	while (this->currChar() == ' ' || this->currChar() == '\t') {
		this->advance();
		if (this->eof()) {
			return;
		}
	}
}
bool GLexer::matchExpr(std::string expr) {
	return this->content.substr(this->idx).rfind(expr, 0) == 0;
}
bool GLexer::matchRegex(std::string expr) {
	return this->matchRegex(std::regex(expr));
}
bool GLexer::matchRegex(std::regex expr) {
	return std::regex_match(this->content.substr(this->idx), expr);
}
std::shared_ptr<GTokenResult> GLexer::makeNumber() {
	std::shared_ptr<GTokenResult> res = std::shared_ptr<GTokenResult>(new GTokenResult());
	std::string str = "";
	bool dot = false, isUnsigned = false;
	std::shared_ptr<GPosition> start = this->pos(), pos = this->pos();
	while (!this->eof() && GLexer::NUMBERS.find(this->currChar()) != std::string::npos) {
		char c = this->currChar();
		if (c == '.') {
			if (dot) {
				break;
			}
			dot = true;
		}
		pos = GPosition::endsAt(start, this->pos());
		if (c == 'f' || c == 'F') {
			if (isUnsigned) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_UNSIGNED_NOT_INTEGER, "Float can not be unsigned", this->pos())));
			}
			this->advance();
			return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::LITTERAL_FLOAT, str, pos)));
		}
		if (c == 'd' || c == 'D') {
			if (isUnsigned) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_UNSIGNED_NOT_INTEGER, "Float can not be unsigned", this->pos())));
			}
			this->advance();
			return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::LITTERAL_DOUBLE, str, pos)));
		}
		if (c == 'u') {
			isUnsigned = true;
			this->advance();
			continue;
		}
		if (c == 'b' || c == 'B') {
			if (dot) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_BYTE_LITTERAL_CONTAINS_DECIMAL_POINT, "Byte/int8 litterals can not be decimal numbers", pos)));
			}
			this->advance();
			return res->success(std::shared_ptr<GToken>(new GToken(isUnsigned ? GTokenType::LITTERAL_UINT8 : GTokenType::LITTERAL_INT8, str, pos)));
		}
		if (c == 's' || c == 'S') {
			if (dot) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_SHORT_LITTERAL_CONTAINS_DECIMAL_POINT, "Short/int16 litterals can not be decimal numbers", pos)));
			}
			this->advance();
			return res->success(std::shared_ptr<GToken>(new GToken(isUnsigned ? GTokenType::LITTERAL_UINT16 : GTokenType::LITTERAL_INT16, str, pos)));
		}
		if (c == 'l' || c == 'L') {
			if (dot) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_LONG_LITTERAL_CONTAINS_DECIMAL_POINT, "Long/int64 litterals can not be decimal numbers", pos)));
			}
			this->advance();
			return res->success(std::shared_ptr<GToken>(new GToken(isUnsigned ? GTokenType::LITTERAL_UINT64 : GTokenType::LITTERAL_INT64, str, pos)));
		}
		if (c == 'n' || c == 'N') {
			if (dot) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_BIGINT_LITTERAL_CONTAINS_DECIMAL_POINT, "BigInt litterals can not be decimal numbers", pos)));
			}
			if(isUnsigned) {
				return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_UNSIGNED_BIGINT, "BigInt can not be unsigned", pos)));
			}
			this->advance();
			return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::LITTERAL_BIGINT, str, pos)));
		}
		str += c;
		this->advance();
	}
	if (dot) {
		return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::LITTERAL_DOUBLE, str, pos)));
	}
	return res->success(std::shared_ptr<GToken>(new GToken(isUnsigned ? GTokenType::LITTERAL_UINT32 : GTokenType::LITTERAL_INT32, str, pos)));
}
std::shared_ptr<GTokenResult> GLexer::makeString(char startChar) {
	std::shared_ptr<GTokenResult> res = std::shared_ptr<GTokenResult>(new GTokenResult());
	std::shared_ptr<GPosition> start = this->pos(), pos = this->pos();
	std::string str = "";
	bool escape = false;
	while (true) {
		this->advance();
		if (start->startLine != this->line || this->eof()) {
			return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_NON_ENDING_STRING, "String must end on the same line it starts on", this->pos())));
		}
		pos = GPosition::endsAt(start, this->pos());
		char c = this->currChar();
		if (escape) {
			escape = false;
			str += c;
			continue;
		}
		if (c == '\\') {
			escape = true;
			continue;
		}
		if (c == startChar) {
			this->advance();
			pos = GPosition::endsAt(start, this->pos());
			break;
		}
		str += c;
	}
	if (startChar == '\'') {
		return res->success(std::shared_ptr<GToken>(new GToken(str.length() == 1 ? GTokenType::LITTERAL_CHAR : GTokenType::S_QUOTE_STRING, str, pos)));
	}
	return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::D_QUOTE_STRING, str, pos)));
}
std::shared_ptr<GTokenResult> GLexer::makeIdentifierKeyword() {
	std::string str = "";
	std::shared_ptr<GPosition> start = this->pos(), pos = this->pos();
	std::shared_ptr<GTokenResult> res = std::shared_ptr<GTokenResult>(new GTokenResult());
	while (!this->eof() && GLexer::IDENTIFIERS.find(this->currChar()) != std::string::npos) {
		str += this->currChar();
		pos = GPosition::endsAt(start, this->pos());
		this->advance();
	}
	if (std::find(GLexer::KEYWORDS.begin(), GLexer::KEYWORDS.end(), str) != GLexer::KEYWORDS.end()) {
		return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::KEYWORD, str, pos)));
	}
	return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::IDENTIFIER, str, pos)));
}
std::shared_ptr<GTokenResult> GLexer::nextToken() {
	std::shared_ptr<GTokenResult> res = std::shared_ptr<GTokenResult>(new GTokenResult());
	if (this->eof()) {
		nextTokenEOF:
		return res->success(std::shared_ptr<GToken>(new GToken(GTokenType::END_OF_FILE, "EOF", this->pos())));
	}
	this->removeSpaces();

	while (true) {
		if (this->matchExpr("//")) {
			int line = this->line;
			while (line == this->line) {
				this->advance();
				if (this->eof()) {
					goto nextTokenEOF;
				}
			}
		}
		else if (this->matchExpr("/*")) {
			while (!this->matchExpr("*/")) {
				this->advance();
				if (this->eof()) {
					goto nextTokenEOF;
				}
			}
		}
		else {
			break;
		}
	}

	this->removeSpaces();

	char c = this->currChar();
	char next = this->content[this->idx + 1];
	if ((c == '.' && next >= '0' && next <= '9') || (c >= '0' && c <= '9')) {
		return this->makeNumber();
	}

	std::vector<std::pair<std::string, GTokenType>>::iterator it = GLexer::DEFAULT_TOKEN_MAP.begin();
	while (it != GLexer::DEFAULT_TOKEN_MAP.end()) {
		std::string str = it->first;
		GTokenType ttype = it->second;
		if (this->matchExpr(str)) {
			std::shared_ptr<GPosition> pos = this->pos();
			this->advance((int) str.length());
			return res->success(std::shared_ptr<GToken>(new GToken(ttype, str, pos)));
		}
		it++;
	}

	if (GLexer::IDENTIFIER_START.find(c) != std::string::npos) {
		return this->makeIdentifierKeyword();
	}

	if (c == '"' || c == '\'') {
		return this->makeString(c);
	}

	std::string msg = "Invalid character '";
	msg += c;
	msg += "'";
	return res->failure(std::shared_ptr<GarlicError>(new GarlicError(GErrorCode::LEXER_INVALID_CHARACTER, msg, this->pos())));
}
std::shared_ptr<GLexingResult> GLexer::makeTokens() {
	std::shared_ptr<GLexingResult> res = std::shared_ptr<GLexingResult>(new GLexingResult());
	while (true) {
		if (this->eof()) {
			goto gotEOF;
		}
		std::string non_garlic = "";
		std::shared_ptr<GPosition> start = this->pos(), ng_pos = this->pos();
		while (!this->matchExpr("<?garlic") && !this->matchExpr("<?g=")) {
			non_garlic += this->currChar();
			ng_pos = GPosition::endsAt(start, ng_pos);
			this->advance();
			if(this->eof()) {
				if (non_garlic.length() > 0) {
					std::shared_ptr<GTokenResult> tokres = std::shared_ptr<GTokenResult>(new GTokenResult());
					res->push(tokres->success(std::shared_ptr<GToken>(new GToken(GTokenType::NON_GARLIC, non_garlic, ng_pos))));
				}
				goto gotEOF;
			}
		}
		if (non_garlic.length() > 0) {
			std::shared_ptr<GTokenResult> tokres = std::shared_ptr<GTokenResult>(new GTokenResult());
			res->push(tokres->success(std::shared_ptr<GToken>(new GToken(GTokenType::NON_GARLIC, non_garlic, ng_pos))));
		}
		if (this->matchExpr("<?garlic")) {
			this->advance(8);
		} else {
			start = this->pos();
			this->advance(4);
			ng_pos = GPosition::endsAt(start, this->pos());
			std::shared_ptr<GTokenResult> tokres = std::shared_ptr<GTokenResult>(new GTokenResult());
			res->push(tokres->success(std::shared_ptr<GToken>(new GToken(GTokenType::KEYWORD, "write", ng_pos))));
		}
		while (!this->matchExpr("?>")) {
			std::shared_ptr<GTokenResult> tok = this->nextToken();
			if (tok->hasError()) {
				return res->failure(tok->getError());
			}
			res->push(tok);
			if (tok->getToken()->type == GTokenType::END_OF_FILE || this->eof()) {
				goto gotEOF;
			}
		}
		if (this->matchExpr("?>")) {
			this->advance(2);
		}
	}
	gotEOF:
	std::shared_ptr<GTokenResult> tokres = std::shared_ptr<GTokenResult>(new GTokenResult());
	res->push(tokres->success(std::shared_ptr<GToken>(new GToken(GTokenType::END_OF_FILE, "EOF", this->pos()))));
	return res;
}