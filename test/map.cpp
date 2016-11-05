#include "Test.hpp"

void Test::test_map() {

	header("Map");

	section("Constructor");
	code("[:]").equals("[:]");
	code("[1 : 1 2 : 2]").equals("[1 : 1 2 : 2]");
	code("[1 : 1 2 : '2']").equals("[1 : 1 2 : '2']");
	code("['1' : '1' '1' : '2' '1' : '3']").equals("['1' : '1']");

	section("Map.size()");
	code("let x = [1 : 1 1 : 2 1 : 3] x.size()").equals("1");
	code("let x = [1 : 1 1 : 2 2 : '3'] x.size()").equals("2");

	section("Map.insert()");
	code("let x = [1 : 1] x.insert(2, 2)").equals("true");
	code("let x = ['a' : 'a'] x.insert(2, 2) x").equals("[2 : 2 'a' : 'a']");
	code("let x = [1 : 'a'] x.insert(2, 3) x").equals("[1 : 'a' 2 : 3]");
	code("let x = ['a' : 1] x.insert(2, 3) x").equals("[2 : 3 'a' : 1]");
	code("let x = ['a' : 1] x.insert('a', 3)").equals("false");

	section("Map.clear()");
	code("let x = [1 : 1] x.clear()").equals("[:]");
	code("let x = ['a' : 'a'] x.clear()").equals("[:]");

	section("Map.erase()");
	code("let x = [1 : 1] x.erase(1)").equals("true");
	code("let x = ['a' : 'a'] x.erase('a') x").equals("[:]");
	code("let x = ['a' : 'a'] x.erase('b') x").equals("['a' : 'a']");
	code("let x = ['a' : 1] x.erase(3.14) x").equals("['a' : 1]");

	section("Map.look()");
	code("let x = [1 : 1] x.look(1,0)").equals("1");
	code("let x = ['a' : 'a'] x.look('a','b')").equals("'a'");
	code("let x = ['a' : 'a'] x.look('b','b')").equals("'b'");
	code("let x = ['a' : 1] x.look(3.14,'a')").semantic_error( ls::SemanticError::METHOD_NOT_FOUND, ls::Type::PTR_INT_MAP.toString() + ".look(" + ls::Type::REAL.toString() + ", " + ls::Type::STRING.toString() + ")");

	section("Map.operator ==");
	code("['a':'b'] == [1:1]").equals("false");
	//code("let x = ['a' : 'b'] let y = [1 : 1] x.clear() == y.clear()").equals("true");

	section("Map.operator <");
	//code("['a':1 'b':2] < ['a':1 'b':3]").equals("true");
	code("[1:1 2:0] < [1:1 2:true]").equals("false");

	section("Map.operator in");
	code("let m = ['salut': 12] 'salut' in m").equals("true");
	code("let m = ['salut': 12] 'salum' in m").equals("false");
	code("let m = ['salut': 12] 12 in m.values()").equals("true");

	section("Map.operator []");
	code("let m = [5: 12] m[5]").equals("12");
	code("let m = [5: 12.5] m[5]").equals("12.5");
	code("let m = [5.5: 12] m[5.5]").equals("12");
	code("let m = [5.5: 12.5] m[5.5]").equals("12.5");
	code("let m = ['salut': 12] m['salut']").equals("12");
	code("let m = ['salut': 12.5] m['salut']").equals("12.5");
	code("let m = ['salut': 'yolo'] m['salut']").equals("'yolo'");

	section("Map.operator [] left-value");
	code("let m = ['salut': 12] m['salut'] = 13 m['salut']").equals("13");

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
