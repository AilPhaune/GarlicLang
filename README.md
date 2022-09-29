# Garlic
Multi purpose embeded programming language.

## Usage
**No main function provided.**

In the below code snippets, it will be assumed that you have already included "Tokens.h", "Lexer.h" and \<iostream\>

[Lexing](https://en.wikipedia.org/wiki/Lexical_analysis) the source code into [tokens](https://en.wikipedia.org/wiki/Lexical_analysis#Token) using the [lexer](https://en.wikipedia.org/wiki/Lexical_analysis#Scanner).
```cpp
// expected SOURCE_CODE to be std::string
// PATH is an indicator of where the source comes from, also std::string
GLexer your_lexer(PATH, SOURCE_CODE);
std::shared_ptr<GLexingResult> lex_result = lexer.makeTokens();
if (lex_result->hasError()) {
    // handle error
    // use lex_result->getError() to get the GarlicError object (std::shared_ptr<GarlicError>)
    // note that GarlicError has a method std::string toString();
}
std::vector<std::shared_ptr<GToken>> tokens = res->getTokens();
```
[Parsing](https://en.wikipedia.org/wiki/Parsing) the tokens into an [AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree) using the [parser](https://en.wikipedia.org/wiki/Parsing#Parser) (note that some features of the language may not be implemented yet)
```cpp
GParser parser(tokens);
std::shared_ptr<GParsingResult> parsing_result = parser.parseAST();
if (parsing_result->error != nullptr) {
    // handle error
}
GNode ast = parsing_result->node;
```
Maybe print the ast ?
```cpp
std::string s = GNode::toString(ast);
std::cout << s << std::endl;
```

# Implementation status
- means implemented

**[x] means not yet implemented**

### [Lexer / Scanner](https://en.wikipedia.org/wiki/Lexical_analysis#Scanner)
- All symbols and operators
- Numbers (integers and decimals), number type hints
- Identifiers and keyword

**[x] Hexadecimal, octal, and binary litterals**

### [Parser](https://en.wikipedia.org/wiki/Parsing#Parser)
- All operators
- Function calls (parenthesis operator)
- Statements
- Comparisons
- Litterals (string, char, numbers, identifiers, boolean litteral, **[x] bigint litterals**)
- Assignment and multi-assignment (assignment of multiple variables and multiple values)
- If, Else, Else If statements
- For, While and Do While loops
- Variable declaration

**[x] Class, Interface, Enum, Annotation declaration**

**[x] Function, Method declaration**

**[x] Operator overloading**

**[x] Annotating variables, functions, methods, classes, interfaces, enums, other annotations**

### [Symbol Table](https://en.wikipedia.org/wiki/Symbol_table)
**[x] Not implemented yet**

### [Intermediate Representation](https://en.wikipedia.org/wiki/Intermediate_representation#Intermediate_representation)
**[x] Not implemented yet**

### [Optimizer](https://en.wikipedia.org/wiki/Program_optimization)
**[x] Not implemented yet**

### [Interpreter](https://en.wikipedia.org/wiki/Interpreter_(computing))
**[x] Not implemented yet**
