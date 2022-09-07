#include "sstream"
#include "Nodes.h"


PrettyPrint::PrettyPrint(std::string name, std::vector<std::shared_ptr<PrettyPrint>> children): name(name), children(children) {}
PrettyPrint::~PrettyPrint() {
	this->children.clear();
}

std::string GNode::toString(std::shared_ptr<PrettyPrint> map, uint32_t spaces) {
	std::stringstream ret;
	ret << map->name;
	std::vector<std::shared_ptr<PrettyPrint>>::iterator it = map->children.begin();
	int i = 0;
	std::string spaces_ = "", rl = "";
	for (uint32_t j = 0; j < spaces; j++) {
		spaces_ += " ";
		rl += "-";
	}
	while (it != map->children.end()) {
		bool isNotLast = (i + 1) < map->children.size();
		ret << "\n|\n" << (isNotLast ? "+" : "@");
		std::string child = GNode::toString(map->children.at(i), spaces);
		size_t last = 0, next = 0, pos = 0;
		while ((next = child.find("\n", last)) != std::string::npos) {
			if (pos == 0) {
				ret << rl;
			}
			else {
				ret << (isNotLast ? "\n|" : "\n ") << spaces_;
			}
			ret << child.substr(last, next - last);
			last = next + 1;
			pos++;
		}
		if (pos == 0) {
			ret << rl;
		}
		else {
			ret << (isNotLast ? "\n|" : "\n ") << spaces_;
		}
		ret << child.substr(last);
		i++;
		it++;
	}
	return ret.str();
}
GNode::GNode(std::shared_ptr<GPosition> pos): pos(pos) {}
GNode::~GNode() {
	this->pos = nullptr;
}
bool GNode::hasValue() {
	return false;
}
bool GNode::isValConst() {
	return false;
}
bool GNode::needsSemicolon() {
	return true;
}
std::shared_ptr<GNode> GNode::reduce() {
	return shared_from_this();
}

GBinOpNode::GBinOpNode(std::shared_ptr<GNode> left, std::shared_ptr<GNode> right, uint8_t operation, std::shared_ptr<GPosition> pos):
	left(left), right(right), operation(operation), GNode(pos) {}
GBinOpNode::~GBinOpNode() {
	this->left = nullptr;
	this->right = nullptr;
}
bool GBinOpNode::hasValue() {
	return true;
}
bool GBinOpNode::isValConst() {
	return this->left->isValConst() && this->right->isValConst();
}
std::shared_ptr<GNode> GBinOpNode::reduce() {
	return shared_from_this(); // TODO
}

GSetterNode::GSetterNode(std::shared_ptr<GNode> left, std::shared_ptr<GNode> right, uint8_t operation, std::shared_ptr<GPosition> pos) :
	left(left), right(right), operation(operation), GNode(pos) {}
GSetterNode::~GSetterNode() {
	this->left = nullptr;
	this->right = nullptr;
}
bool GSetterNode::hasValue() {
	return true;
}
bool GSetterNode::isValConst() {
	return this->left->isValConst() && this->right->isValConst();
}

GUnaryOpNode::GUnaryOpNode(std::shared_ptr<GNode> expr, uint8_t operation, std::shared_ptr<GPosition> pos) :
	expr(expr), operation(operation), GNode(pos) {}
GUnaryOpNode::~GUnaryOpNode() {
	this->expr = nullptr;
}
bool GUnaryOpNode::hasValue() {
	return true;
}
bool GUnaryOpNode::isValConst() {
	return this->expr->isValConst();
}
std::shared_ptr<GNode> GUnaryOpNode::reduce() {
	return shared_from_this(); // TODO
}

GCompOpNode::GCompOpNode(std::shared_ptr<GNode> left, std::shared_ptr<GNode> right, uint8_t operation, std::shared_ptr<GPosition> pos) :
	left(left), right(right), operation(operation), GNode(pos) {}
GCompOpNode::~GCompOpNode() {
	this->left = nullptr;
	this->right = nullptr;
}
bool GCompOpNode::hasValue() {
	return true;
}
bool GCompOpNode::isValConst() {
	return this->left->isValConst() && this->right->isValConst();
}
std::shared_ptr<GNode> GCompOpNode::reduce() {
	return shared_from_this(); // TODO
}

GValueNode::GValueNode(std::shared_ptr<void> value, uint8_t type, std::shared_ptr<GPosition> pos) : value(value), type(type), GNode(pos) {}
GValueNode::~GValueNode() {
	this->value = nullptr;
}
bool GValueNode::isValConst() {
	return true;
}
bool GValueNode::hasValue() {
	return true;
}

GIdentifierNode::GIdentifierNode(std::string identifier, std::shared_ptr<GPosition> pos): identifier(identifier), GNode(pos) {}
GIdentifierNode::~GIdentifierNode() {}

GTupleNode::GTupleNode(std::vector<std::shared_ptr<GNode>> values, std::shared_ptr<GPosition> pos): values(values), GNode(pos) {}
GTupleNode::~GTupleNode() {
	this->values.clear();
}

GNodeList::GNodeList(std::vector<std::shared_ptr<GNode>> body, std::shared_ptr<GPosition> pos) : body(body), GNode(pos) {}
GNodeList::~GNodeList() {
	this->body.clear();
	this->semicolon = true;
}
bool GNodeList::needsSemicolon() {
	return this->semicolon;
}

GNonGarlicNode::GNonGarlicNode(std::string value, std::shared_ptr<GPosition> pos) : value(value), GNode(pos) {}
GNonGarlicNode::~GNonGarlicNode() {}
bool GNonGarlicNode::needsSemicolon() {
	return false;
}

GTernaryNode::GTernaryNode(std::shared_ptr<GNode> condition, std::shared_ptr<GNode> whenTrue, std::shared_ptr<GNode> whenFalse, std::shared_ptr<GPosition> pos):
condition(condition), whenTrue(whenTrue), whenFalse(whenFalse), GNode(pos) {}
GTernaryNode::~GTernaryNode() {
	this->condition = nullptr;
	this->whenFalse = nullptr;
	this->whenTrue = nullptr;
}

GParenthesisOperatorNode::GParenthesisOperatorNode(std::shared_ptr<GNode> value, std::vector<std::shared_ptr<GNode>> parameters, std::shared_ptr<GPosition> pos) :
value(value), parameters(parameters), GNode(pos) {}
GParenthesisOperatorNode::~GParenthesisOperatorNode() {
	this->value = nullptr;
	this->parameters.clear();
}

GBracketOperatorNode::GBracketOperatorNode(std::shared_ptr<GNode> value, std::shared_ptr<GNode> accessor, std::shared_ptr<GPosition> pos) :
	value(value), accessor(accessor), GNode(pos) {}
GBracketOperatorNode::~GBracketOperatorNode() {
	this->value = nullptr;
	this->accessor = nullptr;
}

GDotAccessorNode::GDotAccessorNode(std::shared_ptr<GNode> value, std::string accessor, std::shared_ptr<GPosition> pos) :
value(value), accessor(accessor), GNode(pos) {}
GDotAccessorNode::~GDotAccessorNode() {
	this->value = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                       NODE TYPES                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GNodeType GNode::getType() {
	return GNodeType::NODE_UNKNOWN;
}

GNodeType GBinOpNode::getType() {
	return GNodeType::NODE_BIN_OP;
}

GNodeType GSetterNode::getType() {
	return GNodeType::NODE_SETTER;
}

GNodeType GUnaryOpNode::getType() {
	return GNodeType::NODE_UNARY_OP;
}

GNodeType GCompOpNode::getType() {
	return GNodeType::NODE_COMP_OP;
}

GNodeType GValueNode::getType() {
	return GNodeType::NODE_VALUE;
}

GNodeType GIdentifierNode::getType() {
	return GNodeType::NODE_IDENTIFIER;
}

GNodeType GTupleNode::getType() {
	return GNodeType::NODE_TUPLE;
}

GNodeType GNodeList::getType() {
	return GNodeType::NODE_LIST;
}

GNodeType GNonGarlicNode::getType() {
	return GNodeType::NODE_NON_GARLIC;
}

GNodeType GTernaryNode::getType() {
	return GNodeType::NODE_TERNARY;
}

GNodeType GParenthesisOperatorNode::getType() {
	return GNodeType::NODE_PARENTHESIS_OPERATOR;
}

GNodeType GBracketOperatorNode::getType() {
	return GNodeType::NODE_BRACKET_OPERATOR;
}

GNodeType GDotAccessorNode::getType() {
	return GNodeType::NODE_DOT_ACCESSOR;
}