#pragma once
#include <memory>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <regex>
#include <memory>
#include <stack>
class Symbol;
class SymbolBranch;
class SymbolTable;
class SymbolResult;
class SymbolAnalyzer;
struct GToken;
struct GTokenResult;
struct GLexingResult;
class GLexer;
struct PrettyPrint;
struct GNode;
struct GBinOpNode;
struct GSetterNode;
struct GUnaryOpNode;
struct GCompNode;
struct GValueNode;
struct GIdentifierNode;
struct GTupleNode;
struct GNodeList;
struct GNonGarlicNode;
struct GTernaryNode;
struct GParenthesisOperatorNode;
struct GBracketOperatorNode;
struct GDotAccessorNode;
struct GIfStatementNode;
struct GScopeNode;
struct GVarDeclareNode;
class GOperator;
class GValueType;
struct GarlicError;
struct GPosition;
class Numbers;
struct GParsingResult;
class GParser;
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
std::vector<std::shared_ptr<GTokenResult>> tokens;
std::shared_ptr<GarlicError> error;
public:
GLexingResult();
~GLexingResult();
std::shared_ptr<GLexingResult> failure(std::shared_ptr<GarlicError> error);
std::shared_ptr<GLexingResult> push(std::shared_ptr<GTokenResult> token);
bool hasError();
size_t size();
std::shared_ptr<GToken> tokenAt(size_t idx);
std::shared_ptr<GarlicError> getError();
std::vector<std::shared_ptr<GToken>> getTokens();
};
class GLexer {
public:
static std::vector<std::pair<std::string, GTokenType>> DEFAULT_TOKEN_MAP;
static std::string NUMBERS, IDENTIFIER_START, IDENTIFIERS;
static std::vector<std::string> KEYWORDS;
std::string file;
const char* content;
unsigned int idx, line, linePos;
size_t contentlength;
GLexer(std::string file, const char* content, size_t contentlength);
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
std::shared_ptr<GLexingResult> makeRawTokens();
};
enum GNodeType {
NODE_UNKNOWN, NODE_BIN_OP, NODE_SETTER, NODE_UNARY_OP, NODE_COMP_OP, NODE_VALUE, NODE_IDENTIFIER, NODE_TUPLE, NODE_LIST,
NODE_NON_GARLIC, NODE_TERNARY, NODE_PARENTHESIS_OPERATOR, NODE_BRACKET_OPERATOR, NODE_DOT_ACCESSOR, NODE_IF_STATEMENT,
NODE_FOR_LOOP, NODE_WHILE_LOOP, NODE_DO_WHILE_LOOP, NODE_SCOPE, VAR_DECLARE_NODE, TYPE_NODE,
};
struct PrettyPrint {
public:
std::string name;
std::vector<std::shared_ptr<PrettyPrint>> children;
PrettyPrint(std::string name, std::vector<std::shared_ptr<PrettyPrint>> children);
~PrettyPrint();
};
struct GNode : public std::enable_shared_from_this<GNode> {
public:
static std::string toString(std::shared_ptr<PrettyPrint> beautiful, uint32_t spaces = 4);
virtual bool hasValue();
virtual bool isValConst();
virtual bool needsSemicolon();
virtual std::shared_ptr<GNode> reduce();
virtual GNodeType getType();
virtual std::shared_ptr<PrettyPrint> prettyPrint();
std::shared_ptr<GPosition> pos;
GNode(std::shared_ptr<GPosition> pos);
~GNode();
};
struct GBinOpNode : public GNode {
public:
uint8_t operation;
std::shared_ptr<GNode> left, right;
GBinOpNode(std::shared_ptr<GNode> left, std::shared_ptr<GNode> right, uint8_t operation, std::shared_ptr<GPosition> pos);
~GBinOpNode();
bool hasValue() override;
bool isValConst() override;
std::shared_ptr<GNode> reduce() override;
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GSetterNode : public GNode {
public:
uint8_t operation;
std::shared_ptr<GNode> left, right;
GSetterNode(std::shared_ptr<GNode> left, std::shared_ptr<GNode> right, uint8_t operation, std::shared_ptr<GPosition> pos);
~GSetterNode();
bool hasValue() override;
bool isValConst() override;
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GUnaryOpNode : public GNode {
public:
uint8_t operation;
std::shared_ptr<GNode> expr;
GUnaryOpNode(std::shared_ptr<GNode> expr, uint8_t operation, std::shared_ptr<GPosition> pos);
~GUnaryOpNode();
bool hasValue() override;
bool isValConst() override;
std::shared_ptr<GNode> reduce() override;
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GCompOpNode : public GNode {
public:
uint8_t operation;
std::shared_ptr<GNode> left, right;
GCompOpNode(std::shared_ptr<GNode> left, std::shared_ptr<GNode> right, uint8_t operation, std::shared_ptr<GPosition> pos);
~GCompOpNode();
bool hasValue() override;
bool isValConst() override;
std::shared_ptr<GNode> reduce() override;
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GValueNode : public GNode {
public:
std::shared_ptr<void> value;
uint8_t type;
GValueNode(std::shared_ptr<void> value, uint8_t type, std::shared_ptr<GPosition> pos);
~GValueNode();
bool hasValue() override;
bool isValConst() override;
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GIdentifierNode : public GNode {
public:
std::string identifier;
GIdentifierNode(std::string identifier, std::shared_ptr<GPosition> pos);
~GIdentifierNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GTupleNode : public GNode {
public:
std::vector<std::shared_ptr<GNode>> values;
GTupleNode(std::vector<std::shared_ptr<GNode>> values, std::shared_ptr<GPosition> pos);
~GTupleNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GNodeList : public GNode {
public:
bool semicolon;
std::vector<std::shared_ptr<GNode>> body;
GNodeList(std::vector<std::shared_ptr<GNode>> body, std::shared_ptr<GPosition> pos);
~GNodeList();
bool needsSemicolon() override;
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GNonGarlicNode : public GNode {
public:
std::string value;
GNonGarlicNode(std::string value, std::shared_ptr<GPosition> pos);
~GNonGarlicNode();
bool needsSemicolon() override;
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GTernaryNode : public GNode {
public:
std::shared_ptr<GNode> condition, whenTrue, whenFalse;
GTernaryNode(std::shared_ptr<GNode> condition, std::shared_ptr<GNode> whenTrue, std::shared_ptr<GNode> whenFalse, std::shared_ptr<GPosition> pos);
~GTernaryNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GParenthesisOperatorNode : public GNode {
public:
std::shared_ptr<GNode> value;
std::vector<std::shared_ptr<GNode>> parameters;
GParenthesisOperatorNode(std::shared_ptr<GNode> value, std::vector<std::shared_ptr<GNode>> parameters, std::shared_ptr<GPosition> pos);
~GParenthesisOperatorNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GBracketOperatorNode : public GNode {
public:
std::shared_ptr<GNode> value, accessor;
GBracketOperatorNode(std::shared_ptr<GNode> value, std::shared_ptr<GNode> accessor, std::shared_ptr<GPosition> pos);
~GBracketOperatorNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GDotAccessorNode : public GNode {
public:
std::shared_ptr<GNode> value, accessor;
GDotAccessorNode(std::shared_ptr<GNode> value, std::shared_ptr<GNode> accessor, std::shared_ptr<GPosition> pos);
~GDotAccessorNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GIfStatementNode : public GNode {
public:
std::shared_ptr<GNode> condition, conditionTrue, conditionFalse;
GIfStatementNode(std::shared_ptr<GNode> condition, std::shared_ptr<GNode> conditionTrue, std::shared_ptr<GNode> conditionFalse, std::shared_ptr<GPosition> pos);
~GIfStatementNode();
GNodeType getType() override;
bool needsSemicolon() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GForLoopNode : public GNode {
public:
std::shared_ptr<GNode> initialize, condition, increment, body;
GForLoopNode(std::shared_ptr<GNode> initialize, std::shared_ptr<GNode> condition, std::shared_ptr<GNode> increment, std::shared_ptr<GNode> body, std::shared_ptr<GPosition> pos);
~GForLoopNode();
GNodeType getType() override;
bool needsSemicolon() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GWhileLoopNode : public GNode {
public:
std::shared_ptr<GNode> condition, body;
GWhileLoopNode(std::shared_ptr<GNode> condition, std::shared_ptr<GNode> body, std::shared_ptr<GPosition> pos);
~GWhileLoopNode();
GNodeType getType() override;
bool needsSemicolon() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GDoWhileLoopNode : public GNode {
public:
std::shared_ptr<GNode> condition, body;
GDoWhileLoopNode(std::shared_ptr<GNode> condition, std::shared_ptr<GNode> body, std::shared_ptr<GPosition> pos);
~GDoWhileLoopNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GScopeNode : public GNode {
public:
std::vector<std::string> scope;
GScopeNode(std::vector<std::string> scope, std::shared_ptr<GPosition> pos);
~GScopeNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GVarDeclareNode : GNode {
public:
std::shared_ptr<GNode> type, value;
std::string name;
bool isConst;
GVarDeclareNode(std::shared_ptr<GNode> type, std::string name, bool isConst, std::shared_ptr<GNode> value, std::shared_ptr<GPosition> pos);
~GVarDeclareNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
struct GTypeNode : GNode {
public:
std::string name;
bool native;
int arrayDimension;
std::vector<std::shared_ptr<GTypeNode>> generics;
GTypeNode(std::string name, bool native, int arrayDimension, std::vector<std::shared_ptr<GTypeNode>> generics, std::shared_ptr<GPosition> pos);
~GTypeNode();
GNodeType getType() override;
std::shared_ptr<PrettyPrint> prettyPrint() override;
};
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
struct GParsingResult : public std::enable_shared_from_this<GParsingResult> {
public:
static std::shared_ptr<GParsingResult> create();
std::shared_ptr<GarlicError> error;
std::shared_ptr<GNode> node;
std::string source;
GParsingResult();
~GParsingResult();
std::shared_ptr<GNode> reg(std::shared_ptr<GParsingResult> res);
std::shared_ptr<GParsingResult> success(std::shared_ptr<GNode> node);
std::shared_ptr<GParsingResult> failure(std::shared_ptr<GarlicError> error);
void reset();
};
class GParser {
private:
std::vector<std::shared_ptr<GToken>> tokens;
std::shared_ptr<GToken> token;
std::stack<int> history;
int pos;
void refresh();
void advance(int i = 1);
void push();
void pop();
std::string genUnexpected(std::shared_ptr<GToken> token);
std::shared_ptr<GParsingResult> makeStatements();
std::shared_ptr<GParsingResult> makeStatement();
std::shared_ptr<GParsingResult> makeStatementBase();
std::shared_ptr<GParsingResult> makeComplexExpression();
std::shared_ptr<GParsingResult> makeBooleanExpression();
std::shared_ptr<GParsingResult> makeComparisonExpression();
std::shared_ptr<GParsingResult> makeSimpleExpression();
std::shared_ptr<GParsingResult> makeTerm();
std::shared_ptr<GParsingResult> makeFactor();
std::shared_ptr<GParsingResult> makePower();
std::shared_ptr<GParsingResult> makeCall();
std::shared_ptr<GParsingResult> makeAtom();
std::pair<std::vector<std::shared_ptr<GNode>>, std::shared_ptr<GarlicError>> makeCommaSeparatedValues();
std::shared_ptr<GParsingResult> makeTernary(std::shared_ptr<GNode> atom);
std::shared_ptr<GParsingResult> makeParenthesisOperator(std::shared_ptr<GNode> atom);
std::shared_ptr<GParsingResult> makeBracketOperator(std::shared_ptr<GNode> atom);
std::shared_ptr<GParsingResult> makeDotAccessor(std::shared_ptr<GNode> atom);
std::shared_ptr<GParsingResult> makeSetter(std::shared_ptr<GNode> node);
std::shared_ptr<GParsingResult> makeIfStatement();
std::shared_ptr<GParsingResult> makeForLoop();
std::shared_ptr<GParsingResult> makeWhileLoop();
std::shared_ptr<GParsingResult> makeDoWhileLoop();
std::shared_ptr<GParsingResult> makeDeclaration();
std::shared_ptr<GParsingResult> makeVariableDeclaration(bool isConst);
std::shared_ptr<GParsingResult> makeScope();
std::shared_ptr<GParsingResult> makeType();
public:
GParser(std::vector<std::shared_ptr<GToken>> tokens);
~GParser();
std::shared_ptr<GParsingResult> parseAST();
};
