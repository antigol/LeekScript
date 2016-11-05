#include "Test.hpp"

void Test::test_strings() {

	header("Strings");

	// General
	code("'").lexical_error(ls::LexicalError::Type::UNTERMINATED_STRING);
	code("\"").lexical_error(ls::LexicalError::Type::UNTERMINATED_STRING);
	code("'hello world").lexical_error(ls::LexicalError::Type::UNTERMINATED_STRING);

//	code("'\\''").equals("'''");
	code("\"\\\"\"").equals("'\"'");
//	code("'aujourd\\'hui'").equals("'aujourd'hui'");
	code("\"aujourd\\\"hui\"").equals("'aujourd\"hui'");

	code("'salut ' + 'ça va ?'").equals("'salut ça va ?'");
	code("'salut' + 12").equals("'salut12'");
	code("'salut' + true").equals("'saluttrue'");
	code("'salut' + null").equals("'salutnull'");
	code("'salut' * 3").equals("'salutsalutsalut'");
	code("|'salut'|").equals("5");
	code("'abc' / '.'").equals("['abc']");
	code("'ab.c' / '.'").equals("['ab', 'c']");
	code("'.ab.c' / '.'").equals("['', 'ab', 'c']");
	code("'abc.' / '.'").equals("['abc', '']");
	code("'.aaaaa.bbbb.ccc.dd.e.' / '.'").equals("['', 'aaaaa', 'bbbb', 'ccc', 'dd', 'e', '']");
	code("~'bonjour'").equals("'ruojnob'");
	code("'bonjour'[3]").equals("'j'");
	code("'bonjour'['hello']").semantic_error( ls::SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, "<key 1>");
	code("~('salut' + ' ca va ?')").equals("'? av ac tulas'");
	code("'bonjour'[2:5]").equals("'njou'");
	code("'bonjour'['a':5]").semantic_error( ls::SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, "<key 1>");
	code("'bonjour'[2:'b']").semantic_error( ls::SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, "<key 2>");
	code("'bonjour'['a':'b']").semantic_error(ls::SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, "<key 1>");
	code("'salut' * (1 + 2)").equals("'salutsalutsalut'");
	code("('salut' * 1) + 2").equals("'salut2'");
	code("('hello.world.how.are.you' / '.').size()").equals("5");
	code("'test' == 'etst'").equals("false");
	code("'test' == 'test'").equals("true");
	code("'aaaa' < 'aaba'").equals("true");
	code("'aaab' < 'aaaa'").equals("false");
	code("'test' < 'test'").equals("false");

	// Unicode
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

	// String standard library
	header("String standard library");
	code("String").equals("<class String>");
	code("String()").equals("''");
	code("new String").equals("''");
	code("new String()").equals("''");
	code("new String('salut')").equals("'salut'");
	code("String()").equals("''");
	code("String('yo')").equals("'yo'");
	code("String.size('salut')").equals("5");
	code("['hello'.size(), 'str']").equals("[5, 'str']");

	section("String.toUpper()");
	code("String.toUpper('')").equals("''");
	code("String.toUpper('salut')").equals("'SALUT'");
	//code("String.toUpper('ça va ?')").equals("'Ça va ?'");

	section("String.toLower()");
	code("String.toLower('')").equals("''");
	code("String.toLower('SALUT')").equals("'salut'");
	//code("String.toLower('ÇÀÉÈÔÖÛ')").equals("'çaéèôöû'");

	code("String.length('salut')").equals("5");
	code("String.reverse('salut')").equals("'tulas'");
	code("String.replace('bonjour à tous', 'o', '_')").equals("'b_nj_ur à t_us'");
	code("String.map('salut', x -> '(' + x + ')')").equals("'(s)(a)(l)(u)(t)'");
	code("'salut'.map(char -> char + '.')").equals("'s.a.l.u.t.'");
	code("'♫☯🐖👽韭'.map(u -> u + ' ')").equals("'♫ ☯ 🐖 👽 韭 '");
	code("String.split('bonjour ça va', ' ')").equals("['bonjour', 'ça', 'va']");
	code("String.split('bonjour_*_ça_*_va', '_*_')").equals("['bonjour', 'ça', 'va']");
	code("String.split('salut', '')").equals("['s', 'a', 'l', 'u', 't']");
	code("String.startsWith('salut ça va', 'salut')").equals("true");
	code("String.startsWith('bonjour', 'bonjour ça va')").equals("false");
	code("String.toArray('salut')").equals("['s', 'a', 'l', 'u', 't']");
	code("String.charAt('salut', 1)").equals("'a'");
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

	// Integer conversions
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
	code("'hello'.map(x -> { let b = x == ' ' if b then ' ' else x.code() - 'a'.code() + 1 + ' ' end })").equals("'8 5 12 12 15 '");
	code("'hello'.map(x -> { if x == ' ' then ' ' else x.code() - 'a'.code() + 1 + ' ' end })").equals("'8 5 12 12 15 '");

	code("String.number('1234567')").equals("1234567");
	code("String.number('1469215478186644')").equals("1469215478186644");
	code("'1234567'.number()").equals("1234567");
	code("'1469215478186644'.number()").equals("1469215478186644");
}
