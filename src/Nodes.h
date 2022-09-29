#pragma once
#include <vector>
#include "Utils.h"

enum GNodeType;
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