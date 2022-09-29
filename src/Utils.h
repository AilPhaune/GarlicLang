#pragma once
#include <string>
#include <memory> // for shared_ptr 

enum GErrorCode;
class GOperator;
class GValueType;
struct GarlicError;
struct GPosition;
class Numbers;

enum GErrorCode {
	NOT_IMPLEMENTED = 1,
	LEXER_INVALID_CHARACTER,
	LEXER_BYTE_LITTERAL_CONTAINS_DECIMAL_POINT,
	LEXER_SHORT_LITTERAL_CONTAINS_DECIMAL_POINT,
	LEXER_LONG_LITTERAL_CONTAINS_DECIMAL_POINT,
	LEXER_BIGINT_LITTERAL_CONTAINS_DECIMAL_POINT,
	LEXER_UNSIGNED_NOT_INTEGER,
	LEXER_UNSIGNED_BIGINT,
	LEXER_NON_ENDING_STRING,
	PARSER_UNEXPECTED_TOKEN,
	PARSER_INT8_OUT_OF_RANGE,
	PARSER_INVALID_INT8,
	PARSER_INT16_OUT_OF_RANGE,
	PARSER_INVALID_INT16,
	PARSER_INT32_OUT_OF_RANGE,
	PARSER_INVALID_INT32,
	PARSER_INT64_OUT_OF_RANGE,
	PARSER_INVALID_INT64,
	PARSER_UINT8_OUT_OF_RANGE,
	PARSER_INVALID_UINT8,
	PARSER_UINT16_OUT_OF_RANGE,
	PARSER_INVALID_UINT16,
	PARSER_UINT32_OUT_OF_RANGE,
	PARSER_INVALID_UINT32,
	PARSER_UINT64_OUT_OF_RANGE,
	PARSER_INVALID_UINT64,
	PARSER_FLOAT_OUT_OF_RANGE,
	PARSER_INVALID_FLOAT,
	PARSER_DOUBLE_OUT_OF_RANGE,
	PARSER_INVALID_DOUBLE,
	PARSER_STATEMENT_MISSING_RBRACE,
	PARSER_TUPLE_VALUE_EXPECTED_RPAREN,
	PARSER_TERNARY_EXPECT_COLON,
	PARSER_PARENTHESIS_OPERATOR_CALL_EXPECT_RPAREN,
	PARSER_DOT_ACCESSOR_MISSING_IDENTIFIER,
	PARSER_UNEXPECTED_ASSIGNMENT_TOKEN,
	PARSER_EXPECTED_SCOPE,
	PARSER_EXPECTED_VARIABLE_OR_FIELD_NAME,
	PARSER_INCOMPLETE_VARIABLE_OR_FIELD_DECLARATION,
	PARSER_EXPECT_RBRACK_FOR_ARRAY_DECLARATION,
};

class GOperator {
	public:
		static uint8_t ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO, EXPONENTIATION, XOR, OR, AND, BOOL_OR, BOOL_AND, LSHIFT, RSHIFT,
						LSHIFT_ROT, RSHIFT_ROT, UNARY_PLUS, UNARY_MINUS, UNARY_STAR, BITWISE_NOT, BOOL_NOT, LT, LTE, GT, GTE, EQ, NEQ,
						PLUS_EQ, MINUS_EQ, TIMES_EQ, DIVIDED_EQ, MODULO_EQ, POWER_EQ, AND_EQ, OR_EQ, XOR_EQ, LSHIFT_EQ, RSHIFT_EQ,
						LSHIFT_ROT_EQ, RSHIFT_ROT_EQ, SET_EQ;
		static std::string toString(uint8_t op);
};

class GValueType {
	public:
		static uint8_t INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64, BIGINT, FLOAT, DOUBLE, STRING, CHAR, BOOLEAN;
};

struct GPosition {
	std::string file;
	int startIdx, endIdx, startLine, endLine, startLinePos, endLinePos;

	GPosition(std::string file, int startIdx, int endIdx, int startLine, int endLine, int startLinePos, int endLinePos);
	~GPosition();

	std::string toString();
	std::string noIdxString();
	std::string onlyStartString();

	static GPosition* endsAt(GPosition *start, GPosition *end);
	static GPosition endsAt(GPosition start, GPosition end);
	static std::shared_ptr<GPosition> endsAt(std::shared_ptr<GPosition> start, std::shared_ptr<GPosition> end);
};

struct GarlicError {
	public:
		GErrorCode code;
		std::string message;
		std::shared_ptr<GPosition> pos;

		GarlicError(GErrorCode code, std::string message, std::shared_ptr<GPosition> pos);
		~GarlicError();

		std::string toString();
};