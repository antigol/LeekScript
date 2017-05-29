#include "Test.hpp"

void Test::test_arrays() {

	header("Arrays");

	section("Constructor");
	code("Array").equals("<class Array>");
	code("Array()").equals("[]");
	code("new Array").equals("[]");
	code("new Array()").equals("[]");
	code("[]").equals("[]");
	code("[1]").equals("[1]");
	code("[1, 2, 3]").equals("[1, 2, 3]");
	code("[1.21, -5, 4.55, 12, -6.7]").equals("[1.21, -5, 4.55, 12, -6.7]");
	code("[true, false, true]").equals("[true, false, true]");
	code("[23, true, '', {}, 123]").equals("[23, true, '', {}, 123]");
	code("let a = x -> x [1, 2, a]").equals("[1, 2, <function>]");

	section("No commas");
	code("[1 2 3]").equals("[1, 2, 3]");
	code("['yo' 'ya' 'yu']").equals("['yo', 'ya', 'yu']");
	code("[true false true true]").equals("[true, false, true, true]");
	code("[[1 2] [[3] 4 [5 6] []]]").equals("[[1, 2], [[3], 4, [5, 6], []]]");

	section("Trailing comma");
	code("[1, 2, 3, ]").equals("[1, 2, 3]");
	code("[1,\n2,\n3,\n]").equals("[1, 2, 3]");

	section("Array.operator +");
	code("[1, 2, 3] + [4, 5, 6]").equals("[1, 2, 3, 4, 5, 6]");
	code("[] + 1").equals("[1]");
	code("[] + 1 + 2 + 3").equals("[1, 2, 3]");
	code("[1] + 2").equals("[1, 2]");
	code("[1] + 0.5").equals("[1, 0.5]");
	code("[0.5] + 'a'").equals("[0.5, 'a']");
	code("['a'] + ['b']").equals("['a', 'b']");
	code("[1] + 0.5 + 'a'").equals("[1, 0.5, 'a']");
	code("[1] + 0.5 + 'a' + 'b'").equals("[1, 0.5, 'a', 'b']");
	code("[1] + null").equals("[1, null]");
	code("[1] + true").equals("[1, true]");
	code("[1] + [2] + [3]").equals("[1, 2, 3]");
	code("[1] + [2.5] + ['a']").equals("[1, 2.5, 'a']");
	code("['a'] + [2.5] + [1]").equals("['a', 2.5, 1]");
	code("[1] + ['a']").equals("[1, 'a']");
	code("['a'] + [1]").equals("['a', 1]");
	code("[12.5] + null").equals("[12.5, null]");
	code("let a = ['yo'] a + null").equals("['yo', null]");
	code("[12.5] + true").equals("[12.5, true]");
	code("['yo'] + true").equals("['yo', true]");
	code("let a = ['yo'] a + true").equals("['yo', true]");
	code("[15.5] + [12, ''][0]").equals("[15.5, 12]");
	code("let a = [15.5] a + [12, ''][0]").equals("[15.5, 12]");
	code("let a = [1] a + [12, ''][0]").equals("[1, 12]");
	code("['yo'] + '!'").equals("['yo', '!']");
	code("let a = ['yo'] a + '!'").equals("['yo', '!']");
	code("let a = [5] let b = ['b'] a + b").equals("[5, 'b']");
	code("['a'] + ['b']").equals("['a', 'b']");
	code("let a = ['a'] a + ['b']").equals("['a', 'b']");
	code("[1, 2] + {}").equals("[1, 2, {}]");
	code("let a = [1, 2] a + {}").equals("[1, 2, {}]");
	code("['a', 'b'] + {}").equals("['a', 'b', {}]");
	code("let a = ['a', 'b'] a + {}").equals("['a', 'b', {}]");
	code("['a', 'b'] + (x -> x)").equals("['a', 'b', <function>]");
	code("let a = ['a', 'b'] a + (x -> x)").equals("['a', 'b', <function>]");
	code("['a', 'b'] + Number").equals("['a', 'b', <class Number>]");
	code("let a = ['a', 'b'] a + Number").equals("['a', 'b', <class Number>]");
	code("let a = [1, 2] a + [3, 4]").equals("[1, 2, 3, 4]");
	code("let a = ['a'] [3.5, 4.6] + a").equals("[3.5, 4.6, 'a']");

	section("Array.operator ~");
	code("let a = [1, 2, 3]; ~a").equals("[3, 2, 1]");
	code("~[1, 2, 3]").equals("[3, 2, 1]");

	section("Array.operator !");
	code("let a = [1, 2, 3]; !a").equals("false");
	code("![1, 2, 3]").equals("false");
	code("![1.5, 2.5, 3.5]").equals("false");
	code("![]").equals("true");

	section("Array.operator |x|");
	code("|[1, 2, 3]|").equals("3");
	code("|[1.5, 2.5, 3.5]|").equals("3");
	code("|['a', 'b', 'c']|").equals("3");

	section("Array.operator []");
	code("[1, 2, 3][1]").equals("2");
	code("let a = [1, 2, 3] a[0]").equals("1");
	code("let a = [1.6, 2.5, 3.4] a[0]").equals("1.6");
	code("let a = [1, 2, 3] a[0] = 5 a[0]").equals("5");
	code("let a = [23, 23, true, '', [], 123]; |a|").equals("6");
	code("let a = [] !a").equals("true");
	code("let a = [1, 2, 3] a[1] = 12 a").equals("[1, 12, 3]");
	code("[1.2, 321.42, 23.15]").equals("[1.2, 321.42, 23.15]");
	code("[1, 2, 3, 4, 5][1:3]").equals("[2, 3, 4]");
	code("let a = [5, 'yolo', 12] a[1]").equals("'yolo'");
	code("let a = [12] a[0]++ a").equals("[13]");
	code("[1, 2, 'a'][['salut', 2][0]]").exception(ls::vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("['a', 'b', 'c'][[2, ''][0]]").equals("'c'");
	code("let a = [[12], ''][0] a[0]++ a").equals("[13]");
	code("let a = [[12], ''][0] a[a]++ a").exception(ls::vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("let a = [[12], [5.5], ['a']] a[0][0] += 1 a[1][0] += 1 a[2][0] += 1 a").equals("[[13], [6.5], ['a1']]");
	code("let a = [1, 2, 3] a[0l]").equals("1");
	code("let a = [1, 2, 3] a[1l]").equals("2");
	code("let a = [1, 2, 3] a[2m]").equals("3");
	code("let a = ['a', 'b', 'c'] a[0.5]").equals("'a'");
	code("let a = ['a', 'b', 'c'] a[1.9]").equals("'b'");
	code("['', [2][0]]").equals("['', 2]");
	code("['', [2.5][0]]").equals("['', 2.5]");

	section("[] operator on unknown arrays");
	code("let v = [['a', 'b'], 12] v[0][0]").equals("'a'");
	code("let v = [['a', 'b'], 12] v[0][1]").equals("'b'");
	code("let v = [['a', 'b'], 12] v[0][true]").equals("'b'");
	code("[['a', 'b'], 12][0][['yolo', 1][0]]").exception(ls::vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("[['a', 'b'], 12][0][2]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let v = [['a', 'b'], 12] v[0][0] = 5 v").equals("[[5, 'b'], 12]");
	code("let v = [['a', 'b'], 12] v[0][2] = 5 v").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);

	section("Out of bounds exception");
	code("[][1]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1, 2, 3][100]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [1, 2, 3] a[10]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[5.6, 7.2][-5]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("['hello', true][2]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [1, 2, 3] a[100] = 12").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [1, 2, 3] a[-100] = 12").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [[12], ''][0] a[100]++ a").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [5] let e = a[1] !? 5 e").equals("5");

	section("Access with booleans");
	code("[1, 2, 3][false]").equals("1");
	code("[1, 2, 3][true]").equals("2");
	code("['1', '2', '3'][false]").equals("'1'");
	code("[1.5, 2.5, 3.5][true]").equals("2.5");

	section("Push with empty array access");
	code("var a = [] a[] = 12 a").equals("[12]");
	code("var a = [1, 2] a[] = 3 a").equals("[1, 2, 3]");
	code("var a = [1, 2] a[] = 3 a").equals("[1, 2, 3]");
	code("var a = [] a[] = 'a' a").equals("['a']");
	code("var a = ['a', 'b'] a[] = 'c' a").equals("['a', 'b', 'c']");
	code("var a = [1, 'b', true] a[] = x -> x a").equals("[1, 'b', true, <function>]");

	section("Array.operator +=");
	code("var a = [1.55] a += 12.9 a").equals("[1.55, 12.9]");
	code("var a = ['a'] a += 'b' a").equals("['a', 'b']");
	code("var a = [1, 2, 3] a[0] += 5 a[0]").equals("6");
	code("var v = 12 var a = [v, 2, 3] a[0] += 5 a[0]").equals("17");
	code("var a = [1, 2, 3] a += 'hello' a").equals("[1, 2, 3, 'hello']");
	code("var a = [1.5] a += ['a', 'b'] a").equals("[1.5, 'a', 'b']");
	code("var a = [1.5] a += false a").equals("[1.5, false]");
	code("var a = [1] a += <2, 3> a").equals("[1, 2, 3]");
	code("var a = [1] a += <5.5, 7.314> a").equals("[1, 5.5, 7.314]");
	code("var a = [1] a += <5, 7.314, 'hello'> a").equals("[1, 5, 7.314, 'hello']");
	code("var a = [1] a += [<5, 7.314, 'hello'>, ''][0] a").equals("[1, 5, 7.314, 'hello']");
	code("var a = [1] a += <'z', 'a'> a").equals("[1, 'a', 'z']");
	code("var a = [1] a += 'a' a").equals("[1, 'a']");
	code("var a = [[1]] a[0] += [12, ''][0] a[0]").equals("[1, 12]");
	code("var a = [1.11] a += [2, 3] a").equals("[1.11, 2, 3]");
	code("var a = [[1.55]] a[0] += [12.9, ''][0] a[0]").equals("[1.55, 12.9]");
	code("var a = [[1.55]] a[0] += [-1.5, 6.7] a[0]").equals("[1.55, -1.5, 6.7]");
	code("var a = [[1.55]] a[0] += <8, 4> a[0]").equals("[1.55, 4, 8]");
	code("var a = [[1.55]] a[0] += < -8.5, 4.7> a[0]").equals("[1.55, -8.5, 4.7]");
	// TODO crashing
	// code("var a = [[1.55]] a[0] += < -8.5, 4.7, 'hello'> a[0]").equals("[1.55, -8.5, 4.7, 'hello']");
	code("var a = ['a'] a += [1, 2]").equals("['a', 1, 2]");
	code("var a = ['a'] a += [1.5, 2.5]").equals("['a', 1.5, 2.5]");
	code("var a = ['a'] a += <1, 2>").equals("['a', 1, 2]");
	code("var a = ['a'] a += <1.5, 2.5>").equals("['a', 1.5, 2.5]");
	code("var a = ['a'] var b = <'b'> a += b a").equals("['a', 'b']");
	code("var a = ['a'] var b = [12] a += b a").equals("['a', 12]");
	code("var a = [[5], ''][0] var b = [12] a + b").equals("[5, 12]");
	code("var a = [[1.5], ''][0] var b = ['a'] a + b").equals("[1.5, 'a']");
	code("var a = [['a'], ''][0] var b = [12] a + b").equals("['a', 12]");
	code("var a = [['a'], ''][0] var b = [12.9] a + b").equals("['a', 12.9]");
	code("var a = [1, 2, 3] a[1] += 0.5 a").equals("[1, 2.5, 3]");
	code("var a = [1, 2, 3] a += [4] a").equals("[1, 2, 3, 4]");

	section("Array.operator ~~");
	code("[1, 2, 3, 4, 5] ~~ x -> x ** 2").equals("[1, 4, 9, 16, 25]");
	code("[1.5, 2.5, 3.5] ~~ x -> x.floor()").equals("[1, 2, 3]");
	code("[1, 2, 3, 4, 5] ~~ (x -> x ** 2)").equals("[1, 4, 9, 16, 25]");
	code("['yo', 'toto', 'salut'] ~~ x -> x + ' !'").equals("['yo !', 'toto !', 'salut !']");
	code("[1, 2, 3] ~~ x -> [x]").equals("[[1], [2], [3]]");
	code("[1, 2, 3] ~~ x -> 'yo'").equals("['yo', 'yo', 'yo']");
	code("let f = x -> x * 10 [1, 2, 3] ~~ f").equals("[10, 20, 30]");
	code("[1.2, 321.42] ~~ x -> x * 1.7").equals("[2.04, 546.414]");
	code("[1, 2, 3, 4, 5] ~~ x -> x.max(3)").equals("[3, 3, 3, 4, 5]");
	// code("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10] ~~ x -> x.max(3).min(8)").equals("[3, 3, 3, 4, 5, 6, 7, 8, 8, 8]");
	code("[1, 2, 3, 4] ~~ x -> x + 0.5").equals("[1.5, 2.5, 3.5, 4.5]");

	section("Array.operator <");
	code("[1] < [1, 2]").equals("true");
	code("[1, 1] < [1, 2]").equals("true");
	code("['a', 'b'] < [1.5, 2.8]").equals("false");
	code("[1, 2] < [1, 2, 'c']").equals("true");
	code("[1, 2] < [1, null]").equals("false");
	code("[1, 2] < [1, 'a']").equals("true");
	code("[1, 2] < [1, 5, 'a']").equals("true");
	code("[1, 2] < [1, 1, 'a']").equals("false");
	code("[1, 'a'] < [1]").equals("false");
	code("[1, null] < [1, 2]").equals("true");
	code("[1, 'a'] < [0, 2]").equals("false");
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
	code("[[1, 2, 3], 1][0] < 5").equals("false");
	code("ptr([1, 2, 3]) < <>").equals("true");

	section("Array.operator ==");
	code("[] == []").equals("true");
	code("[1, 1] == [1]").equals("false");
	code("[1, 1] == [1, 2]").equals("false");
	code("[1, 2] == [1, 2]").equals("true");
	code("[1, '1'] == [1, '2']").equals("false");
	code("[1, '2'] == [1, '2']").equals("true");
	code("[1, 1] == [1, 2.5]").equals("false");
	code("[1, 2.5] == [1, 2.5]").equals("true");

	section("Array.operator in");
	code("2 in [1, 2, 3]").equals("true");
	code("4 in [1, 2, 3]").equals("false");
	code("'yo' in ['ya', 'yu', 'yo']").equals("true");
	code("let a = 2 if (a in [1, 2, 3]) { 'ok' } else { 'no' }").equals("'ok'");
	code("'a' in [['a', 'b', 'c'], ''][0]").equals("true");
	code("2★ in [1, 2, 3]").equals("true");
	code("4★ in [1, 2, 3]").equals("false");
	code("'salut' in [1, 2, 3]").equals("false");
	// TODO
	// let f = x -> x[0]; [f([1]), f([0..3])]

	/*
	 * Methods
	 */
	section("Array.size()");
	code("Array.size([1, 'yo', true])").equals("3");
	code("Array.size([6, 4, 3, 2, -11])").equals("5");
	code("Array.size([6.8999, 5.56])").equals("2");
	code("[1, 'yo', true].size()").equals("3");
	code("[1, -2, 3, -12, -6].size()").equals("5");
	code("[1, 2, 3].size").equals("<function>");
	code("Array.size([1, [1, 2, 3]][1])").equals("3");
	// TODO
	// code("[[1, 2, 3], 'foo'][0].size()").equals("3");

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
	code("['abc', true, 12, [1, 2]].sum()").equals("'abctrue12[1, 2]'");
	code("[10, -5.7, 30.89, 66].sum()").almost(101.19);

	section("Array.product()");
	code("[1, 2, 3, 4].product()").equals("24");
	code("[1.5, 2.5, 3.5, 4.5].product()").equals("59.0625");

	section("Array.map()");
	code("Array.map([1, 2, 3], x -> x ** 2)").equals("[1, 4, 9]");
	code("[3, 4, 5].map(x -> x ** 2)").equals("[9, 16, 25]");
	code("let a = [3, 4, 5] a.map(x -> x ** 2)").equals("[9, 16, 25]");
	code("[321, 213, 121].map(x -> x ** 2).size()").equals("3");
	code("[3.2, 4.5, 5.8].map(x -> x ** 2)").equals("[10.24, 20.25, 33.64]");
	code("['a' 'b' 'c'].map(x -> x)").equals("['a', 'b', 'c']");
	code("let a = ['a' 'b' 'c'] a.map(x -> x)").equals("['a', 'b', 'c']");
	// TODO
	// code("[65 66 67].map(x -> x.char()).join('')").equals("'ABC'");
	// TODO
	// code("[['a', 'b', 'c'], 'foo'][0].map(x -> x + '.')").equals("['a.', 'b.', 'c.']");

	section("Array.map2()");
	code("Array.map2([1, 'yo ', []], [12, 55, 9], (x, y -> x + y))").equals("[13, 'yo 55', [9]]");

	section("Array.max()");
	code("[].max()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().max()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[4, 20, 1, 4].max()").equals("20");
	code("let a = [4, 12, 1, 4] a.max()").equals("12");
	code("['c', 'a', 'd', 'b'].max()").equals("'d'");
	code("let a = ['c', 'a', 'e', 'b'] a.max()").equals("'e'");
	code("[4, 20.5, 1, 4].max()").equals("20.5");

	section("Array.min()");
	code("[].min()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().min()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[4, 20, 1, 4].min()").equals("1");
	code("let a = [4, 12, 1, 4] a.min()").equals("1");
	code("['c', 'a', 'd', 'b'].min()").equals("'a'");
	code("let a = ['c', 'a', 'e', 'b'] a.min()").equals("'a'");
	code("[4, 20.5, 1.5, 4].min()").equals("1.5");

	section("Array.chunk()");
	code("let x = [1, 2, 3, 4] x.chunk(2)").equals("[[1, 2], [3, 4]]");
	code("let x = [1, 2, 3, 4] x.chunk(3)").equals("[[1, 2, 3], [4]]");
	code("let x = [1, 2, 3, 4] x.chunk()").equals("[[1], [2], [3], [4]]");
	code("let x = ['a', 'b', 'c', 'd'] x.chunk()").equals("[['a'], ['b'], ['c'], ['d']]");
	code("let x = [1.6, 2.5, 3.1, 4.67] x.chunk()").equals("[[1.6], [2.5], [3.1], [4.67]]");
	code("let x = ['hello', x -> x, true, Number] x.chunk()").equals("[['hello'], [<function>], [true], [<class Number>]]");

	section("Array.unique()");
	code("let x = [1, 1, 2, 2, 1] x.unique() x").equals("[1, 2, 1]");
	code("let x = [1.5, 1.5, 2.5, 2.5, 1.5] x.unique() x").equals("[1.5, 2.5, 1.5]");
	code("let x = [1, 1, 2, 2, 1] x.sort() x.unique() x").equals("[1, 2]");
	code("let x = ['a', 'a', 'b'] x.unique() x").equals("['a', 'b']");
	code("let x = ['a', 'b', 'c'] x.unique() x").equals("['a', 'b', 'c']");
	code("let x = ['a', 'a', 'b', 'a', 'a'] x.unique() x").equals("['a', 'b', 'a']");

	section("Array.sort()");
	code("let x = [3, 1, 2] x.sort() x").equals("[1, 2, 3]");
	code("let x = [3.1, 3.2, 2.7] x.sort() x").equals("[2.7, 3.1, 3.2]");
	code("let x = ['foo', 'yop', 'abc'] x.sort() x").equals("['abc', 'foo', 'yop']");
	code("let x = [[[]], [[], [], []], [[], []]]; x.sort() x").equals("[[[]], [[], []], [[], [], []]]");
	code("let x = [[1, 2, 3], [3, 1, 2], [2, 3, 1]]; x.sort() x").equals("[[1, 2, 3], [2, 3, 1], [3, 1, 2]]");
	code("Array.sort([3, 2, 1])").equals("[1, 2, 3]");

	section("Array.filter()");
	code("Array.filter([1, 2, 3, 10, true, 'yo'], x -> x > 2)").equals("['yo', 10, 3]");
	code("[3, 4, 5].filter(x -> x > 6)").equals("[]");
	code("[1, 2, 3, 4, 5, 6, 7].filter(x -> x % 2 == 0)").equals("[2, 4, 6]");
	code("let a = [3, 4, 5] a.filter(x -> x < 5)").equals("[3, 4]");
	code("let a = ['a', 'b', 'a'] a.filter(x -> x == 'a')").equals("['a', 'a']");

	section("Array.contains()");
	code("Array.contains([1, 2, 3, 10, 1], 1)").equals("true");
	code("[3, 4, 5].contains(6)").equals("false");
	code("[3.5, 4.5, 5.5].contains(6.5)").equals("false");
	code("[3.5, 4.5, 6.5].contains(6.5)").equals("true");
	code("['a', true, {}].contains(true)").equals("true");
	code("['a', true, {}].contains(12)").equals("false");

	section("Array.isEmpty()");
	code("Array.isEmpty([])").equals("true");
	code("[3, 4, 5].isEmpty()").equals("false");

	section("Array.iter()");
	code("Array.iter([1, 2, 3], x -> System.print(x))").output("1\n2\n3\n");
	code("[4, 5, 6].iter(x -> System.print(x))").output("4\n5\n6\n");
	// TODO will work with capture references in closures
	// code("let a = 0 Array.iter([1, 2, 3], x -> a += x) a").equals("6");
	// TODO crash
	// code("var a = 2 [1, 2, 3].iter(x -> a *= x) a").equals("12");

	section("Array.partition()");
	code("Array.partition([1, 2, 3, 4, 5], (x -> x < 3))").equals("[[1, 2], [3, 4, 5]]");
	code("Array.partition([1, 2, 3, 10, true, 'yo'], x -> x > 2)").equals("[[3, 10, 'yo'], [1, 2, true]]");
	code("[1, 2, 3, 4, 5].partition(x -> x > 3)").equals("[[4, 5], [1, 2, 3]]");
	code("[1, 2, 3, 4, 5].partition(x -> x == 3)").equals("[[3], [1, 2, 4, 5]]");
	code("[1, 2, 3, 4, 5, 6].filter(x -> x > 2).partition(x -> x > 4)").equals("[[5, 6], [3, 4]]");
	// TODO
	// code("[1, 2, 3, 4, 5].partition(x -> 'yolo')").equals("**error**");

	section("Array.first()");
	code("Array.first([1, 2, 3, 10, true, 'yo', null])").equals("1");
	code("['yo', 3, 4, 5].first()").equals("'yo'");
	code("[12, 2].first()").equals("12");
	code("[12.891, 2].first()").equals("12.891");
	code("[].first()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().first()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12.5].clear().first()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);

	section("Array.last()");
	code("Array.last([1, 2, 3, 10, true, 'yo', null])").equals("null");
	code("['yo', 3, 4, 5].last()").equals("5");
	code("[12, 2].last()").equals("2");
	code("[12.891, 2].last()").equals("2");
	code("[].last()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().last()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12.5].clear().last()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	// TODO : the return type of first() must be the element type of the array if it's homogeneous
	// code("[[321, 21], [23, 212], [654, 9876]].first().last()").equals("21");

	section("Array.foldLeft()");
	code("Array.foldLeft([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')").equals("'concat:12310trueyonull'");
	code("Array.foldLeft([2, 2, 3], (x, y -> x ** y), 1)").equals("1");
	code("Array.foldLeft(['a', 'b', 'c', 'd'], (x, y -> x + y), 'X')").equals("'Xabcd'");

	section("Array.foldRight()");
	code("Array.foldRight([2, 2, 3], (x, y -> x ** y), 1)").equals("256");
	code("Array.foldRight(['a', 'b', 'c', 'd'], (x, y -> x + y), 'X')").equals("'abcdX'");

	section("Array.shuffle()");
	code("[].shuffle()");
	code("[1.5, 2.5].shuffle().size()").equals("2");
	code("Array.shuffle([1, 2, 3, 10, true, 'yo', null]).size()").equals("7");
	code("let a = [1, 2, 3, 10, true, 'yo', null] a.shuffle().size()").equals("7");

	section("Array.reverse()");
	code("Array.reverse([1, 2, 3, 10, true, 'yo', null])").equals("[null, 'yo', true, 10, 3, 2, 1]");
	code("[null].reverse()").equals("[null]");
	code("[].reverse()").equals("[]");
	code("[1, 2, 3].reverse()").equals("[3, 2, 1]");
	code("[1.5, 2.5, 3.5].reverse()").equals("[3.5, 2.5, 1.5]");
	code("let a = [1, 2, 3] a.reverse()").equals("[3, 2, 1]");

	section("Array.search()");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], 10, 0)").equals("3");
	code("Array.search([1, 2, 3, 4, 5], 5, 0)").equals("4");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], 3, 3)").equals("-1");
	code("[1, 2, 3, 10, true, 'yo', null].search('yo', 0)").equals("5");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], false, 0)").equals("-1");
	code("[null].search(null, 0)").equals("0");
	code("[100, 125, 112].search(12, 0)").equals("-1");

	section("Array.subArray()");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 5)").equals("[10, true, 'yo']");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 1)").equals("[]");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 0, 100)").equals("[1, 2, 3, 10, true, 'yo', null]");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 1, 1)").equals("[2]");

	section("Array.pop()");
	code("[].pop()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("Array.pop(['1', '2'])").equals("'2'");
	code("['1', '2'].pop()").equals("'2'");
	code("let a = ['1', '2', '3'] a.pop() a").equals("['1', '2']");
	code("[1, 2].pop()").equals("2");
	code("[1.9, 2.78].pop()").equals("2.78");
	code("let a = [1, 2] a.pop()").equals("2");
	code("let a = [1.9, 2.78] a.pop()").equals("2.78");
	code("[12].clear().pop()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12.5].clear().pop()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);

	section("Array.push()");
	code("let a = [1, 2, 3] Array.push(a, 4)").equals("[1, 2, 3, 4]");
	code("[].push([])").equals("[[]]");
	code("let a = [1, 2] a.push(3) a").equals("[1, 2, 3]");
	// TODO
	// code("let a = [1, 2] a.push(3.5) a").equals("[1, 2, 3]");
	code("let a = [1.5, -2.9] a.push(3.5) a").equals("[1.5, -2.9, 3.5]");

	section("Array.pushAll()");
	code("Array.pushAll([], [true, 'yo'])").equals("[true, 'yo']");
	code("[0].pushAll([1, 2, 3])").equals("[0, 1, 2, 3]");
	code("[0].pushAll([3.5])").equals("[0, 3]");
	code("[0.5].pushAll(['a'])").equals("[0.5]");

	section("Array.join()");
	code("[].join('a')").equals("''");
	code("['salut', 'ça', 'va'].join(' ')").equals("'salut ça va'");
	code("[1, null, 'va'].join(' ')").equals("'1 null va'");
	code("[-14, 21, -45, 5].join(' ')").equals("'-14 21 -45 5'");
	code("[-14.67, 21.05, -45, 5.81].join(' ')").equals("'-14.67 21.05 -45 5.81'");
	code("[12].clear().join('whatever')").equals("''");

	section("Array.clear()");
	code("let a = [1, 2, 3] a.clear() a").equals("[]");
	code("let a = [1, 2, 3] Array.clear(a)").equals("[]");
	code("let a = [1, 'yo', true] a.clear() a").equals("[]");

	section("Array.fill()");
	code("let a = [1, 2, 3] a.fill(12, 4) a").equals("[12, 12, 12, 12]");
	// TODO should be possible
	// code("let a = [1, 2, 3] a.fill(12.5, 4) a").equals("[12.5, 12.5, 12.5, 12.5]");
	code("let a = [] Array.fill(a, 'test', 2)").equals("['test', 'test']");
	code("let a = [1.5] a.fill(12.5, 4) a").equals("[12.5, 12.5, 12.5, 12.5]");
	code("let a = [1.5] a.fill(12, 4) a").equals("[12, 12, 12, 12]");

	section("Array.insert()");
	code("let a = ['a', 'b', 'c'] Array.insert(a, 'hello', 1)").equals("['a', 'hello', 'b', 'c']");
	code("let a = ['a', 'b', 'c'] Array.insert(a, 'hello', 6)").equals("['a', 'b', 'c', null, null, null, 'hello']");
	code("let a = [1, 2, 3] Array.insert(a, 12, 1)").equals("[1, 12, 2, 3]");
	code("let a = [1, 2, 3] Array.insert(a, 12, 6)").equals("[1, 2, 3, 0, 0, 0, 12]");

	section("Array.isPermutation()");
	// TODO
	// code("[].isPermutation([])").equals("true");
	code("[1].isPermutation([1])").equals("true");
	code("[1, 2, 3, 4, 5].isPermutation([4, 1, 3, 5, 2])").equals("true");
	code("[1, 2, 3, 4, 5].isPermutation([4, 1, 3, 5, 3])").equals("false");
	code("[1, 2, 3, 4, 5].isPermutation([1, 2, 3, 4, 5, 6])").equals("false");

	section("Array.random()");
	code("[].random(1)").equals("[]");
	code("[1, 2, 3].random(3).size()").equals("3");
	code("[1, 2, 3].random(3).sum()").equals("6");
	code("[1, 2, 3].random(0)").equals("[]");
	code("[1, 2, 3].random(-10)").equals("[]");
	code("['a', 'b', 'c'].random(2).size()").equals("2");
	code("[1, 2, 3].random(1000).size()").equals("3");
	code("['a', 'b', 'c'].random(0)").equals("[]");
	code("let a = ['a', 'b', 'c'] a.random(0)").equals("[]");
	code("let a = ['a', 'b', 'c'] a.random(1).size()").equals("1");
	code("let a = ['a', 'b', 'c'] a.random(3).size()").equals("3");

	section("Array.remove()");
	code("let a = [1, 2, 3] Array.remove(a, 1)").equals("2");
	code("let a = [1, 2, 3] Array.remove(a, 1) a").equals("[1, 3]");
	code("let a = [1, 2, 3] Array.remove(a, 1)").equals("2");
	code("let a = [1, 'yo', true] Array.remove(a, 1)").equals("'yo'");

	// TODO
	// code("let a = [] Array.removeKey(a, 'key')").equals("null");
	// code("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key')").equals("'test'");
	// code("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key') a").equals("[0: 1, 1: 2, 2: 3]");

	section("Array.removeElement()");
	code("[].removeElement(12)").equals("false");
	code("let a = [1, 2, 3] a.removeElement(1) a").equals("[3, 2]");
	code("let a = [1, 2, 3] a.removeElement('key') a").semantic_error(ls::SemanticError::METHOD_NOT_FOUND, {ls::Type::INT_ARRAY.to_string() + ".removeElement(" + ls::Type::STRING_TMP.to_string() + ")"});
	code("[1, 2, 3].removeElement(3)").equals("true");
	code("[1, 2, 3].removeElement(4)").equals("false");
	code("let a = [true, 'hello', [1, 2, 3]] a.removeElement([1, 2, 3]) a").equals("[true, 'hello']");
	code("[true, 'hello', [1, 2, 3]].removeElement('hello')").equals("true");
	code("[true, 'hello', [1, 2, 3]].removeElement('yolo')").equals("false");

	section("Postfix expressions");
	code("let a = [10, ''] a[0]++").equals("10");
	code("let a = [10, ''] a[0]++ a[0]").equals("11");
	code("let a = [10, ''] a[0]--").equals("10");
	code("let a = [10, ''] a[0]-- a[0]").equals("9");
	code("var v = 10 [v++, '']").equals("[10, '']");
	code("var v = 90 [v--, '']").equals("[90, '']");

	section("Range access");
	code("[1 2 3 4 5 6 7 8][0:0]").equals("[1]");
	code("[1 2 3 4 5 6 7 8][0:1]").equals("[1, 2]");
	code("[1 2 3 4 5 6 7 8][3:5]").equals("[4, 5, 6]");
	// TODO add more

	section("Misc");
	// code("let a = [1, []] a[1].push('str') a").equals("[1, ['str']]");
	code("let a = ['hello', 1] a[0].reverse()").equals("'olleh'");
	code("let a = [['a', 'b', 'c'], 1] a[0].reverse()").equals("['c', 'b', 'a']");
	// code("let a = [['a', 'b', 'c'], 'hello'] [a[0].reverse(), a[1].reverse()]").equals("[['c', 'b', 'a'], 'olleh']");
	// code("let h = [1, 'text', [1,2,3], x -> x + 1] h[2].push('test') h[0] = [h[3](h[0]), h[3](h[1])]").equals("[]");

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

	section("Array v1 pushAll");
	// TODO pushAll function must take first array as reference
	code_v1("var a = [] pushAll(a, ['a', 'b', 'c']) a").equals("[]");

	section("Array v1 count");
	code_v1("var a = ['a', 'b', 'c'] count(a)").equals("3");

}
