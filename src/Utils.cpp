#include "Utils.h"
#include <sstream>
#include <vector>

GarlicError::GarlicError(GErrorCode code, std::string message, std::shared_ptr<GPosition> pos) : code(code), message(message), pos(pos) {};
GarlicError::~GarlicError() {
	this->pos = nullptr;
}
std::string GarlicError::toString() {
	std::stringstream ret;
	ret << "Error: " << this->message << "\nAt " << this->pos->onlyStartString() << "\nE" << this->code << "";
	return ret.str();
}

uint8_t GOperator::ADD = 1,
		GOperator::SUBTRACT = 2,
		GOperator::MULTIPLY = 3,
		GOperator::DIVIDE = 4,
		GOperator::MODULO = 5,
		GOperator::EXPONENTIATION = 6,
		GOperator::XOR = 7,
		GOperator::OR = 8,
		GOperator::AND = 9,
		GOperator::BOOL_OR = 10,
		GOperator::BOOL_AND = 11,
		GOperator::LSHIFT = 12,
		GOperator::RSHIFT = 13,
		GOperator::LSHIFT_ROT = 14,
		GOperator::RSHIFT_ROT = 15,
		GOperator::UNARY_PLUS = 16,
		GOperator::UNARY_MINUS = 17,
		GOperator::UNARY_STAR = 18,
		GOperator::BITWISE_NOT = 19,
		GOperator::BOOL_NOT = 20,
		GOperator::LT = 21,
		GOperator::LTE = 22,
		GOperator::GT = 23,
		GOperator::GTE = 24,
		GOperator::EQ = 25,
		GOperator::NEQ = 26,
		GOperator::PLUS_EQ = 27,
		GOperator::MINUS_EQ = 28,
		GOperator::TIMES_EQ = 29,
		GOperator::DIVIDED_EQ = 30,
		GOperator::MODULO_EQ = 31,
		GOperator::POWER_EQ = 32,
		GOperator::AND_EQ = 33,
		GOperator::OR_EQ = 34,
		GOperator::XOR_EQ = 35,
		GOperator::LSHIFT_EQ = 36,
		GOperator::RSHIFT_EQ = 37,
		GOperator::LSHIFT_ROT_EQ = 38,
		GOperator::RSHIFT_ROT_EQ = 39,
		GOperator::SET_EQ = 40;
std::string GOperator::toString(uint8_t op) {
	std::vector<std::string> map = { "ADD", "SUBTRACT", "MULTIPLY", "DIVIDE", "MODULO", "POWER", "XOR", "OR", "AND", "BOOL OR", "BOOL AND",
			"LEFT SHIFT", "RIGHT SHIFT", "LEFT SHIFT ROTATE", "RIGHT SHIFT ROTATE", "UNARY PLUS", "UNARY MINUS", "UNARY_STAR", "BITWISE NOT",
			"LOGICAL NOT", "COMPARE <", "COMPARE <=", "COMPARE >", "COMPARE >=", "COMPARE ==", "COMPARE !=", "OPERATOR +=", "OPERATOR -=",
			"OPERATOR *=", "OPERATOR /=", "OPERATOR %=", "OPERATOR **=", "OPERATOR &=", "OPERATOR |=", "OPERATOR ^=", "OPERATOR <<=",
			"OPERATOR >>=", "OPERATOR <<<=", "OPERATOR >>>=", "SET ="};
	return map[op-1];
}

uint8_t GValueType::INT8 = 1,
		GValueType::UINT8 = 2,
		GValueType::INT16 = 3,
		GValueType::UINT16 = 4,
		GValueType::INT32 = 5,
		GValueType::UINT32 = 6,
		GValueType::INT64 = 7,
		GValueType::UINT64 = 8,
		GValueType::BIGINT = 9,
		GValueType::FLOAT = 10,
		GValueType::DOUBLE = 11,
		GValueType::STRING = 12,
		GValueType::CHAR = 13,
		GValueType::BOOLEAN = 14;

GPosition::GPosition(std::string file, int startIdx, int endIdx, int startLine, int endLine, int startLinePos, int endLinePos):
file(file), startIdx(startIdx), endIdx(endIdx), startLine(startLine), endLine(endLine), startLinePos(startLinePos),
endLinePos(endLinePos) {}
GPosition::~GPosition() {}
std::string GPosition::toString() {
	std::stringstream ret;
	ret << this->file << "(" << this->startLine << ":" << this->startLinePos << "[" << this->startIdx << "]-" << this->endLine << ":" << this->endLinePos << "[" << this->endIdx << "])";
	return ret.str();
}
std::string GPosition::noIdxString() {
	std::stringstream ret;
	ret << this->file << "(" << this->startLine << ":" << this->startLinePos << "-" << this->endLine << ":" << this->endLinePos << ")";
	return ret.str();
}
std::string GPosition::onlyStartString() {
	std::stringstream ret;
	ret << this->file << "(" << this->startLine << ":" << this->startLinePos << ")";
	return ret.str();
}

GPosition* GPosition::endsAt(GPosition* start, GPosition* end) {
	return new GPosition(start->file, start->startIdx, end->endIdx, start->startLine, end->endLine, start->startLinePos, end->endLinePos);
}

GPosition GPosition::endsAt(GPosition start, GPosition end) {
	return GPosition(start.file, start.startIdx, end.endIdx, start.startLine, end.endLine, start.startLinePos, end.endLinePos);
}

std::shared_ptr<GPosition> GPosition::endsAt(std::shared_ptr<GPosition> start, std::shared_ptr<GPosition> end) {
	return std::shared_ptr<GPosition>(new GPosition(start->file, start->startIdx, end->endIdx, start->startLine, end->endLine, start->startLinePos, end->endLinePos));
}