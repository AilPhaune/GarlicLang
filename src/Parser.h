#pragma once
#include <vector>
#include <stack>
#include "Tokens.h"
#include "Nodes.h"

struct GParsingResult;
class GParser;

struct GParsingResult : public std::enable_shared_from_this<GParsingResult> {
	public:
		static std::shared_ptr<GParsingResult> create();

		std::shared_ptr<GarlicError> error;
		std::shared_ptr<GNode> node;
		
		GParsingResult();
		~GParsingResult();

		std::shared_ptr<GNode> reg(std::shared_ptr<GParsingResult> res);
		std::shared_ptr<GParsingResult> success(std::shared_ptr<GNode> node);
		std::shared_ptr<GParsingResult> failure(std::shared_ptr<GarlicError> error);
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
	public:
		GParser(std::vector<std::shared_ptr<GToken>> tokens);
		~GParser();

		std::shared_ptr<GParsingResult> parseAST();
};