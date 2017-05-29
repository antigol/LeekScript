#include "Test.hpp"

void Test::test_map() {

	header("Map");

	section("Constructor");
	code("[:]").equals("[:]");
	code("[1: 1, 2: 2]").equals("[1: 1, 2: 2]");
	code("[1: 1, 2: '2']").equals("[1: 1, 2: '2']");
	code("['1': '1', '1': '2', '1': '3']").equals("['1': '1']");

	section("Map::to_bool()");
	code("![:]").equals("true");
	code("![2: 2]").equals("false");
	code("if ([2: 2]) { 12 } else { 5 }").equals("12");

	section("Array of maps");
	code("[[:], [1: 1], [1: 2]]").equals("[[:], [1: 1], [1: 2]]");
	code("let m = ['a': 'b'] [m]").equals("[['a': 'b']]");

	/*
	 * Operators
	 */
	std::vector<std::string> maps = {"[5: 5]", "[5: 9.99]", "[5: 'abc']", "[9.99: 5]", "[9.99: 9.99]", "[9.99: 'abc']", "['abc': 5]", "['abc': 9.99]", "['abc': 'abc']"};

	section("Map.operator ==");
	code("['a': 'b'] == [1: 1]").equals("false");
	code("['a': 'b'] == ['a': 'b']").equals("true");
	code("['a': 'b'] == ['a': 'b', 'c': 'd']").equals("false");
	code("let x = ['a' : 'b'] let y = [1 : 1] x.clear() == y.clear()").equals("true");
	for (auto& m1 : maps)
		for (auto& m2 : maps)
			code(m1 + " == " + m2).equals(m1 == m2 ? "true" : "false");
	code("[12.5: 9.99] == 'hello'").equals("false");

	section("Map.operator <");
	code("['a': 1, 'b': 2] < ['a': 1, 'b': 3]").equals("true");
	code("[1: 1, 2: 0] < [1: 1, 2: true]").equals("false");
	code("[1: 1, 2: 0.5] < [1: 1, 2: true]").equals("false");
	code("[1: 1, 2: 'a'] < [1: 1.5, 2: 5.5]").equals("true");
	for (size_t i = 0; i < maps.size(); ++i)
		for (size_t j = 0; j < maps.size(); ++j)
			code(maps[i] + " < " + maps[j]).equals(i < j ? "true" : "false");

	section("Map.operator in");
	code("let m = ['salut': 12] 'salut' in m").equals("true");
	code("let m = ['salut': 12] 'salum' in m").equals("false");
	code("let m = ['salut': 12] 12 in m.values()").equals("true");

	section("Map.operator []");
	code("let m = [1: 1] m[1]").equals("1");
	code("let m = ['a': 'b'] m['a']").equals("'b'");
	code("let m = [5: 12] m[5]").equals("12");
	code("let m = [5: 12.5] m[5]").equals("12.5");
	code("let m = [5.5: 12] m[5.5]").equals("12");
	code("let m = [5.5: 12.5] m[5.5]").equals("12.5");
	code("let m = ['salut': 12] m['salut']").equals("12");
	code("let m = ['salut': 12.5] m['salut']").equals("12.5");
	code("let m = ['salut': 'yolo'] m['salut']").equals("'yolo'");
	code("let m = ['a': 'b'] m['a'] = 'c' m").equals("['a': 'c']");
	code("let m = ['salut': 12] m['salut'] = 13 m['salut']").equals("13");
	code("let m = ['salut': 'yo'] m['salut'] = 'ya' m['salut']").equals("'ya'");
	code("let m = [5: 12] m[5.7]").equals("12");
	code("let m = [5: 12] m['salut']").semantic_error(ls::SemanticError::INVALID_MAP_KEY, {"'salut'", "m", ls::Type::STRING_TMP.to_string()});
	code("let m = [5.7: 'hello'] m['salut']").semantic_error(ls::SemanticError::INVALID_MAP_KEY, {"'salut'", "m", ls::Type::STRING_TMP.to_string()});
	code("var m = [1: 'a', 2: 'b'] m[2] = 'B' m").equals("[1: 'a', 2: 'B']");
	code("var m = [1: 'a', 2: 'b'] m[3]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("var m = [1: 2, 3: 4] m[5] = 6 m").equals("[1: 2, 3: 4, 5: 6]");
	code("var m = ['a': 2, 'b': 4] m['c'] = 6 m").equals("['a': 2, 'b': 4, 'c': 6]");
	code("var m = ['a': 2.5, 'b': 4.8] m['c'] = 6.9 m").equals("['a': 2.5, 'b': 4.8, 'c': 6.9]");
	code("var m = [1: 'a', 2: 'b'] m[3] = 'c' m").equals("[1: 'a', 2: 'b', 3: 'c']");
	code("var m = ['a': '2', 'b': '4'] m['c'] = '6' m").equals("['a': '2', 'b': '4', 'c': '6']");
	code("var m = [1: 2, 3: 4] m[3] = 6 m").equals("[1: 2, 3: 6]");
	code("var m = [1: 2.5, 3: 4.5] m[3] = 6.5 m").equals("[1: 2.5, 3: 6.5]");
	code("var m = [1.5: 2, 3.5: 4] m[3.5] = 6 m").equals("[1.5: 2, 3.5: 6]");
	code("var m = [1.5: 2.5, 3.5: 4.5] m[3.5] = 6.5 m").equals("[1.5: 2.5, 3.5: 6.5]");
	code("var m = ['1': 2, '3': 4] m['3'] = 6 m").equals("['1': 2, '3': 6]");
	code("var m = [1.5: 'a', 2.5: 'b'] m[2.5] = 'c' m").equals("[1.5: 'a', 2.5: 'c']");
	code("['', [1: 2][1]]").equals("['', 2]");
	code("['', [1: 2.5][1]]").equals("['', 2.5]");

	section("Map.operator [] on unknown maps");
	code("var m = ptr(['a': '2']) m['c'] = '6' m").equals("['a': '2', 'c': '6']");
	code("var m = ptr([2: 'a']) m[3] = 'b' m").equals("[2: 'a', 3: 'b']");
	code("var m = ptr([2.5: 'a']) m[3.6] = 'b' m").equals("[2.5: 'a', 3.6: 'b']");
	code("var m = ptr(['a': 2]) m['c'] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr(['a': 2.2]) m['c'] = 6.6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2: 2]) m[3] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2.5: 2]) m[3.5] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2.5: 2.8]) m[3.5] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2: 2.8]) m[3] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2: 'a']) m['toto'] = 'b' m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2.5: 'a']) m['toto'] = 'b' m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	/*
	 * Methods
	 */
	section("Map.size()");
	code("let x = [1 : 1 1 : 2 1 : 3] x.size()").equals("1");
	code("let x = [1 : 1 1 : 2 2 : '3'] x.size()").equals("2");

	section("Map.insert()");
	code("let x = [1 : 1] x.insert(2, 2)").equals("true");
	code("let x = ['a' : 'a'] x.insert(2, 2) x").equals("[2: 2, 'a': 'a']");
	code("let x = [1 : 'a'] x.insert(2, 3) x").equals("[1: 'a', 2: 3]");
	code("let x = ['a' : 1] x.insert(2, 3) x").equals("[2: 3, 'a': 1]");
	code("let x = ['a' : 1] x.insert('a', 3)").equals("false");

	section("Map.clear()");
	code("let x = [:] x.clear()").equals("[:]");
	code("let x = [1: 1] x.clear()").equals("[:]");
	code("let x = [1: 'a'] x.clear()").equals("[:]");
	code("let x = ['a': 1] x.clear()").equals("[:]");
	code("let x = ['a': 'a'] x.clear()").equals("[:]");
	code("let x = ['a': 'a', 'b': 'b'] x.clear()").equals("[:]");

	section("Map.erase()");
	code("let x = [1 : 1] x.erase(1)").equals("true");
	code("let x = ['a' : 'a'] x.erase('a') x").equals("[:]");
	code("let x = ['a' : 'a'] x.erase('b') x").equals("['a': 'a']");
	code("let x = ['a' : 1] x.erase(3.14) x").equals("['a': 1]");

	section("Map.look()");
	code("let x = [1: 1] x.look(1, 0)").equals("1");
	code("let x = ['a': 'a'] x.look('a', 'b')").equals("'a'");
	code("let x = ['a': 'a'] x.look('b', 'b')").equals("'b'");
	code("let x = ['a': 1] x.look(3.14, 'a')").semantic_error( ls::SemanticError::METHOD_NOT_FOUND, {ls::Type::PTR_INT_MAP.to_string() + ".look(" + ls::Type::REAL.to_string() + ", " + ls::Type::STRING_TMP.to_string() + ")"});
	code("[1 : 1].look(1, 0)").equals("1");

	section("Map.values()");
	code("let m = [5: 1, 7: 2, -21: 3] m.values()").equals("[3, 1, 2]");
	code("let m = [5: 1.5, 7: 2.5, -21: 3.5] m.values()").equals("[3.5, 1.5, 2.5]");
	code("let m = [5: [], 7: 'str', -21: true] m.values()").equals("[true, [], 'str']");
	code("[5.5: 10, 7.2: 100, -21.95: 1000].values()").equals("[1000, 10, 100]");
	code("[5.1: 12.1, -7.6: 0, 14.88: -0xfe5a].values()").equals("[0, 12.1, -65114]");
	code("[-1.111: ':)', 6.6: ':/', 9: ':D'].values()").equals("[':)', ':/', ':D']");
	code("['yolo': 3, false: 1, 12: 4].values()").equals("[1, 4, 3]");
	code("[[]: 3.7, [1: 2]: 1.3, {x: 12}: 4.8].values()").equals("[3.7, 1.3, 4.8]");
	code("[(x, y -> x + y): (x, y -> x - y), null: 'null', <'a', 'b'>: 0].values()").equals("['null', 0, <function>]");
}
