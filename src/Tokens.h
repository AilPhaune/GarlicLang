#pragma once
#include <string>
#include <map>
#include <vector>
#include <regex>
#include "Utils.h"

enum GTokenType;
struct GToken;
struct GTokenResult;
struct GLexingResult;
class GLexer;

enum GTokenType {
	NON_GARLIC, EQ, PLUS, MINUS, STAR, SLASH, COMMA, SEMICOLON, COLON, QUESTION, EXCLAMATION, PERCENT, LPAREN, RPAREN, LBRACK,
	RBRACK, LBRACE, RBRACE, COMM_AT, TILDA, AMPERSAND, PIPE, CARET, DOT, LT, GT, LT2, GT2, STAR2, EQ2, PIPE2, AMPERSAND2,
	PLUS2, MINUS2, PLUS_EQ, MINUS_EQ, STAR_EQ, SLASH_EQ, PERCENT_EQ, CARET_EQ, PIPE_EQ, LT_EQ, GT_EQ, EXCLAMATION_EQ, AMPERSAND_EQ,
	LT3, GT3, LT2_EQ, GT2_EQ, LT3_EQ, GT3_EQ, IDENTIFIER, KEYWORD, S_QUOTE_STRING, D_QUOTE_STRING, LITTERAL_CHAR, LITTERAL_BOOLEAN,
	LITTERAL_INT8, LITTERAL_INT16, LITTERAL_INT32, LITTERAL_INT64, LITTERAL_UINT8, LITTERAL_UINT16, LITTERAL_UINT32, LITTERAL_UINT64,
	LITTERAL_BIGINT, LITTERAL_FLOAT, LITTERAL_DOUBLE, END_OF_FILE
};

struct GToken {
	public:
		static std::string safeValue(std::shared_ptr<GToken> token);

		GTokenType type;
		std::string value;
		std::shared_ptr<GPosition> pos;

		GToken(GTokenType type, std::string value, std::shared_ptr<GPosition> pos);
		~GToken();
};

struct GTokenResult : public std::enable_shared_from_this<GTokenResult> {
	private:
		std::shared_ptr<GToken> result;
		std::shared_ptr<GarlicError> error;
	public:
		GTokenResult();
		~GTokenResult();

		std::shared_ptr<GTokenResult> failure(std::shared_ptr<GarlicError> error);
		std::shared_ptr<GTokenResult> success(std::shared_ptr<GToken> result);
		bool hasError();
		bool hasToken();
		std::shared_ptr<GarlicError> getError();
		std::shared_ptr<GToken> getToken();
};

struct GLexingResult : public std::enable_shared_from_this<GLexingResult> {
	private:
		std::shared_ptr<std::vector<GTokenResult>> tokens;
		std::shared_ptr<GarlicError> error;
	public:
		GLexingResult();
		~GLexingResult();

		std::shared_ptr<GLexingResult> failure(std::shared_ptr<GarlicError> error);
		std::shared_ptr<GLexingResult> push(std::shared_ptr<GTokenResult> token);
		bool hasError();
		std::shared_ptr<GarlicError> getError();
		std::shared_ptr<std::vector<GTokenResult>> getTokens();
};

class GLexer {
	public:
		static std::vector<std::pair<std::string, GTokenType>> DEFAULT_TOKEN_MAP;
		static std::string NUMBERS, IDENTIFIER_START, IDENTIFIERS;
		static std::vector<std::string> KEYWORDS;

		std::string file;
		std::string content;
		unsigned int idx, line, linePos;

		GLexer(std::string file, std::string content);
		~GLexer();
	
		void advance();
		void advance(bool skipNewLines);
		void advance(int num);
		void advance(int num, bool skipNewLines);

		bool eof();
		char currChar();
		std::shared_ptr<GPosition> pos();

		void removeSpaces();
		bool matchExpr(std::string expr);
		bool matchRegex(std::string expr);
		bool matchRegex(std::regex expr);
		std::shared_ptr<GTokenResult> makeNumber();
		std::shared_ptr<GTokenResult> makeString(char startChar);
		std::shared_ptr<GTokenResult> makeIdentifierKeyword();
		std::shared_ptr<GTokenResult> nextToken();
	
		std::shared_ptr<GLexingResult> makeTokens();
};