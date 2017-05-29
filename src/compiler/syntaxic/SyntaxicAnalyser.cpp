#include "SyntaxicAnalyser.hpp"

#include <string>
#include <math.h>

#include "../value/Function.hpp"
#include "../instruction/Instruction.hpp"
#include "../value/Block.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Foreach.hpp"
#include "../instruction/For.hpp"
#include "../value/If.hpp"
#include "../value/Match.hpp"
#include "../instruction/Continue.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../instruction/ClassDeclaration.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Throw.hpp"
#include "../instruction/While.hpp"
#include "../value/AbsoluteValue.hpp"
#include "../value/Array.hpp"
#include "../value/Map.hpp"
#include "../value/Set.hpp"
#include "../value/ArrayAccess.hpp"
#include "../value/Boolean.hpp"
#include "../value/FunctionCall.hpp"
#include "../value/Nulll.hpp"
#include "../value/Number.hpp"
#include "../value/Object.hpp"
#include "../value/ObjectAccess.hpp"
#include "../value/PostfixExpression.hpp"
#include "../value/PrefixExpression.hpp"
#include "../value/Reference.hpp"
#include "../value/String.hpp"
#include "../value/VariableValue.hpp"
#include "../value/ArrayFor.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "SyntaxicalError.hpp"
#include "../lexical/Token.hpp"

using namespace std;

namespace ls {

SyntaxicAnalyser::SyntaxicAnalyser() {
	time = 0;
	nt = nullptr;
	t = nullptr;
	i = 0;
}

SyntaxicAnalyser::~SyntaxicAnalyser() {}

Function* SyntaxicAnalyser::analyse(vector<Token*>& tokens) {

	this->tokens = tokens;
	this->t = tokens.at(0);
	this->nt = tokens.size() > 1 ? tokens.at(1) : nullptr;
	this->i = 0;

	Function* function = new Function();
	function->body = eatMain();

	return function;
}

Block* SyntaxicAnalyser::eatMain() {

	auto block = new Block();

	while (true) {
		if (t->type == TokenType::FINISHED) {
			eat();
			break;
		} else if (t->type == TokenType::SEMICOLON) {
			eat();
		} else {
			auto ins = eatInstruction();
			if (ins) {
				block->instructions.push_back(ins);
			}
		}
	}
	return block;
}

/*
 * Detects whether a opening brace is an object or a block
 *
 * {} => object
 * {a: 12} => object
 * { 12 } => block
 */
bool SyntaxicAnalyser::isObject() {

	if (nt != nullptr and nt->type == TokenType::CLOSING_BRACE) {
		return true;
	}

	auto nnt = nextTokenAt(2);
	if (nt != nullptr and nt->type == TokenType::IDENT
			and nnt != nullptr and nnt->type == TokenType::COLON) {
		return true;
	}
	return false;
}

Value* SyntaxicAnalyser::eatBlockOrObject() {
	if (isObject()) {
		return eatObject();
	} else {
		return eatBlock();
	}
}

Block* SyntaxicAnalyser::eatBlock() {

	Block* block = new Block();

	bool brace = false;
	if (t->type == TokenType::OPEN_BRACE) {
		brace = true;
		eat();
	}

	while (true) {
		if (t->type == TokenType::CLOSING_BRACE) {
			eat();
			break;
		} else if (t->type == TokenType::FINISHED || t->type == TokenType::ELSE || t->type == TokenType::END || t->type == TokenType::IN) {
			if (brace) {
				errors.push_back(SyntaxicalError(t, SyntaxicalError::Type::BLOCK_NOT_CLOSED, {}));
			}
			break;
		} else if (t->type == TokenType::SEMICOLON) {
			eat();
		} else {
			Instruction* ins = eatInstruction();
			if (ins) block->instructions.push_back(ins);
		}
	}

	return block;
}

Object* SyntaxicAnalyser::eatObject() {

	eat(TokenType::OPEN_BRACE);

	auto o = new Object();

	while (t->type == TokenType::IDENT) {

		o->keys.push_back(std::unique_ptr<Token> { eatIdent() });
		eat(TokenType::COLON);
		o->values.push_back(eatExpression());

		if (t->type == TokenType::COMMA) {
			eat();
		}
	}
	eat(TokenType::CLOSING_BRACE);

	return o;
}

Instruction* SyntaxicAnalyser::eatInstruction() {

	switch (t->type) {

		case TokenType::LET:
		case TokenType::VAR:
		case TokenType::GLOBAL:
			return eatVariableDeclaration();

		case TokenType::NUMBER:
		case TokenType::PI:
		case TokenType::TRUE:
		case TokenType::FALSE:
		case TokenType::STRING:
		case TokenType::IDENT:
		case TokenType::OPEN_BRACKET:
		case TokenType::OPEN_PARENTHESIS:
		case TokenType::OPEN_BRACE:
		case TokenType::NULLL:
		case TokenType::ARROW:
		case TokenType::IF:
		case TokenType::MINUS:
		case TokenType::NOT:
		case TokenType::MINUS_MINUS:
		case TokenType::PLUS_PLUS:
		case TokenType::NEW:
		case TokenType::AROBASE:
		case TokenType::PLUS:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::POWER:
		case TokenType::MODULO:
		case TokenType::PIPE:
		case TokenType::TILDE:
		case TokenType::LOWER:
			return new ExpressionInstruction(eatExpression());

		case TokenType::MATCH:
			return new ExpressionInstruction(eatMatch(false));

		case TokenType::FUNCTION:
			return eatFunctionDeclaration();

		case TokenType::RETURN: {
			eat();
			if (t->type == TokenType::FINISHED or t->type == TokenType::CLOSING_BRACE
				or t->type == TokenType::ELSE or t->type == TokenType::END) {
				return new Return();
			} else {
				return new Return(eatExpression());
			}
		}
		case TokenType::THROW: {
			auto throw_token = eat_get();
			if (t->type == TokenType::FINISHED or t->type == TokenType::CLOSING_BRACE
				or t->type == TokenType::ELSE or t->type == TokenType::END) {
				return new Throw(std::shared_ptr<Token>(throw_token));
			} else {
				return new Throw(std::shared_ptr<Token>(throw_token), eatExpression());
			}
		}
		case TokenType::BREAK:
			return eatBreak();

		case TokenType::CONTINUE:
			return eatContinue();

		case TokenType::CLASS:
			return eatClassDeclaration();

		case TokenType::FOR:
			return eatFor();

		case TokenType::WHILE:
			return eatWhile();

		default:
			errors.push_back(SyntaxicalError(t, SyntaxicalError::Type::UNEXPECTED_TOKEN, {to_string((int) t->type), t->content}));
			eat();
			return nullptr;
	}
}

VariableDeclaration* SyntaxicAnalyser::eatVariableDeclaration() {

	auto vd = new VariableDeclaration();

	if (t->type == TokenType::GLOBAL) {
		vd->keyword.reset(eat_get(TokenType::GLOBAL));
		vd->global = true;
	} else if (t->type == TokenType::LET) {
		vd->keyword.reset(eat_get(TokenType::LET));
		vd->constant = true;
	} else {
		vd->keyword.reset(eat_get(TokenType::VAR));
	}

	auto ident = eatIdent();
	vd->variables.push_back(std::unique_ptr<Token> { ident });
	if (t->type == TokenType::EQUAL) {
		eat(TokenType::EQUAL);
		vd->expressions.push_back(eatExpression());
	} else {
		vd->expressions.push_back(nullptr);
	}

	while (t->type == TokenType::COMMA) {
		eat();
		ident = eatIdent();
		vd->variables.push_back(std::unique_ptr<Token> { ident });
		if (t->type == TokenType::EQUAL) {
			eat(TokenType::EQUAL);
			vd->expressions.push_back(eatExpression());
		} else {
			vd->expressions.push_back(nullptr);
		}
	}
	return vd;
}

Function* SyntaxicAnalyser::eatFunction() {

	if (t->type == TokenType::FUNCTION) {
		eat();
	}

	auto f = new Function();

	eat(TokenType::OPEN_PARENTHESIS);

	while (t->type != TokenType::FINISHED && t->type != TokenType::CLOSING_PARENTHESIS) {

		bool reference = false;
		if (t->type == TokenType::AROBASE) {
			eat();
			reference = true;
		}

		auto ident = eatIdent();

		Value* defaultValue = nullptr;
		if (t->type == TokenType::EQUAL) {
			eat();
			defaultValue = eatExpression();
		}

		f->addArgument(ident, reference, defaultValue);

		if (t->type == TokenType::COMMA) {
			eat();
		}
	}
	eat(TokenType::CLOSING_PARENTHESIS);

	bool braces = false;
	if (t->type == TokenType::OPEN_BRACE) {
		braces = true;
	}

	f->body = eatBlock();

	if (!braces) {
		eat(TokenType::END);
	}

	return f;
}

VariableDeclaration* SyntaxicAnalyser::eatFunctionDeclaration() {

	eat(TokenType::FUNCTION);

	auto vd = new VariableDeclaration();
	vd->global = true;

	vd->variables.push_back(std::unique_ptr<Token> { eatIdent() });
	vd->expressions.push_back(eatFunction());

	return vd;
}

bool SyntaxicAnalyser::beginingOfExpression(TokenType type) {

	return type == TokenType::NUMBER or type == TokenType::IDENT
		or type == TokenType::AROBASE or type == TokenType::OPEN_BRACKET
		or type == TokenType::OPEN_BRACE or type == TokenType::OPEN_PARENTHESIS
		or type == TokenType::STRING or type == TokenType::PI or type == TokenType::TRUE
		or type == TokenType::FALSE or type == TokenType::NULLL;
}

int SyntaxicAnalyser::findNextClosingParenthesis() {
	int p = i;
	int level = 1;
	while (level > 0) {
		auto t = tokens.at(p++)->type;
		if (t == TokenType::FINISHED) return -1;
		if (t == TokenType::OPEN_PARENTHESIS) level++;
		if (t == TokenType::CLOSING_PARENTHESIS) level--;
	}
	return p - 1;
}

int SyntaxicAnalyser::findNextArrow() {
	int p = i;
	while (true) {
		auto t = tokens.at(p++)->type;
		if (t == TokenType::FINISHED) return -1;
		if (t == TokenType::ARROW) break;
	}
	return p - 1;
}

Value* SyntaxicAnalyser::eatSimpleExpression(bool pipe_opened, bool set_opened, bool comma_list, Value* initial) {

	Value* e = nullptr;

	if (initial == nullptr) {
		if (t->type == TokenType::OPEN_PARENTHESIS) {

			e = eatLambdaOrParenthesisExpression(pipe_opened, set_opened, comma_list);

		} else if (t->type == TokenType::PIPE) {

			auto open_pipe = eat_get();
			auto av = new AbsoluteValue();
			av->open_pipe.reset(open_pipe);
			av->expression = eatExpression(true);
			av->close_pipe.reset(eat_get(TokenType::PIPE));
			e = new Expression(av);

		} else {

			// Opérateurs unaires préfixe
			if (t->type == TokenType::NEW || t->type == TokenType::MINUS ||
				t->type == TokenType::PLUS || t->type == TokenType::NOT ||
				t->type == TokenType::MINUS_MINUS || t->type == TokenType::PLUS_PLUS
				|| t->type == TokenType::TILDE) {

				if (t->type == TokenType::MINUS && nt != nullptr && t->location.start.line == nt->location.start.line) {
					auto minus = eat_get();
					if (beginingOfExpression(t->type)) {

						auto op = new Operator(minus);
						auto ex = eatExpression(pipe_opened, set_opened);
						auto expr = dynamic_cast<Expression*>(ex);

						if (expr and expr->op->priority >= op->priority) {
							auto pexp = new PrefixExpression();
							pexp->operatorr = std::shared_ptr<Operator>(op);
							pexp->expression = expr->v1;
							expr->v1 = pexp;
							e = expr;
						} else {
							auto pe = new PrefixExpression();
							pe->operatorr = std::shared_ptr<Operator>(op);
							pe->expression = ex;
							e = pe;
						}
					} else {
						// No expression after the -, so it's the variable '-'
						e = new VariableValue(std::shared_ptr<Token>(minus));
					}

				} else if (t->type == TokenType::PLUS) {

					auto plus = eat_get(); // eat the +

					if (beginingOfExpression(t->type)) {
						delete plus;
						e = eatExpression(pipe_opened);
					} else {
						// No expression after the +, so it's the variable '+'
						e = new VariableValue(std::shared_ptr<Token>(plus));
					}

				} else {
					auto ex = new PrefixExpression();
					ex->operatorr = std::make_shared<Operator>(eat_get());
					ex->expression = eatSimpleExpression();
					e = new Expression(ex);
				}
			} else {
				e = eatValue(comma_list);
			}
		}
	} else {
		e = initial;
	}

	while (t->type == TokenType::OPEN_BRACKET || t->type == TokenType::OPEN_PARENTHESIS || t->type == TokenType::DOT) {

		if (t->location.start.column != last_character + last_size)
			break;

		switch (t->type) {
			case TokenType::OPEN_BRACKET: {

				auto aa = new ArrayAccess();
				aa->open_bracket.reset(eat_get(TokenType::OPEN_BRACKET));

				aa->array = e;
				if (t->type == TokenType::CLOSING_BRACKET) {
					aa->key = nullptr;
				} else {
					aa->key = eatExpression();
				}

				if (t->type == TokenType::COLON) {
					eat();
					aa->key2 = eatExpression();
				}

				aa->close_bracket.reset(eat_get(TokenType::CLOSING_BRACKET));
				e = aa;
				break;
			}
			case TokenType::OPEN_PARENTHESIS: {

				auto par = eat_get(TokenType::OPEN_PARENTHESIS);

				auto fc = new FunctionCall(std::shared_ptr<Token>(par));
				fc->function = e;

				if (t->type != TokenType::CLOSING_PARENTHESIS) {
					fc->arguments.push_back(eatExpression(false, false, nullptr, true));
					while (t->type != TokenType::CLOSING_PARENTHESIS && t->type != TokenType::FINISHED) {
						if (t->type == TokenType::COMMA) {
							eat();
						}
						fc->arguments.push_back(eatExpression(false, false, nullptr, true));
					}
				}
				fc->closing_parenthesis.reset(eat_get(TokenType::CLOSING_PARENTHESIS));

				e = fc;
				break;
			}
			case TokenType::DOT: {

				ObjectAccess* oa;
				eat(TokenType::DOT);

				if (t->type == TokenType::NEW || t->type == TokenType::CLASS) {
					oa = new ObjectAccess(std::shared_ptr<Token>(eat_get()));
				} else if (t->type == TokenType::RETURN) {
					oa = new ObjectAccess(std::shared_ptr<Token>(eat_get()));
				} else {
					oa = new ObjectAccess(std::shared_ptr<Token>(eatIdent()));
				}
				oa->object = e;
				e = oa;
				break;
			}
			default: {}
		}
	}

	// Opérateurs unaires postfixes
	if (t->type == TokenType::MINUS_MINUS || t->type == TokenType::PLUS_PLUS) {

		if (last_line == t->location.start.line) {

			auto op = eat_get();
			auto ex = new PostfixExpression();

			ex->operatorr = std::make_shared<Operator>(op);
			ex->expression = (LeftValue*) e;
			e = ex;
		}
	}

	return e;
}

Value* SyntaxicAnalyser::eatExpression(bool pipe_opened, bool set_opened, Value* initial, bool comma_list) {

	Expression* ex = nullptr;
	Value* e = (initial != nullptr) ? initial : eatSimpleExpression(pipe_opened, set_opened, comma_list);

	// Opérateurs binaires
	while (t->type == TokenType::PLUS || t->type == TokenType::MINUS ||
		   t->type == TokenType::TIMES || t->type == TokenType::DIVIDE ||
		   t->type == TokenType::INT_DIV || t->type == TokenType::INT_DIV_EQUAL ||
		   t->type == TokenType::MODULO || t->type == TokenType::AND ||
		   t->type == TokenType::OR || t->type == TokenType::XOR ||
		   t->type == TokenType::EQUAL || t->type == TokenType::POWER ||
		   t->type == TokenType::DOUBLE_EQUAL || t->type == TokenType::DIFFERENT ||
		   t->type == TokenType::TRIPLE_EQUAL || t->type == TokenType::TRIPLE_DIFFERENT ||
		   (!set_opened && t->type == TokenType::GREATER) || t->type == TokenType::LOWER ||
		   t->type == TokenType::GREATER_EQUALS || t->type == TokenType::LOWER_EQUALS ||
		   t->type == TokenType::TIMES_EQUAL || t->type == TokenType::PLUS_EQUAL ||
		   t->type == TokenType::MINUS_EQUAL || t->type == TokenType::DIVIDE_EQUAL ||
		   t->type == TokenType::MODULO_EQUAL || t->type == TokenType::POWER_EQUAL ||
		   t->type == TokenType::SWAP || t->type == TokenType::TILDE ||
		   t->type == TokenType::TILDE_TILDE || t->type == TokenType::TILDE_EQUAL ||
		   t->type == TokenType::TILDE_TILDE_EQUAL || t->type == TokenType::IN ||
		   t->type == TokenType::INSTANCEOF ||
		   t->type == TokenType::BIT_AND || t->type == TokenType::BIT_AND_EQUALS ||
		   (!pipe_opened and t->type == TokenType::PIPE) || t->type == TokenType::BIT_OR_EQUALS ||
		   t->type == TokenType::BIT_XOR || t->type == TokenType::BIT_XOR_EQUALS ||
		   t->type == TokenType::BIT_SHIFT_LEFT ||	t->type == TokenType::BIT_SHIFT_LEFT_EQUALS ||
		   t->type == TokenType::BIT_SHIFT_RIGHT || t->type == TokenType::BIT_SHIFT_RIGHT_EQUALS ||
		   t->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED || t->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS ||
		   t->type == TokenType::BIT_ROTATE_LEFT || t->type == TokenType::BIT_ROTATE_LEFT_EQUALS ||
		   t->type == TokenType::BIT_ROTATE_RIGHT || t->type == TokenType::BIT_ROTATE_RIGHT_EQUALS ||
		   t->type == TokenType::DOUBLE_QUESTION_MARK || t->type == TokenType::CATCH_ELSE
		   ) {

		if (t->type == TokenType::MINUS && t->location.start.line != last_line && nt != nullptr && t->location.start.line == nt->location.start.line)
			break;

		auto op = new Operator(eat_get());

		if (ex == nullptr) {
			if (auto exx = dynamic_cast<Expression*>(e)) {
				ex = exx;
			} else {
				ex = new Expression();
				ex->v1 = e;
			}
		}
		ex->append(std::shared_ptr<Operator>(op), eatSimpleExpression());
	}
	if (ex != nullptr) {
		e = ex;
	}

	// Ternary
	if (t->type == TokenType::QUESTION_MARK) {
		eat();
		auto ternary = new If(true);
		ternary->condition = e;

		auto then = new Block();
		then->instructions.push_back(new ExpressionInstruction(eatExpression()));
		ternary->then = then;

		eat(TokenType::COLON);

		auto elze = new Block();
		elze->instructions.push_back(new ExpressionInstruction(eatExpression()));
		ternary->elze = elze;
		return ternary;
	}

	return e;
}

Value* SyntaxicAnalyser::eatValue(bool comma_list) {

	switch (t->type) {

		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::MODULO:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::POWER:
		case TokenType::TERNARY:
		case TokenType::INT_DIV:
		{
			return new VariableValue(std::shared_ptr<Token>(eat_get()));
		}

		case TokenType::NUMBER:
		{
			auto n_token = eat_get();
			auto n = new Number(n_token->content, std::shared_ptr<Token>(n_token));

			if (t->type == TokenType::STAR) {
				n->pointer = true;
				eat();
			}
			return n;
		}

		case TokenType::PI: {
			std::stringstream stream;
			stream << std::fixed << std::setprecision(19) << M_PI;
			return new Number(stream.str(), std::shared_ptr<Token>(eat_get()));
		}
		case TokenType::STRING:
		{
			return new String(std::shared_ptr<Token>(eat_get()));
		}

		case TokenType::TRUE:
		case TokenType::FALSE:
		{
			return new Boolean(std::shared_ptr<Token>(eat_get()));
		}

		case TokenType::NULLL:
			return new Nulll(std::shared_ptr<Token>(eat_get()));

		case TokenType::AROBASE:
		case TokenType::IDENT:
			return eatLambdaOrParenthesisExpression(false, false, comma_list);
			break;

		case TokenType::OPEN_BRACKET:
			return eatArrayOrMap();

		case TokenType::LOWER:
			return eatSet();

		case TokenType::OPEN_BRACE:
			return eatBlockOrObject();

		case TokenType::IF:
			return eatIf();

		case TokenType::MATCH:
			return eatMatch(true);

		case TokenType::FUNCTION:
			return eatFunction();


		case TokenType::ARROW:
		{
			Function* l = new Function();
			l->lambda = true;
			eat(TokenType::ARROW);
			l->body = new Block();
			l->body->instructions.push_back(new ExpressionInstruction(eatExpression()));
			return l;
		}

		default:
			break;
	}

	errors.push_back(SyntaxicalError(t, SyntaxicalError::EXPECTED_VALUE, {t->content}));
	eat();
	return nullptr;
}

/*
 * Starting from:
 *   <ident>, <open_parenthesis>, <arobase>
 * Can return:
 *   <variable_value>, <expression>, <lambda>
 */
Value* SyntaxicAnalyser::eatLambdaOrParenthesisExpression(bool pipe_opened, bool set_opened, bool comma_list) {
	bool parenthesis = false;
	if (t->type == TokenType::OPEN_PARENTHESIS) {
		eat();
		parenthesis = true;
	}
	bool arobase = false;
	if (t->type == TokenType::AROBASE) {
		eat();
		arobase = true;
	}
	if (arobase and t->type != TokenType::IDENT) {
		eat(TokenType::IDENT); // will fail, we need an ident after an arobase
		return new Nulll(nullptr);
	}
	if (parenthesis and t->type != TokenType::IDENT) {
		if (t->type == TokenType::CLOSING_PARENTHESIS) {
			return eatValue(); // error, expected a value got ')', it's wrong
		}
		// (...)
		auto ex = eatExpression();
		ex->parenthesis = true;
		eat(TokenType::CLOSING_PARENTHESIS);
		return ex;
	}
	auto ident = eatIdent();
	// var
	if (t->type == TokenType::EQUAL) {
		// var =
		auto eq = eat_get();
		auto ex = eatExpression();
		// var = <ex>
		if (parenthesis and t->type == TokenType::CLOSING_PARENTHESIS) {
			// (var = <ex>)
			eat();
			if (t->type == TokenType::ARROW) {
				// (var = <ex>) ->  [lambda]
				delete eq;
				return eatLambdaContinue(false, arobase, ident, ex, comma_list);
			} else {
				// (var = <ex>) <token ?>	[expression]
				Expression* e = new Expression();
				e->parenthesis = true;
				if (arobase) {
					e->v1 = new Reference(new VariableValue(std::shared_ptr<Token>(ident)));
				} else {
					e->v1 = new VariableValue(std::shared_ptr<Token>(ident));
				}
				e->op = std::make_shared<Operator>(eq);
				e->v2 = ex;
				return e;
			}
		} else if (t->type == TokenType::COMMA or t->type == TokenType::ARROW) {
			// var = <ex> ,|->  [lambda]
			delete eq;
			return eatLambdaContinue(parenthesis, arobase, ident, ex, comma_list);
		} else {
			// var = <ex> <?>
			Expression* e = new Expression();
			e->v1 = new VariableValue(std::shared_ptr<Token>(ident));
			e->op = std::make_shared<Operator>(eq);
			e->v2 = ex;
			return eatExpression(pipe_opened, set_opened, e);
		}
	} else if (t->type == TokenType::ARROW) {
		// var ->
		return eatLambdaContinue(parenthesis, arobase, ident, nullptr, comma_list);
	} else if (t->type == TokenType::COMMA) {
		// var,  [lambda]
		if (!parenthesis && comma_list) {
			return new VariableValue(std::shared_ptr<Token>(ident));
		}
		int p = findNextClosingParenthesis();
		int a = findNextArrow();
		if (parenthesis or (a != -1 and (a < p or p == -1))) {
			return eatLambdaContinue(parenthesis, arobase, ident, nullptr, comma_list);
		} else {
			return new VariableValue(std::shared_ptr<Token>(ident));
		}
	} else {
		if (parenthesis) {
			if (t->type == TokenType::CLOSING_PARENTHESIS) {
				// (var)  or  (@var)
				eat();
				if (t->type == TokenType::ARROW) {
					return eatLambdaContinue(false, arobase, ident, nullptr, comma_list);
				}
				if (arobase) {
					return new Reference(new VariableValue(std::shared_ptr<Token>(ident)));
				} else {
					return new VariableValue(std::shared_ptr<Token>(ident));
				}
			} else {
				// ( var + ... )
				auto v = [&]() -> Value* {
					if (arobase)
						return new Reference(new VariableValue(std::shared_ptr<Token>(ident)));
					else
						return new VariableValue(std::shared_ptr<Token>(ident));
				}();
				auto exx = eatSimpleExpression(false, false, false, v);
				auto ex = eatExpression(pipe_opened, set_opened, exx);
				ex->parenthesis = true;
				eat(TokenType::CLOSING_PARENTHESIS);
				return ex;
			}
		}
		// var <?>  [expression]
		if (arobase) {
			auto v = new VariableValue(std::shared_ptr<Token>(ident));
			auto ex = eatSimpleExpression(false, false, false, v);
			return new Reference(ex);
		} else {
			return new VariableValue(std::shared_ptr<Token>(ident));
		}
	}
}

/*
 * Continue to eat a lambda starting from a comma or the arrow
 */
Value* SyntaxicAnalyser::eatLambdaContinue(bool parenthesis, bool arobase, Ident ident, Value* expression, bool comma_list) {
	auto l = new Function();
	l->lambda = true;
	// Add first argument
	l->addArgument(ident.token, arobase, expression);
	// Add other arguments
	while (t->type == TokenType::COMMA) {
		eat();
		bool reference = false;
		if (t->type == TokenType::AROBASE) {
			eat();
			reference = true;
		}
		auto ident = eatIdent();
		Value* defaultValue = nullptr;
		if (t->type == TokenType::EQUAL) {
			eat();
			defaultValue = eatExpression();
		}
		l->addArgument(ident, reference, defaultValue);
	}
	if (t->type == TokenType::CLOSING_PARENTHESIS) {
		eat();
		parenthesis = false;
	}
	eat(TokenType::ARROW);
	l->body = new Block();
	l->body->instructions.push_back(new ExpressionInstruction(eatExpression(false, false, nullptr, comma_list)));
	if (parenthesis) {
		eat(TokenType::CLOSING_PARENTHESIS);
	}
	return l;
}

Value* SyntaxicAnalyser::eatArrayOrMap() {

	auto opening_bracket = eat_get(TokenType::OPEN_BRACKET);

	// Empty array
	if (t->type == TokenType::CLOSING_BRACKET) {
		auto array = new Array();
		array->opening_bracket.reset(opening_bracket);
		array->closing_bracket.reset(eat_get());
		return array;
	}

	// Empty map
	if (t->type == TokenType::COLON) {
		eat();
		auto map = new Map();
		map->opening_bracket.reset(opening_bracket);
		map->closing_bracket.reset(eat_get());
		return map;
	}

	// Array For
	if (t->type == TokenType::FOR) {
		ArrayFor* arrayFor = new ArrayFor();
		arrayFor->forr = eatFor();
		eat(TokenType::CLOSING_BRACKET);
		return arrayFor;
	}

	Value* value = eatExpression(false, false, nullptr, true);

	// eatInterval
	if (t->type == TokenType::TWO_DOTS) {

		Array* interval = new Array();
		interval->opening_bracket.reset(opening_bracket);
		interval->interval = true;
		interval->expressions.push_back(value);
		eat();
		interval->expressions.push_back(eatExpression());

		interval->closing_bracket.reset(eat_get(TokenType::CLOSING_BRACKET));
		return interval;
	}

	// eatMap
	if (t->type == TokenType::COLON) {

		auto map = new Map();
		map->opening_bracket.reset(opening_bracket);
		map->keys.push_back(value);
		eat();
		map->values.push_back(eatExpression());

		while (t->type != TokenType::CLOSING_BRACKET && t->type != TokenType::FINISHED) {
			if (t->type == TokenType::COMMA)
				eat();
			map->keys.push_back(eatExpression());
			eat(TokenType::COLON);
			map->values.push_back(eatExpression());
		}
		map->closing_bracket.reset(eat_get(TokenType::CLOSING_BRACKET));
		return map;
	}

	// eatArray
	auto array = new Array();
	array->opening_bracket.reset(opening_bracket);
	array->expressions.push_back(value);
	if (t->type == TokenType::COMMA) {
		eat();
	}
	while (t->type != TokenType::CLOSING_BRACKET && t->type != TokenType::FINISHED) {
		array->expressions.push_back(eatExpression(false, false, nullptr, true));
		if (t->type == TokenType::COMMA)
			eat();
	}
	array->closing_bracket.reset(eat_get(TokenType::CLOSING_BRACKET));
	return array;
}

Set* SyntaxicAnalyser::eatSet() {
	eat(TokenType::LOWER);

	auto set = new Set();

	if (t->type == TokenType::GREATER) {
		eat();
		return set;
	}

	set->expressions.push_back(eatExpression(false, true));

	while (t->type != TokenType::GREATER && t->type != TokenType::FINISHED) {
		if (t->type == TokenType::COMMA) eat();
		set->expressions.push_back(eatExpression(false, true));
	}
	eat(TokenType::GREATER);

	return set;
}

If* SyntaxicAnalyser::eatIf() {

	auto iff = new If();

	eat(TokenType::IF);

	iff->condition = eatExpression();

	bool braces = false;
	bool then = false;
	if (t->type == TokenType::OPEN_BRACE) {
		braces = true;
	} else if (t->type == TokenType::THEN) {
		eat(TokenType::THEN);
		then = true;
	}

	if (then or braces) {
		Value* v = eatBlockOrObject();
		if (dynamic_cast<Block*>(v)) {
			iff->then = (Block*) v;
		} else {
			auto block = new Block();
			block->instructions.push_back(new ExpressionInstruction(v));
			iff->then = block;
		}
	} else {
		Block* block = new Block();
		block->instructions.push_back(eatInstruction());
		iff->then = block;
	}

	if (then) {
		if (t->type != TokenType::ELSE) {
			eat(TokenType::END);
		}
	}

	if (t->type == TokenType::ELSE) {
		eat();

		bool bracesElse = false;
		if (t->type == TokenType::OPEN_BRACE) {
			bracesElse = true;
		}

		if (then or bracesElse) {
			Value* v = eatBlockOrObject();
			if (dynamic_cast<Block*>(v)) {
				iff->elze = (Block*) v;
			} else {
				Block* block = new Block();
				block->instructions.push_back(new ExpressionInstruction(v));
				iff->elze = block;
			}
		} else {
			Block* body = new Block();
			body->instructions.push_back(eatInstruction());
			iff->elze = body;
		}

		if (then) {
			eat(TokenType::END);
		}
	}

	return iff;
}

Match* SyntaxicAnalyser::eatMatch(bool force_value) {

	Match* match = new Match();

	eat(TokenType::MATCH);

	match->value = eatExpression();

	eat(TokenType::OPEN_BRACE);

	while (t->type != TokenType::CLOSING_BRACE && t->type != TokenType::FINISHED) {
		vector<Match::Pattern> patterns;
		patterns.push_back(eatMatchPattern());
		while (t->type == TokenType::PIPE) {
			eat();
			patterns.push_back(eatMatchPattern());
		}
		match->pattern_list.push_back(patterns);
		eat(TokenType::COLON);
		if (t->type == TokenType::OPEN_BRACE) {
			match->returns.push_back(eatBlockOrObject());
		} else if (force_value) {
			Block* block = new Block();
			block->instructions.push_back(new ExpressionInstruction(eatExpression()));
			match->returns.push_back(block);
		} else {
			Block* body = new Block();
			body->instructions.push_back(eatInstruction());
			match->returns.push_back(body);
		}

		while (t->type == TokenType::SEMICOLON) eat();
	}

	eat(TokenType::CLOSING_BRACE);
	return match;
}

Match::Pattern SyntaxicAnalyser::eatMatchPattern() {

	if (t->type == TokenType::TWO_DOTS) {
		eat();

		if (t->type == TokenType::COLON || t->type == TokenType::PIPE) {
			return Match::Pattern(nullptr, nullptr);
		} else {
			return Match::Pattern(nullptr, eatSimpleExpression());
		}
	}

	Value* value = eatSimpleExpression();

	if (t->type == TokenType::TWO_DOTS) {
		eat();
		if (t->type == TokenType::COLON || t->type == TokenType::PIPE) {
			return Match::Pattern(value, nullptr);
		} else {
			return Match::Pattern(value, eatSimpleExpression());
		}
	} else {
		return Match::Pattern(value);
	}
}

Instruction* SyntaxicAnalyser::eatFor() {

	eat(TokenType::FOR);

	bool parenthesis = false;
	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	// Foreach:
	// for v in || for var v in || for k, v in
	// for var k, v in || for k, var v in || for var k, var v in
	auto t1 = t->type, t2 = nt->type, t3 = nextTokenAt(2)->type, t4 = nextTokenAt(3)->type, t5 = nextTokenAt(4)->type, t6 = nextTokenAt(5)->type;
	auto var = TokenType::VAR, let = TokenType::LET, ident = TokenType::IDENT, comma = TokenType::COMMA, colon = TokenType::COLON, in = TokenType::IN;
	bool is_foreach = (t1 == ident and t2 == in)
		or ((t1 == var or t1 == let) and t2 == ident and t3 == in)
		or (t1 == ident and (t2 == comma or t2 == colon) and t3 == ident and t4 == in)
		or ((t1 == var or t1 == let) and t2 == ident and (t3 == comma or t3 == colon) and t4 == ident and t5 == in)
		or (t1 == ident and (t2 == comma or t2 == colon) and (t3 == var or t3 == let) and t4 == ident and t5 == in)
		or ((t1 == var or t1 == let) and t2 == ident and (t3 == comma or t3 == colon) and (t4 == var or t4 == let) and t5 == ident and t6 == in);

	if (!is_foreach) {

		vector<Instruction*> inits;
		while (true) {
			if (t->type == TokenType::FINISHED || t->type == TokenType::SEMICOLON || t->type == TokenType::IN || t->type == TokenType::OPEN_BRACE) {
				break;
			}
			auto ins = eatInstruction();
			if (ins) inits.push_back(ins);
		}

		// for inits; condition; increments { body }
		auto f = new For();

		// init
		f->inits = inits;
		eat(TokenType::SEMICOLON);

		// condition
		if (t->type == TokenType::SEMICOLON) {
			f->condition = nullptr;
			eat();
		} else {
			f->condition = eatExpression();
			eat(TokenType::SEMICOLON);
		}

		// increment
		while (true) {
			if (t->type == TokenType::FINISHED || t->type == TokenType::SEMICOLON || t->type == TokenType::DO || t->type == TokenType::OPEN_BRACE || t->type == TokenType::CLOSING_PARENTHESIS) {
				break;
			}
			Instruction* ins = eatInstruction();
			if (ins) f->increments.push_back(ins);
		}

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		// body
		if (t->type == TokenType::OPEN_BRACE) {
			f->body = eatBlock();
		} else if (t->type == TokenType::DO) {
			eat(TokenType::DO);
			f->body = eatBlock();
			eat(TokenType::END);
		} else {
			auto block = new Block();
			block->instructions.push_back(eatInstruction());
			f->body = block;
		}
		return f;

	} else {

		// for key , value in container { body }
		auto f = new Foreach();

		if (t->type == TokenType::LET or t->type == TokenType::VAR) eat();

		if (nt->type == TokenType::COMMA || nt->type == TokenType::COLON) {
			f->key.reset(eatIdent());
			eat();
		}
		if (t->type == TokenType::LET or t->type == TokenType::VAR) eat();

		f->value.reset(eatIdent());

		eat(TokenType::IN);

		f->container = eatExpression();

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		// body
		if (t->type == TokenType::OPEN_BRACE) {
			f->body = eatBlock();
		} else {
			eat(TokenType::DO);
			f->body = eatBlock();
			eat(TokenType::END);
		}

		return f;
	}
}

Instruction* SyntaxicAnalyser::eatWhile() {

	auto while_token = eat_get(TokenType::WHILE);

	auto w = new While();
	w->token.reset(while_token);

	bool parenthesis = false;
	bool braces = false;

	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	w->condition = eatExpression();

	if (parenthesis) {
		eat(TokenType::CLOSING_PARENTHESIS);
	}
	if (t->type == TokenType::OPEN_BRACE) {
		braces = true;
	} else {
		eat(TokenType::DO);
	}

	w->body = eatBlock();

	if (!braces) {
		eat(TokenType::END);
	}

	return w;
}

Break* SyntaxicAnalyser::eatBreak() {
	auto token = eat_get(TokenType::BREAK);
	Break* b = new Break();
	b->token.reset(token);

	if (t->type == TokenType::NUMBER /*&& t->line == lt->line*/) {
		int deepness = std::stoi(t->content);
		if (deepness <= 0) {
			errors.push_back(SyntaxicalError(t, SyntaxicalError::Type::BREAK_LEVEL_ZERO, std::vector<std::string>()));
		} else {
			b->deepness = deepness;
			eat();
		}
	}

	return b;
}

Continue* SyntaxicAnalyser::eatContinue() {
	eat(TokenType::CONTINUE);
	Continue* c = new Continue();

	if (t->type == TokenType::NUMBER /*&& t->line == lt->line*/) {
		int deepness = std::stoi(t->content);
		if (deepness <= 0) {
			errors.push_back(SyntaxicalError(t, SyntaxicalError::Type::CONTINUE_LEVEL_ZERO, std::vector<std::string>()));
		} else {
			c->deepness = deepness;
			eat();
		}
	}

	return c;
}

ClassDeclaration* SyntaxicAnalyser::eatClassDeclaration() {

	eat(TokenType::CLASS);

	auto token = eatIdent();

	auto cd = new ClassDeclaration(std::shared_ptr<Token>(token));
	eat(TokenType::OPEN_BRACE);

	while (t->type == TokenType::LET) {
		cd->fields.push_back(eatVariableDeclaration());
	}

	eat(TokenType::CLOSING_BRACE);

	return cd;
}

Token* SyntaxicAnalyser::eatIdent() {
	return eat_get(TokenType::IDENT);
}

void SyntaxicAnalyser::eat() {
	eat(TokenType::DONT_CARE);
}

void SyntaxicAnalyser::eat(TokenType type) {
	auto old = t;
	eat_get(type);
	if (old != nullptr) {
		delete old;
	}
}

Token* SyntaxicAnalyser::eat_get() {
	return eat_get(TokenType::DONT_CARE);
}

Token* SyntaxicAnalyser::eat_get(TokenType type) {

	auto eaten = t;

	last_character = t->location.start.column;
	last_line = t->location.start.line;
	last_size = t->size;
	if (i < tokens.size() - 1) {
		t = tokens[++i];
	} else {
		t = new Token(TokenType::FINISHED, 0, 0, 0, "");
	}
	nt = i < tokens.size() - 1 ? tokens[i + 1] : nullptr;

	if (type != TokenType::DONT_CARE && eaten->type != type) {
		errors.push_back(SyntaxicalError(eaten, SyntaxicalError::Type::UNEXPECTED_TOKEN, {eaten->content}));
		std::cout << "unexpected token : " << to_string((int) type) << " != " << to_string((int) eaten->type) << " (" << eaten->content << ") char " << eaten->location.start.column << std::endl;
		return new Token(TokenType::FINISHED, 0, 0, 0, "");
	}
	return eaten;
}

Token* SyntaxicAnalyser::nextTokenAt(int pos) {
	if (i + pos < tokens.size())
		return tokens[i + pos];
	else
		return new Token(TokenType::FINISHED, 0, 0, 0, "");
}

vector<SyntaxicalError> SyntaxicAnalyser::getErrors() {
	return errors;
}

}
