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
class TypeName;

class SyntaxicAnalyser {

	std::vector<Token> tokens;
	Token* t;
	Token* lt;
	Token* nt;
	unsigned i;
	std::vector<SyntaxicalError*> errors;
	long time;
	std::vector<std::pair<unsigned,size_t>> stack;

public:

	SyntaxicAnalyser();
	~SyntaxicAnalyser();

	Function* analyse(std::vector<Token>&);

	Block* eatMain();
	Token* eatIdent();
	Value* eatExpression(bool pipe_opened = false, bool set_opened = false);
	Value* eatSimpleExpression(bool pipe_opened = false, bool set_opened = false);
	Value* eatValue();
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
	TypeName* eatTypeName();
	ClassDeclaration* eatClassDeclaration();
	VariableDeclaration* eatVariableDeclaration();
	Function* eatFunction();
	VariableDeclaration* eatFunctionDeclaration();
	Instruction* eatInstruction();

	bool beginingOfExpression(TokenType type);

	Token* eat();
	Token* eat(TokenType type);
	Token* nextTokenAt(int pos);

	void save_current_state();
	void restore_saved_state();
	void forgot_saved_state();

	std::vector<SyntaxicalError*> getErrors();
	long getTime();
};

}

#endif
