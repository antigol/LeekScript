#include "Test.hpp"

void Test::test_strings() {

	header("Strings");
	code("'").lexical_error(ls::LexicalError::Type::UNTERMINATED_STRING);
	code("\"").lexical_error(ls::LexicalError::Type::UNTERMINATED_STRING);
	code("'hello world").lexical_error(ls::LexicalError::Type::UNTERMINATED_STRING);

	section("Escape sequences");
	code("'\\\\'").equals("'\\'");
	code("\"\\\\\"").equals("'\\'");
	code("'\\\"'").equals("'\\\"'");
	code("\"\\\"\"").equals("'\"'");
	code("'\\''").equals("'''");
	code("\"\\\"\"").equals("'\"'");
	code("'aujourd\\'hui'").equals("'aujourd'hui'");
	code("\"aujourd\\\"hui\"").equals("'aujourd\"hui'");
	code("\"\\t\"").equals("'	'");
	code("'\\t'").equals("'	'");
	code("'yolo\\b'").equals("'yolo'");
	code("'yolo\\b'.size()").equals("5");
	code("'yolo\\byolo'").equals("'yoloyolo'");
	code("'\\n'").equals("'\n'");
	code("\"\\n\"").equals("'\n'");
	code("'salut\\nça va ?'").equals("'salut\nça va ?'");
	code("\"salut\\nça va ?\"").equals("'salut\nça va ?'");
	code("'\\f'").equals("''");
	code("\"\\f\"").equals("''");
	code("'yo\\flo'").equals("'yolo'");
	code("'\\r'").equals("''");
	code("\"\\r\"").equals("''");
	code("'salut\\rhello'").equals("'saluthello'");
	code("'\\y'").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);
	code("'\\A'").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);
	code("'\\ '").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);
	code("'\\	'").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);
	code("'\\2'").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);
	code("'\\-'").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);
	code("'\\*'").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);
	code("'\\#'").lexical_error(ls::LexicalError::Type::UNKNOWN_ESCAPE_SEQUENCE);

	/*
	 * Operators
	 */
	section("String.operator !");
	code("!'hello'").equals("false");

	section("String.operator -");
	code("-'hello'").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("String.operator x++");
	code("'hello'++").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_LVALUE, {"'hello'"});
	code("var a = 'hello' a++").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("String.operator x--");
	code("var a = 'hello' a--").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("String.operator +");
	code("'salut ' + 'ça va ?'").equals("'salut ça va ?'");
	code("'salut' + 12").equals("'salut12'");
	code("'salut' + true").equals("'saluttrue'");
	code("'salut' + null").equals("'salutnull'");
	code("'salut' + 12.8").equals("'salut12.8'");

	section("String.operator *");
	code("'salut' * 3").equals("'salutsalutsalut'");
	code("'salut' * (1 + 2)").equals("'salutsalutsalut'");
	code("('salut' * 1) + 2").equals("'salut2'");
	code("'hello' * 'abc'").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("String.operator |x|");
	code("|'salut'|").equals("5");

	section("String.operator /");
	code("'azerty' / ''").equals("['a', 'z', 'e', 'r', 't', 'y']");
	code("'abc' / '.'").equals("['abc']");
	code("'ab.c' / '.'").equals("['ab', 'c']");
	code("'.ab.c' / '.'").equals("['', 'ab', 'c']");
	code("'abc.' / '.'").equals("['abc', '']");
	code("'.aaaaa.bbbb.ccc.dd.e.' / '.'").equals("['', 'aaaaa', 'bbbb', 'ccc', 'dd', 'e', '']");
	code("('hello.world.how.are.you' / '.').size()").equals("5");
	code("'hello' / 2").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("String.operator \\");
	code("'azerty' \\ ''").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("String.operator \\=");
	code("var a = 'azerty' a \\= ''").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("String.operator~");
	code("~'bonjour'").equals("'ruojnob'");

	section("String.operator []");
	code("'bonjour'[3]").equals("'j'");
	code("'bonjour'['hello']").semantic_error(ls::SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, {"'hello'", "'bonjour'", ls::Type::STRING_TMP.to_string()});
	code("~('salut' + ' ca va ?')").equals("'? av ac tulas'");
	code("'bonjour'[2:5]").equals("'njou'");
	code("'bonjour'['a':5]").semantic_error(ls::SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, {"<key 1>"});
	code("'bonjour'[2:'b']").semantic_error(ls::SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, {"<key 2>"});
	code("'bonjour'['a':'b']").semantic_error(ls::SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, {"<key 1>"});
	code("let a = ['bonjour', 2][0] a[3]").equals("'j'");
	code("'hello'[-1]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("''[0]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("'yoyo'[1000]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);

	section("String.operator ==");
	code("'test' == 'etst'").equals("false");
	code("'test' == 'test'").equals("true");

	section("String.operator <");
	code("'aaaa' < 'aaba'").equals("true");
	code("'aaab' < 'aaaa'").equals("false");
	code("'test' < 'test'").equals("false");

	section("Unicode");
	code("'韭'").equals("'韭'");
	code("'♫☯🐖👽'").equals("'♫☯🐖👽'");
	code("'a♫b☯c🐖d👽'").equals("'a♫b☯c🐖d👽'");
	code("var hello = '你好，世界' hello").equals("'你好，世界'");
	code("'♫☯🐖👽'[3]").equals("'👽'");
	code("'韭' + '♫'").equals("'韭♫'");
	code("|'♫👽'|").equals("2");
	code("'♫👽'.size()").equals("2");
	code("'☣🦆🧀𑚉𒒫𑓇𐏊'.size()").equals("7");
	code("'௵௵a௵௵' / 'a'").equals("['௵௵', '௵௵']");
	code("'a☂a' / '☂'").equals("['a', 'a']");
	code("~'∑∬∰∜∷⋙∳⌘⊛'").equals("'⊛⌘∳⋙∷∜∰∬∑'");
	code("'ↂↂ' × 3").equals("'ↂↂↂↂↂↂ'");
	code("'ḀḂḈḊḖḞḠḦḮḰḸḾṊṎṖ'[5:9]").equals("'ḞḠḦḮḰ'");

	/*
	 * Iteration
	 */
	section("String iteration");
	code("for c in 'bonjour' { System.print(c) }").output("b\no\nn\nj\no\nu\nr\n");
	code("for (c in '汉堡 漢堡') { System.print(c) }").output("汉\n堡\n \n漢\n堡\n");
	code("[for c in 'salut' { c }]").equals("['s', 'a', 'l', 'u', 't']");
	code("[for c in 'salut' { (c.code() + 2).char() }]").equals("['u', 'c', 'n', 'w', 'v']");
	code("let a = [for c in 'salut' { (c.code() + 2).char() }] a.join('')").equals("'ucnwv'");
	code("var r = '' for k : c in 'azerty' { r += k + '_' + c + '_' } r").equals("'0_a_1_z_2_e_3_r_4_t_5_y_'");

	/*
	 * String standard library
	 */
	header("String standard library");
	code("String").equals("<class String>");
	code("String()").equals("''");
	code("new String").equals("''");
	code("new String()").equals("''");
	code("new String('salut')").equals("'salut'");
	code("String()").equals("''");
	code("String('yo')").equals("'yo'");
	code("['hello'.size(), 'str']").equals("[5, 'str']");

	section("String.size()");
	code("String.size('salut')").equals("5");
	code("'a'.size == 'b'.size").equals("true");
	code("[1, 'salut tout le monde'][1].size()").equals("19");

	section("String.toUpper()");
	code("String.toUpper('')").equals("''");
	code("String.toUpper('salut')").equals("'SALUT'");
	code("String.toUpper([1, 'yolo'][1])").equals("'YOLO'");
	code("String.toUpper([1, 'yolo'][0])").exception(ls::vm::Exception::WRONG_ARGUMENT_TYPE);

	section("String.toLower()");
	code("String.toLower('')").equals("''");
	code("String.toLower('SALUT')").equals("'salut'");

	section("String.length()");
	code("String.length('salut')").equals("5");

	section("String.reverse()");
	code("String.reverse('salut')").equals("'tulas'");

	section("String.replace()");
	code("String.replace('bonjour à tous', 'o', '_')").equals("'b_nj_ur à t_us'");

	section("String.map()");
	code("String.map('salut', x -> '(' + x + ')')").equals("'(s)(a)(l)(u)(t)'");
	code("'salut'.map(char -> char + '.')").equals("'s.a.l.u.t.'");
	code("'♫☯🐖👽韭'.map(u -> u + ' ')").equals("'♫ ☯ 🐖 👽 韭 '");
	// TODO
	// code("let f = n -> n.string().map(d -> (d.code() + 9263).char() + ' ') f(196457238)").equals("'① ⑨ ⑥ ④ ⑤ ⑦ ② ③ ⑧ '");

	section("String.split()");
	code("String.split('bonjour ça va', ' ')").equals("['bonjour', 'ça', 'va']");
	code("String.split('bonjour_*_ça_*_va', '_*_')").equals("['bonjour', 'ça', 'va']");
	code("String.split('salut', '')").equals("['s', 'a', 'l', 'u', 't']");

	section("String.startsWith()");
	code("String.startsWith('salut ça va', 'salut')").equals("true");
	code("String.startsWith('bonjour', 'bonjour ça va')").equals("false");

	section("String.toArray()");
	code("String.toArray('salut')").equals("['s', 'a', 'l', 'u', 't']");

	section("String.charAt()");
	code("String.charAt('salut', 1)").equals("'a'");

	section("String.substring()");
	code("'salut'.substring(3, 4)").equals("'ut'");

	section("String.endsWith()");
	code("'salut'.endsWith('lut')").equals("true");
	code("'hello world how are you? ♫'.endsWith('are you? ♫')").equals("true");
	code("'bonjour'.endsWith('soir')").equals("false");
	code("String.endsWith('salut', 'lut')").equals("true");
	code("String.endsWith('aaaaaabbbb', 'abc')").equals("false");
	code("String.endsWith('', '')").equals("true");
	code("String.endsWith('too', 'too long')").equals("false");

	section("String.indexOf()");
	code("'bonjour'.indexOf('jour')").equals("3");
	code("String.indexOf('azerty', 'zer')").equals("1");
	code("String.indexOf('bonjour tout le monde', 'not found')").equals("-1");
	code("String.indexOf('bonjour', 'needle too long')").equals("-1");

	section("String.code()");
	code("'A'.code()").equals("65");
	code("'ABC'.code(2)").equals("67");
	code("'©'.code()").equals("169");
	code("'é'.code()").equals("233");
	code("'♫'.code()").equals("9835");
	code("'🐨'.code()").equals("128040");
	code("String.code('🐨')").equals("128040");
	code("String.code('ABC', 2)").equals("67");
	code("(x -> x)(65).char()").equals("'A'");
	code("[128040][0].char()").equals("'🐨'");
	// TODO
	// code("'hello'.map(x -> { let b = x == ' ' if b then ' ' else x.code() - 'a'.code() + 1 + ' ' end })").equals("'8 5 12 12 15 '");
	// code("'hello'.map(x -> { if x == ' ' then ' ' else x.code() - 'a'.code() + 1 + ' ' end })").equals("'8 5 12 12 15 '");
	code("[String.code('♫'), '']").equals("[9835, '']");

	section("String.number()");
	code("String.number('1234567')").equals("1234567");
	code("String.number('1469215478186644')").equals("1469215478186644");
	code("'1234567'.number()").equals("1234567");
	code("'1469215478186644'.number()").equals("1469215478186644");

	section("String.is_permutation()");
	code("''.isPermutation('')").equals("true");
	code("'A'.isPermutation('A')").equals("true");
	code("'A'.isPermutation('a')").equals("false");
	code("'abcde'.isPermutation('abcde')").equals("true");
	code("'abcde'.isPermutation('beacd')").equals("true");
	code("'abcde'.isPermutation('beaca')").equals("false");
	code("'aaa'.isPermutation('aaaa')").equals("false");

	section("v1 string charAt");
	code_v1("charAt('hello', 3)").equals("'l'");

	section("v1 string replace()");
	code_v1("replace('bonjour', 'o', 'u')").equals("'bunjuur'");
	code_v1("replace('hello\\\\', '\\\\\\\\', '\\\\\\\\o')").equals("'hello\\o'");
}
