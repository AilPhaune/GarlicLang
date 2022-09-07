#include "Nodes.h"
#include <sstream>

std::shared_ptr<PrettyPrint> GNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint("UNKNOWN_NODE", std::vector<std::shared_ptr<PrettyPrint>>()));
}

std::shared_ptr<PrettyPrint> GBinOpNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		GOperator::toString(this->operation),
		std::vector<std::shared_ptr<PrettyPrint>>({ this->left->prettyPrint(), this->right->prettyPrint() })
	));
}

std::shared_ptr<PrettyPrint> GSetterNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		GOperator::toString(this->operation),
		std::vector<std::shared_ptr<PrettyPrint>>({ this->left->prettyPrint(), this->right->prettyPrint() })
	));
}

std::shared_ptr<PrettyPrint> GUnaryOpNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		GOperator::toString(this->operation),
		std::vector<std::shared_ptr<PrettyPrint>>({ this->expr->prettyPrint() })
	));
}

std::shared_ptr<PrettyPrint> GCompOpNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		GOperator::toString(this->operation),
		std::vector<std::shared_ptr<PrettyPrint>>({ this->left->prettyPrint(), this->right->prettyPrint() })
	));
}

std::shared_ptr<PrettyPrint> GValueNode::prettyPrint() {
	std::stringstream ret;
	ret << "VALUE";
	if (this->type == GValueType::BIGINT) {
		// TODO
	}
	else if (this->type == GValueType::BOOLEAN) {
		ret << " bool { " << *((bool*)(this->value.get()));
	}
	else if (this->type == GValueType::CHAR) {
		ret << " char { " << *((char*)(this->value.get()));
	}
	else if (this->type == GValueType::DOUBLE) {
		ret << " double { " << *((double_t*)(this->value.get()));
	}
	else if (this->type == GValueType::FLOAT) {
		ret << " float { " << *((float_t*)(this->value.get()));
	}
	else if (this->type == GValueType::INT64) {
		ret << " int64 { " << *((int64_t*)(this->value.get()));
	}
	else if (this->type == GValueType::INT32) {
		ret << " int32 { " << *((int32_t*)(this->value.get()));
	}
	else if (this->type == GValueType::INT16) {
		ret << " int16 { " << *((int16_t*)(this->value.get()));
	}
	else if (this->type == GValueType::INT8) {
		ret << " int8 { " << *((int8_t*)(this->value.get()));
	}
	else if (this->type == GValueType::UINT64) {
		ret << " uint64 { " << *((uint64_t*)(this->value.get()));
	}
	else if (this->type == GValueType::UINT32) {
		ret << " uint32 { " << *((uint32_t*)(this->value.get()));
	}
	else if (this->type == GValueType::UINT16) {
		ret << " uint16 { " << *((uint16_t*)(this->value.get()));
	}
	else if (this->type == GValueType::UINT8) {
		ret << " uint8 { " << *((uint8_t*)(this->value.get()));
	}
	else if (this->type == GValueType::STRING) {
		ret << " string { " << *((std::string*)(this->value.get()));
	}
	ret << " }";
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(ret.str(), std::vector<std::shared_ptr<PrettyPrint>>()));
}

std::shared_ptr<PrettyPrint> GIdentifierNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint("IDENTIFIER { " + this->identifier + " }", std::vector<std::shared_ptr<PrettyPrint>>()));
}

std::shared_ptr<PrettyPrint> GTupleNode::prettyPrint() {
	std::vector<std::shared_ptr<PrettyPrint>> children;
	std::vector<std::shared_ptr<GNode>>::iterator it = this->values.begin();
	while (it != this->values.end()) {
		children.push_back(it->get()->prettyPrint());
		it++;
	}
	return std::shared_ptr<PrettyPrint>(new PrettyPrint("TUPLE", children));
}

std::shared_ptr<PrettyPrint> GNodeList::prettyPrint() {
	std::vector<std::shared_ptr<PrettyPrint>> children;
	std::vector<std::shared_ptr<GNode>>::iterator it = this->body.begin();
	while (it != this->body.end()) {
		children.push_back(it->get()->prettyPrint());
		it++;
	}
	return std::shared_ptr<PrettyPrint>(new PrettyPrint("NODE LIST", children));
}

std::shared_ptr<PrettyPrint> GNonGarlicNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		"NON GARLIC { \"" + (this->value.length() > 20 ? (this->value.substr(0, 17) + "...") : this->value) + "\" }",
		std::vector<std::shared_ptr<PrettyPrint>>()
	));
}

std::shared_ptr<PrettyPrint> GTernaryNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		"TERNARY",
		std::vector<std::shared_ptr<PrettyPrint>>(
			{ this->condition->prettyPrint(), this->whenTrue->prettyPrint(), this->whenFalse->prettyPrint() }
		)
	));
}

std::shared_ptr<PrettyPrint> GParenthesisOperatorNode::prettyPrint() {
	std::vector<std::shared_ptr<PrettyPrint>> children = { this->value->prettyPrint() };
	std::vector<std::shared_ptr<GNode>>::iterator it = this->parameters.begin();
	while (it != this->parameters.end()) {
		children.push_back(it->get()->prettyPrint());
		it++;
	}
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		"PARENTHESIS OPERATOR / CALL FUNCTION",
		children
	));
}

std::shared_ptr<PrettyPrint> GBracketOperatorNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		"BRACKET OPERATOR",
		std::vector<std::shared_ptr<PrettyPrint>>({ this->value->prettyPrint(), this->accessor->prettyPrint() })
	));
}

std::shared_ptr<PrettyPrint> GDotAccessorNode::prettyPrint() {
	return std::shared_ptr<PrettyPrint>(new PrettyPrint(
		"DOT ACCESSOR",
		std::vector<std::shared_ptr<PrettyPrint>>({ 
			this->value->prettyPrint(),
			std::shared_ptr<PrettyPrint>(new PrettyPrint(
				this->accessor, 
				std::vector<std::shared_ptr<PrettyPrint>>()
			))
		})
	));
}