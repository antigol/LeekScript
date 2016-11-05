#include "../../compiler/lexical/LexicalAnalyser.hpp"

#include <iostream>
#include "../../../lib/utf8.h"
#include <string.h>

using namespace std;

namespace ls {

static vector<vector<string>> type_literals = {

	{}, // Unknown
	{}, // Finished
	{}, // Don't care

	{}, // Number
	{}, // Ident
	{}, // String

	{ "let", "var" }, { "global" }, { "function" }, { "return" },
	{ "break" }, { "continue" },

	{ "!=", "is not", "≠" }, { "=" },

	{ "!", "not" }, { "null" }, { "true" }, { "false" }, { "/*" },
	{ "*/" }, { "(" }, { ")" }, { "[" }, { "]" }, { "{" }, { "}" },

	{ "<=>", "<->" },

	{ "@" },

	{ "===" }, { "==", "is" }, { "!==" },
	{ "+" }, { "-" }, { "*", "×" }, { "/", "÷" },

	{ "++" }, { "--" },

	{ "+=" }, {"-=" }, { "/=", "÷=" }, { "*=", "×=" },

	{ ";" },

	{ ">" }, {	">=", "≥" }, { "<" }, { "<=", "≤" },

	{ "->", "=>", "→" }, { "," }, { ":" }, { "if" }, { "else" }, { "match" }, { "for" }, { "in" },
	{ "while" }, { "do" }, { "?" },

	{ "or", "||" }, { "and", "&&" }, { "xor" },

	{ "." }, { "|" },

	{ "**" }, { "**=" }, { "%=" }, {"%" },

	{ "class" }, { "new" }, { "parent" }, { "then" },
	{ "end" }, { "?:" }, { "!!" },

	{ "~" }, { "~=" }, { "~~" }, { "~~=" },

	{ "π" },

	{ ".." }, { "..." },

	{ "instanceof" },

	{ "&" }, { "&=" },
	{ "|=" },
	{ "^" }, { "^=" },
	{ "<<" }, { "<<=" },
	{ ">>" }, { ">>=" },
	{ ">>>" }, { ">>>=" },
	{ "^<" }, { "^<=" },
	{ "^>" }, { "^>=" },

	{ "??" },
	{ "\\" }
};

LexicalAnalyser::LexicalAnalyser() {}

LetterType LexicalAnalyser::getLetterType(char32_t c) {

	if (c == '\'') {
		return LetterType::QUOTE;
	}
	if (c == '"') {
		return LetterType::DOUBLE_QUOTE;
	}
	if (c >= '0' && c <= '9') {
		return LetterType::NUMBER;
	}
	if (c == ' ' || c == '	' || c == '\n' || c == 0x00A0 /* unbreakable space */) {
		return LetterType::WHITE;
	}
	if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c >= 0xc0) {
		return LetterType::LETTER;
	}

	return LetterType::OTHER;
}

TokenType LexicalAnalyser::getTokenType(string word, TokenType by_default) {
	for (size_t j = 0; j < type_literals.size(); ++j) {
		for (string text : type_literals[j]) {
			if (word == text) return (TokenType) j;
		}
	}
	return by_default;
}

vector<Token> LexicalAnalyser::analyse(std::string code) {

	vector<Token> tokens = LexicalAnalyser::parseTokens(code + " ");

	tokens.push_back(Token(TokenType::FINISHED, 0, 1, ""));

	for (size_t i = 0; i < tokens.size(); ++i) {
		if (i + 1 < tokens.size() && tokens[i].content == "is" && tokens[i + 1].content == "not") {
			tokens[i].type = TokenType::DIFFERENT;
			tokens[i].content = "is not";
			tokens.erase(tokens.begin() + i + 1);
		}
	}

	return tokens;
}

vector<Token> LexicalAnalyser::parseTokens(string code) {

	char buff[5];
	const char* string_chars = code.c_str();

	vector<Token> tokens;

	int line = 1;
	int character = 1;
	string word = "";
	bool ident = false;
	bool number = false;
	bool string1 = false;
	bool string2 = false;
	bool other = false;
	bool escape = false;
	bool hex = false;
	bool bin = false;
	int comment = 0;
	bool lineComment = false;

	int i = 0;
	int l = strlen(string_chars);

	while (i < l) {

		character++;

		u_int32_t c = u8_nextchar(string_chars, &i);
		int oldi = i;
		u_int32_t nc = u8_nextchar(string_chars, &i);
		i = oldi;

		LetterType type = getLetterType(c);

		if (lineComment) {
			if (c == '\n') {
				lineComment = false;
			}
		} else {

			if (c == '/' && nc == '/') {
				lineComment = true;
			} else if (c == '/' && nc == '*') {
				comment++;
				i++;
			} else if (c == '*' && nc == '/') {
				comment--;
				i++;

			} else if (comment == 0) {

				if (type == LetterType::WHITE) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
					} else if (number) {
						if ((bin || hex) && word.size() == 2) {
							errors.push_back({LexicalError::Type::NUMBER_INVALID_REPRESENTATION, line, character});
						}
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = bin = hex = false;
					} else if (string1 || string2) {
						if (escape) {
							escape = false;
							errors.push_back({LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE, line, character});
						}
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
					}

				} else if (type == LetterType::LETTER) {

					if (ident || string1 || string2) {
						if (escape) {
							escape = false;
							if (c == 'b') {
								word += '\b';
							} else if (c == 'f') {
								word += '\f';
							} else if (c == 'n') {
								word += '\n';
							} else if (c == 'r') {
								word += '\r';
							} else if (c == 't') {
								word += '\t';
							} else {
								errors.push_back({LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE, line, character});
							}
						} else {
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						}
					} else if (number) {
						if (word == "0" && (c == 'x' || c == 'b')) {
							hex = c == 'x';
							bin = c == 'b';
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						} else if (hex && (c <= 'F' || (c >= 'a' && c <= 'f'))) {
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						} else if (c == 'l' or c == 'L') {
							word += "l";
							tokens.push_back(Token(TokenType::NUMBER, line, character, word));
							number = bin = hex = false;
							word = "";
						} else if (c == 'm' or c == 'M') {
							word += "m";
							tokens.push_back(Token(TokenType::NUMBER, line, character, word));
							number = bin = hex = false;
							word = "";
						} else {
							errors.push_back({LexicalError::Type::NUMBER_INVALID_REPRESENTATION, line, character});
							tokens.push_back(Token(TokenType::NUMBER, line, character, word));
							number = bin = hex = false;
						}
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						ident = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					} else {
						ident = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					}

				} else if (type == LetterType::NUMBER) {

					if (number) {
						if (bin && c > '1') {
							errors.push_back({LexicalError::Type::NUMBER_INVALID_REPRESENTATION, line, character});
						} else {
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						}
					} else if (ident || string1 || string2) {
						if (escape) {
							escape = false;
							errors.push_back({LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE, line, character});
						}
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						number = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					} else {
						number = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					}

				} else if (type == LetterType::QUOTE) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
						string1 = true;
						word = "";
					} else if (number) {
						if ((bin || hex) && word.size() == 2) {
							errors.push_back({LexicalError::Type::NUMBER_INVALID_REPRESENTATION, line, character});
						}
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = bin = hex = false;
						string1 = true;
						word = "";
					} else if (string2 || (string1 && escape)) {
						escape = false;
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (string1) {
						if (escape) {
							escape = false;
							errors.push_back({LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE, line, character});
						}
						tokens.push_back(Token(TokenType::STRING, line, character, word));
						string1 = false;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						string1 = true;
						word = "";
					} else {
						string1 = true;
						word = "";
					}

				} else if (type == LetterType::DOUBLE_QUOTE) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
						string2 = true;
						word = "";
					} else if (number) {
						if ((bin || hex) && word.size() == 2) {
							errors.push_back({LexicalError::Type::NUMBER_INVALID_REPRESENTATION, line, character});
						}
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = bin = hex = false;
						string2 = true;
						word = "";
					} else if (string1 || (string2 && escape)) {
						escape = false;
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (string2) {
						if (escape) {
							escape = false;
							errors.push_back({LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE, line, character});
						}
						tokens.push_back(Token(TokenType::STRING, line, character, word));
						string2 = false;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						string2 = true;
						word = "";
					} else {
						string2 = true;
						word = "";
					}

				} else if (type == LetterType::OTHER) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
						other = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					} else if (number) {
						if (!hex && !bin && c == '.' && word.find('.') == string::npos && getLetterType(nc) == LetterType::NUMBER) {
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						} else {
							if ((bin || hex) && word.size() == 2) {
								errors.push_back({LexicalError::Type::NUMBER_INVALID_REPRESENTATION, line, character});
							}
							tokens.push_back(Token(TokenType::NUMBER, line, character, word));
							number = bin = hex = false;
							other = true;
							u8_toutf8(buff, 5, &c, 1);
							word = buff;
						}
					} else if (string1 || string2) {
						if (escape && c != '\\') {
							escape = false;
							errors.push_back({LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE, line, character});
						}
						if (!escape && c == '\\') {
							escape = true;
						} else {
							escape = false;
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						}
					} else if (other) {
						bool is_longer = false;
						for (size_t j = 0; j < type_literals.size() && !is_longer; ++j) {
							for (string text : type_literals[j]) {
								if (text.size() > word.size() && text.compare(0, word.size(), word) == 0 && text[word.size()] == (char) c) {
									is_longer = true;
									break;
								}
							}
						}
						if (!is_longer) {
							tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
							u8_toutf8(buff, 5, &c, 1);
							word = buff;
						} else {
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						}
					} else {
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
						other = true;
					}
				}
			}
		}
		if (c == '\n') {
			line++;
			character = 0;
		}
	}

	if (string1 or string2) {
		errors.push_back({LexicalError::Type::UNTERMINATED_STRING, line, character});
	}

	return tokens;
}

}
