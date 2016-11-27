#include "Test.hpp"

void Test::test_arrays() {

	header("Arrays");
	code("[]").equals("[]");
	code("[1]").equals("[1]");
	code("[1, 2, 3]").equals("[1, 2, 3]");
	code("[1.21, -5, 4.55, 12, -6.7]").equals("[1.21, -5, 4.55, 12, -6.7]");
	code("[true, false, true]").equals("[true, false, true]");
	code("[23, true, '', {}, 123]").equals("[23, true, '', {}, 123]");
	code("[1, 2, 3] + [4, 5, 6]").equals("[1, 2, 3, 4, 5, 6]");
	code("[] + 1").equals("[1]");
	code("[] + 1 + 2 + 3").equals("[1, 2, 3]");
	code("[1] + 2").equals("[1, 2]");
	code("[1] + 0.5").equals("[1, 0.5]");
	code("[1] + 0.5 + 'a' + null").equals("[1, 0.5, 'a', null]");
	code("[1] + null").equals("[1, null]");
	code("[1] + true").equals("[1, true]");
	code("[1] + [2] + [3]").equals("[1, 2, 3]");
	code("[1] + [2.5] + ['a']").equals("[1, 2.5, 'a']");
	code("['a'] + [2.5] + [1]").equals("['a', 2.5, 1]");
	code("[1] + ['a']").equals("[1, 'a']");
	code("['a'] + [1]").equals("['a', 1]");
	code("let a = ['a'] a += 'b' a").equals("['a', 'b']");
	code("[1, 2, 3][1]").equals("2");
	code("let a = [1, 2, 3] a[0]").equals("1");
	code("let a = [1, 2, 3] a[0] = 5 a[0]").equals("5");
	code("let a = [1, 2, 3] a[0] += 5 a[0]").equals("6");
	code("let v = 12 let a = [v, 2, 3] a[0] += 5 a[0]").equals("17");
	code("let a = [23, 23, true, '', [], 123]; |a|").equals("6");
	code("let a = [1, 2, 3]; ~a").equals("[3, 2, 1]");
	code("let a = [] !a").equals("true");
	code("let a = [1, 2, 3] a[1] = 12 a").equals("[1, 12, 3]");
	code("[1.2, 321.42, 23.15]").equals("[1.2, 321.42, 23.15]");
	code("[1, 2, 3, 4, 5][1:3]").equals("[2, 3, 4]");
	code("2 in [1, 2, 3]").equals("true");
	code("4 in [1, 2, 3]").equals("false");
	code("'yo' in ['ya', 'yu', 'yo']").equals("true");
	code("let a = 2 if (a in [1, 2, 3]) { 'ok' } else { 'no' }").equals("'ok'");
	code("let a = [5, 'yolo', 12] a[1]").equals("'yolo'");

	section("No commas");
	code("[1 2 3]").equals("[1, 2, 3]");
	code("['yo' 'ya' 'yu']").equals("['yo', 'ya', 'yu']");
	code("[true false true true]").equals("[true, false, true, true]");
	code("[[1 2] [[3] 4 [5 6] []]]").equals("[[1, 2], [[3], 4, [5, 6], []]]");

	section("Type changes");
	code("let a = [1, 2, 3] a += 'hello' a").equals("[1, 2, 3, 'hello']");
	code("let a = [1.5] a += ['a', 'b'] a").equals("[1.5, 'a', 'b']");
	code("let a = [1.5] a += false a").equals("[1.5, false]");
	code("let a = [1] a += <'z', 'a'> a").equals("[1, 'a', 'z']");
	code("let a = [1] a += 'a' a").equals("[1, 'a']");

	section("Array.operator <");
	code("[1, 2, 3, 4] < [1, 2, 3, 5]").equals("true");
	code("[1, 2, 4, 4] < [1, 2, 3, 5]").equals("false");
	code("[1, 2, 3, 4.1] < [1, 2, 3, 5.1]").equals("true");
	code("[1, 2, 4, 4.1] < [1, 2, 3, 5.1]").equals("false");
	code("['1', '2', '3', '4'] < ['1', '2', '3', '5']").equals("true");
	code("['1', '2', '4', '4'] < ['1', '2', '3', '5']").equals("false");
	code("[1, 2, 3, 4] < [1, 2, 3, 5.1]").equals("true");
	code("[1, 2, 4, 4.1] < [1, 2, 3, 5]").equals("false");
	code("[1, 2, '3'] < [1, 2, 3]").equals("false");
	code("[1, 1, '3'] < [1, 2, 3]").equals("true");

	section("Array operations");
	code("[1, 2, 3, 4, 5] ~~ x -> x ** 2").equals("[1, 4, 9, 16, 25]");
	code("[1.5, 2.5, 3.5] ~~ x -> x.floor()").equals("[1, 2, 3]");
	code("[1, 2, 3, 4, 5] ~~ (x -> x ** 2)").equals("[1, 4, 9, 16, 25]");
	code("['yo', 'toto', 'salut'] ~~ x -> x + ' !'").equals("['yo !', 'toto !', 'salut !']");
	code("[1, 2, 3] ~~ x -> [x]").equals("[[1], [2], [3]]");
	code("[1, 2, 3] ~~ x -> 'yo'").equals("['yo', 'yo', 'yo']");
	code("let f = x -> x * 10 [1, 2, 3] ~~ f").equals("[10, 20, 30]");
	code("[1.2, 321.42] ~~ x -> x * 1.7").equals("[2.04, 546.414]");
	code("[1, 2, 3, 4, 5] ~~ x -> x.max(3)").equals("[3, 3, 3, 4, 5]");
	code("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10] ~~ x -> x.max(3).min(8)").equals("[3, 3, 3, 4, 5, 6, 7, 8, 8, 8]");
	code("[1] < [1, 2]").equals("true");
	code("[1, 1] < [1, 2]").equals("true");
	code("[1, 1] == [1, 2]").equals("false");
	code("[1, 2] == [1, 2]").equals("true");
	code("[1, '1'] == [1, '2']").equals("false");
	code("[1, '2'] == [1, '2']").equals("true");
	code("[1, 1] == [1, 2.5]").equals("false");
	code("[1, 2.5] == [1, 2.5]").equals("true");

//	let f=x->x[0]; [f([1]), f([0..3])]

	section("Constructor");
	code("Array").equals("<class Array>");
	code("Array()").equals("[]");
	code("new Array").equals("[]");
	code("new Array()").equals("[]");

	section("Array.size()");
	code("Array.size([1, 'yo', true])").equals("3");
	code("Array.size([6, 4, 3, 2, -11])").equals("5");
	code("[1, 'yo', true].size()").equals("3");
	code("[1, -2, 3, -12, -6].size()").equals("5");

	section("Array.average()");
	code("Array.average([1, 2, 3, 4, 5, 6])").equals("3.5");
	code("Array.average([])").equals("0");
	code("[1, 2, 3, 4, 5, 6].average()").equals("3.5");
	code("[].average()").equals("0");
	code("['salut', true, []].average()").equals("0");
	code("[10, -5.7, 30.89, 66].average()").almost(25.2975);

	section("Array.sum()");
	code("Array.sum([10, -5, 30, 66])").equals("101");
	code("[10, -5, 30, 66].sum()").equals("101");
	code("Array.sum(['ab', 'cd', 'ef'])").equals("'abcdef'");
	code("['ab', 'cd', 'ef'].sum()").equals("'abcdef'");
	code("['abc', true, 12, [1, 2]].sum()").equals("'abctrue12<array>'");
	code("[10, -5.7, 30.89, 66].sum()").almost(101.19);

	section("Array.map()");
	code("Array.map([1, 2, 3], x -> x ** 2)").equals("[1, 4, 9]");
	code("[3, 4, 5].map(x -> x ** 2)").equals("[9, 16, 25]");
	code("[321, 213, 121].map(x -> x ** 2).size()").equals("3");
	code("['a' 'b' 'c'].map(x -> x)").equals("['a', 'b', 'c']");
	code("[65 66 67].map(x -> x.char()).join('')").equals("'ABC'");

	section("Array.map2()");
	code("Array.map2([1, 'yo ', []], [12, 55, 9], (x, y -> x + y))").equals("[13, 'yo 55', [9]]");

	section("Array.chunk()");
	code("let x = [1, 2, 3, 4] x.chunk(2)").equals("[[1, 2], [3, 4]]");
	code("let x = [1, 2, 3, 4] x.chunk(3)").equals("[[1, 2, 3], [4]]");
	code("let x = [1, 2, 3, 4] x.chunk()").equals("[[1], [2], [3], [4]]");
	code("let x = [1.6, 2.5, 3.1, 4.67] x.chunk()").equals("[[1.6], [2.5], [3.1], [4.67]]");
	code("let x = ['hello', x -> x, true, Number] x.chunk()").equals("[['hello'], [<function>], [true], [<class Number>]]");

	section("Array.unique()");
	code("let x = [1, 1, 2, 2, 1] x.unique() x").equals("[1, 2, 1]");
	code("let x = [1, 1, 2, 2, 1] x.sort() x.unique() x").equals("[1, 2]");
	code("let x = ['a', 'a', 'b'] x.unique() x").equals("['a', 'b']");
	code("let x = ['a', 'b', 'c'] x.unique() x").equals("['a', 'b', 'c']");
	code("let x = ['a', 'a', 'b', 'a', 'a'] x.unique() x").equals("['a', 'b', 'a']");

	section("Array.sort()");
	code("let x = [3, 1, 2] x.sort() x").equals("[1, 2, 3]");
	code("let x = ['foo', 'yop', 'abc'] x.sort() x").equals("['abc', 'foo', 'yop']");
	code("let x = [[[]], [[], [], []], [[], []]]; x.sort() x").equals("[[[]], [[], []], [[], [], []]]");
	code("let x = [[1, 2, 3], [3, 1, 2], [2, 3, 1]]; x.sort() x").equals("[[1, 2, 3], [2, 3, 1], [3, 1, 2]]");

	section("Array.filter()");
	code("Array.filter([1, 2, 3, 10, true, 'yo'], x -> x > 2)").equals("['yo', 10, 3]");
	code("[3, 4, 5].filter(x -> x > 6)").equals("[]");
	code("[1, 2, 3, 4, 5, 6, 7].filter(x -> x % 2 == 0)").equals("[2, 4, 6]");

	section("Array.contains()");
	code("Array.contains([1, 2, 3, 10, 1], 1)").equals("true");
	code("[3, 4, 5].contains(6)").equals("false");

	section("Array.isEmpty()");
	code("Array.isEmpty([])").equals("true");
	code("[3, 4, 5].isEmpty()").equals("false");
	//code("let a = 0 Array.iter([1,2,3], x -> a += x) a").equals("6");
	//code("let a = 2 [1,2,3].iter(x -> a *= x) a").equals("12");

	section("Array.partition()");
	code("Array.partition([1, 2, 3, 4, 5], (x -> x < 3))").equals("[[1, 2], [3, 4, 5]]");
	code("Array.partition([1, 2, 3, 10, true, 'yo'], x -> x > 2)").equals("[[3, 10, 'yo'], [1, 2, true]]");
	code("[1, 2, 3, 4, 5].partition(x -> x > 3)").equals("[[4, 5], [1, 2, 3]]");
	code("[1, 2, 3, 4, 5].partition(x -> x == 3)").equals("[[3], [1, 2, 4, 5]]");
	code("[1, 2, 3, 4, 5, 6].filter(x -> x > 2).partition(x -> x > 4)").equals("[[5, 6], [3, 4]]");
	// code("[1, 2, 3, 4, 5].partition(x -> 'yolo')").equals("**error**");

	section("Array.first()");
	code("Array.first([1, 2, 3, 10, true, 'yo', null])").equals("1");
	code("['yo', 3, 4, 5].first()").equals("'yo'");
	code("Array.last([1, 2, 3, 10, true, 'yo', null])").equals("null");
	// code("['yo', 3, 4, 5].last()").equals("5");

	// TODO : the return type of first() must be the element type of the array if it's homogeneous
//	code("[[321, 21], [23, 212], [654, 9876]].first().last()").equals("21");

	section("Array.foldLeft()");
	code("Array.foldLeft([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')").equals("'concat:12310trueyonull'");
	// TODO more tests
	//code("Array.foldRight([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')").equals("16");

	section("Array.shuffle()");
	code("Array.shuffle([1, 2, 3, 10, true, 'yo', null]).size()").equals("7");

	section("Array.reverse()");
	code("Array.reverse([1, 2, 3, 10, true, 'yo', null])").equals("[null, 'yo', true, 10, 3, 2, 1]");
	code("[null].reverse()").equals("[null]");
	code("[].reverse()").equals("[]");
	code("[1, 2, 3].reverse()").equals("[3, 2, 1]");

	section("Array.search()");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], 10, 0)").equals("3");
	code("Array.search([1, 2, 3, 4, 5], 5, 0)").equals("4");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], 3, 3)").equals("-1");
	code("[1, 2, 3, 10, true, 'yo', null].search('yo', 0)").equals("5");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], false, 0)").equals("-1");
	code("[null].search(null, 0)").equals("0");

	section("Array.subArray()");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 5)").equals("[10, true, 'yo']");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 1)").equals("[]");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 0, 100)").equals("[1, 2, 3, 10, true, 'yo', null]");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 1, 1)").equals("[2]");

	section("Array.pop()");
	code("[].pop()").equals("null");
	code("Array.pop(['1', '2'])").equals("'2'");
	code("['1', '2'].pop()").equals("'2'");
	code("let a = ['1', '2', '3'] a.pop() a").equals("['1', '2']");

	section("Array.push()");
	code("let a = [1, 2, 3] Array.push(a, 4)").equals("[1, 2, 3, 4]");
	code("[].push([])").equals("[[]]");
	code("[0].pushAll([1, 2, 3])").equals("[0, 1, 2, 3]");
	code("[0].pushAll([3.5])").equals("[0, 3]");
	code("[0.5].pushAll(['a'])").equals("[0.5]");
	code("let a = [1, 2] a.push(3) a").equals("[1, 2, 3]");
//	code("let a = [1, 2] a.push(3.5) a").equals("[1, 2, 3]");
	code("let a = [1.5, -2.9] a.push(3.5) a").equals("[1.5, -2.9, 3.5]");

	section("Array.pushAll()");
	code("Array.pushAll([], [true, 'yo'])").equals("[true, 'yo']");

	section("Array.join()");
	code("[].join('a')").equals("''");
	code("['salut', 'ça', 'va'].join(' ')").equals("'salut ça va'");
	code("[1, null, 'va'].join(' ')").equals("'1 null va'");

	section("Array.json()");
	code("[1, 2, 3].json()").equals("'[1,2,3]'");

	section("Array.clear()");
	code("let a = [1, 2, 3] a.clear() a").equals("[]");
	code("let a = [1, 2, 3] Array.clear(a)").equals("[]");

	section("Array.fill()");
	code("let a = [1, 2, 3] a.fill(12, 4) a").equals("[12, 12, 12, 12]");
	code("let a = [] Array.fill(a, 'test', 2)").equals("['test', 'test']");

	section("Array.insert()");
	code("let a = [1, 2, 3] Array.insert(a, 12, 1)").equals("[1, 12, 2, 3]");
	code("let a = [1, 2, 3] Array.insert(a, 12, 6)").equals("[1, 2, 3, 0, 0, 0, 12]");

	section("Array.remove()");
	code("let a = [1, 2, 3] Array.remove(a, 1)").equals("2");
	code("let a = [1, 2, 3] Array.remove(a, 1) a").equals("[1, 3]");
//	code("let a = [1, 2, 3] Array.remove(a, 1) a").equals("[0: 1, 2: 3]");
	code("let a = [1, 2, 3] Array.remove(a, 1)").equals("2");

//	code("let a = [] Array.removeKey(a, 'key')").equals("null");
//	code("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key')").equals("'test'");
//	code("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key') a").equals("[0: 1, 1: 2, 2: 3]");

	section("Array.removeElement()");
	code("let a = [1, 2, 3] a.removeElement(1) a").equals("[3, 2]");
	code("let a = [1, 2, 3] a.removeElement('key') a").semantic_error( ls::SemanticError::METHOD_NOT_FOUND, ls::Type::INT_ARRAY.toString() + ".removeElement(" + ls::Type::STRING_TMP.toString() + ")");

	section("Postfix expressions");
	code("let a = [10, ''] a[0]++").equals("11");
	code("let a = [10, ''] a[0]--").equals("9");
	code("let v = 10 [v++, '']").equals("[10, '']");
	code("let v = 90 [v--, '']").equals("[90, '']");

	section("Range access");
	code("[1 2 3 4 5 6 7 8][0:0]").equals("[1]");
	code("[1 2 3 4 5 6 7 8][0:1]").equals("[1, 2]");
	code("[1 2 3 4 5 6 7 8][3:5]").equals("[4, 5, 6]");
	// TODO add more

	/*
	code("3 ~ x -> x ^ x").equals("27");
	code("[1, 2, 3] ~ x -> x + 4").equals("[1, 2, 3, 4]");
	code("[1, 2, 3] ~~ print").equals("[null, null, null]");
	code("[1, 2, 3] ~~ x -> [x, x]").equals("[[1, 1], [2, 2], [3, 3]]");
	code("let s = 0 let a = [1, 2, 3, 4] let f = x -> s += x a ~~ f s").equals("10");
	code("let s = 0 [1, 2, 3, 4, 5].each(x -> s += x)").equals("15");
	code("[1, 2, 3, 4, 5].fold_left(+,0)").equals("15");
	//code("let f = x -> x + 10 let g = x -> x ^ 2 [1, 2, 3] ~~ f ~~ g").equals("[121, 144, 169]");
	//code("[1, 2, 3] ~~ (x -> x * 5) ~~ (x -> x - 1)").equals("[4, 9, 14]");
	*/

	/*
	[1, 2, 3] ~~= (x -> x * 5 + 2)
	[1, 2, 3] ~+= 1   =>   [2, 3, 4]
	[1, 2, 3] ~*= 5
	[1, 2, 3] ~= (x -> x * 5)
	*/

	// let a = [for let i = 0; i < 100; i++ do i end]
	// a[10:*]
	// a[] = 12 (a += 12)
}
