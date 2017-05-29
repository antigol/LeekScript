#ifndef SYNTAXICANALYSER_HPP
#define SYNTAXICANALYSER_HPP

#include <vector>

#include "SyntaxicalError.hpp"
#include "../value/Match.hpp"

namespace ls {

class Token;
enum class TokenType;
class Ident;
class Instruction;
class Value;
class ClassDeclaration;
class VariableDeclaration;
class If;
class For;
class Foreach;
class Program;
class Block;
class Object;
class Array;
class Set;
class Function;
class Break;
class Continue;

class SyntaxicAnalyser {

	std::vector<Token*> tokens;
	Token* t;
	int last_character = 0;
	size_t last_line = 0;
	size_t last_size = 0;
	Token* nt;
	unsigned i;
	std::vector<SyntaxicalError> errors;
	long time;
	std::vector<std::pair<unsigned,size_t>> stack;
	Token* finished_token;

public:

	SyntaxicAnalyser();
	~SyntaxicAnalyser();

	Function* analyse(std::vector<Token*>&);

	Block* eatMain();
	Token* eatIdent();
	Value* eatExpression(bool pipe_opened = false, bool set_opened = false, Value* initial = nullptr, bool comma_list = false);
	Value* eatSimpleExpression(bool pipe_opened = false, bool set_opened = false, bool comma_list = false, Value* initial = nullptr);
	Value* eatValue(bool comma_list = false);
	bool isObject();
	Value* eatBlockOrObject();
	Block* eatBlock();
	Object* eatObject();
	Value* eatArrayOrMap();
	Set* eatSet();
	If* eatIf();
	Match* eatMatch(bool force_value);
	Match::Pattern eatMatchPattern();
	Instruction* eatFor();
	Instruction* eatWhile();
	Break* eatBreak();
	Continue* eatContinue();
	ClassDeclaration* eatClassDeclaration();
	VariableDeclaration* eatVariableDeclaration();
	Function* eatFunction();
	VariableDeclaration* eatFunctionDeclaration();
	Instruction* eatInstruction();
	Value* eatLambdaContinue(bool parenthesis, bool arobase, Ident ident, Value* expression, bool comma_list = false);
	Value* eatLambdaOrParenthesisExpression(bool pipe_opened = false, bool set_opened = false, bool comma_list = false);

	bool beginingOfExpression(TokenType type);
	int findNextClosingParenthesis();
	int findNextArrow();
	bool isLambda();

	Token* eat_get();
	void eat();
	Token* eat_get(TokenType type);
	void eat(TokenType type);
	Token* nextTokenAt(int pos);

	std::vector<SyntaxicalError> getErrors();
};

}

#endif
