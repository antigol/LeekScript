#include "Test.hpp"

void Test::test_loops() {

	/*
	 * Conditions
	 */
	header("Conditions");
	code("if true then 12 else 5 end").equals("12");
	code("if false then 12 else 5 end").equals("5");
	code("if (true) { 12 } else { 5 }").equals("12");
	code("if (false) { 12 } else { 5 }").equals("5");
	code("let a = if (false) { 12 } else { 5 } a").equals("5");
	code("let a = if (true) { 'yo' } else { 'hello' } a").equals("'yo'");
	// TODO
	// code("let a = if (true) { 12 } else { 'hello' } a").equals("12");
	code("let a = if (true) { 'hello' } else { 12 } a").equals("'hello'");
	code("if (true) {} else {}").equals("{}");
	// TODO
	// code("if (true) {;} else {}").equals("null");
	code("if (true) { {} } else {}").equals("{}");
	code("if (true) null else {}").equals("null");
	// TODO
	// code("if true").equals("null");
	// code("if true else").equals("null"); crash
	code("if (true) {a: 12} else {b: 5}").equals("{a: 12}");
	code("if (true) { {a: 12} } else { {b: 5} }").equals("{a: 12}");
	code("if (true) 12 else 5").equals("12");
	code("if (false) 12 else 5").equals("5");
	code("if (true) 12").equals("12");
	// TODO
	// code("if (false) 12").equals("null");
	code("if true then 12 end").equals("12");
	code("if true { 5 } else { return 2 }").equals("5");
	code("if true { return 5 } else { 2 }").equals("5");
	code("if false { 5 } else { return 2 }").equals("2");
	code("if false { return 5 } else { 2 }").equals("2");
	code("let a = 5m if true { a } else { 2m }").equals("5");
	code("let a = 5m if true { a } else { a }").equals("5");

	section("Conditions with other types");
	code("if 1212 { 'ok' } else { 5 }").equals("'ok'");
	code("if ['str', true][0] { 12 } else { 5 }").equals("12");
	code("if null { 12 } else { 5 }").equals("5");

	section("Ternary conditions");
	code("true ? 5 : 12").equals("5");
	code("false ? 5 : 12").equals("12");
	code("true ? 'a' : 'b'").equals("'a'");
	code("false ? 'a' : 'b'").equals("'b'");
	code("true ? 'a' : 5").equals("'a'");
	code("false ? 'a' : 5").equals("5");
	code("true ? 5 : 'b'").equals("5");
	code("false ? 5 : 'b'").equals("'b'");
	code("'good' ? 5 : 12").equals("5");
	code("'' ? 5 : 12").equals("12");
	code("'good' ? 'a' : 'b'").equals("'a'");
	code("true ? true ? 5 : 12 : 7").equals("5");
	code("true ? false ? 5 : 12 : 7").equals("12");
	code("false ? false ? 5 : 12 : 7").equals("7");
	code("false ? true ? 5 : 12 : 7").equals("7");
	code("true ? true ? true ? 5 : 12 : 7 : 8").equals("5");
	code("true ? true ? false ? 5 : 12 : 7 : 8").equals("12");
	code("true ? false ? false ? 5 : 12 : 7 : 8").equals("7");
	code("(5 > 10) ? 'a' : (4 == 2 ** 2) ? 'yes' : 'no'").equals("'yes'");

	/*
	 * While loops
	 */
	header("While loops");
	code("var i = 0 while (i < 10) { i++ } i").equals("10");
	code("var i = 0 var s = 0 while (i < 10) { s += i i++ } s").equals("45");
	code("var i = 0 while (i < 100) { i++ if (i == 50) break } i").equals("50");
	code("var i = 0 var a = 0 while (i < 10) { i++ if (i < 8) continue a++ } a").equals("3");
	code("while (true) { break }").equals("(void)");
	code("var i = 10 while (['hello', i][1]) { i-- } i").equals("0");
	code("var i = 0 while i < 10 do i++ end i").equals("10");

	/*
	 * For loops
	 */
	header("For loops");
	code("for var i = 0; ; i++ {}").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);
	code("for var i = 0; false; i++ {}").equals("(void)");
	code("for var i = 0; i < 10; i++ {}").equals("(void)");
	code("var s = 0 for var i = 0; i < 5; i++ do s += i end s").equals("10");
	code("var s = 0 for var i = 0; i < 10; i += 2 do s += i end s").equals("20");
	code("var i = 0 for i = 0; i < 10; i++ { } i").equals("10");
	code("var i = 0 for i = 0; i < 10; i++ { if i == 5 { break } } i").equals("5");
	code("var a = 0 for var i = 0; i < 10; i++ { a++ } a").equals("10");
	code("var a = 0 for var i = 0; i < 10; i++ { if i < 5 { continue } a++ } a").equals("5");
	code("var c = 0 for var t = []; t.size() < 10; t.push('x') { c++ } c").equals("10");
	// TODO wrong result
	// code("var s = 0 for let m = [1: 3, 2: 2, 3: 1]; m; var l = 0 for k, x in m { l = k } m.erase(l) { for x in m { s += x } } s").equals("14");
	code("for var i = 0; ['', i < 10][1]; i++ {}").equals("(void)");
	code("var i = ['', 1][1] for ; i < 10; i <<= 1 {}").equals("(void)");

	section("For whitout braces");
	code("var s = 0 for (var i = 0; i < 10; i++) s += i s").equals("45");

	section("For loops with returns");
	code("for return 12; true; null {}").equals("12");
	code("for ;; return 'hello' {}").equals("'hello'");

	/*
	 * Foreach loops
	 */
	header("Foreach loops");
	code("for v in [1, 2, 3, 4] {}").equals("(void)");
	code("for (v in [1, 2, 3, 4]) {}").equals("(void)");
	code("for (v in [1, 2, 3, 4]) do end").equals("(void)");
	code("var s = 0 for v in [1, 2, 3, 4] { s += v } s").equals("10");
	code("var s = 0 for v in [1.2, 2, 3.76, 4.01] { s += v } s").almost(10.97);
	code("var s = '' for v in ['salut ', 'ça ', 'va ?'] { s += v } s").equals("'salut ça va ?'");
	code("var a = 0 let x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9] for i in x { if i < 5 { continue } a++ } a").equals("5");
	code("var s = 0 for k : v in [1, 2, 3, 4] { s += k * v } s").equals("20");
	code("var s = '' for k : v in ['a': 1, 'b': 2, 'c': 3, 'd': 4] { s += v * k } s").equals("'abbcccdddd'");
	code("(a -> { var s = 0; for x in a { s += x } s })([1, 2, 3, 4.25])").equals("10.25");
	code("var y = '' for k, x in { let x = [] x.push(4) x } { y += k + ':' + x + ' ' } y").equals("'0:4 '");
	// TODO wrong result
	// code("var y = '' for k, x in { let x = [1: 2] x.insert(3, 4) x } { y += k + ':' + x + ' ' } y").equals("'1:2 3:4 '");
	code("var y = '' for k, x in { let x = [1: 2.5] x.insert(3, 4) x } { y += k + ':' + x + ' ' } y").equals("'1:2.5 3:4 '");
	code("var y = '' for k, x in { let x = [1: '2'] x.insert(3, 4) x } { y += k + ':' + x + ' ' } y").equals("'1:2 3:4 '");
	// TODO crash
	// code("var y = 'test' for x in 1 { y = x } y").equals("'test'");
	code("var y = 'test' for x in 'salut' { y = x } y").equals("'t'");
	code("var x = 'test' for x in [1] {} x").equals("'test'");
	// TODO crash
	// code("var y = '' for k, x in { let x = <> x.insert(4) x } { y += k + ':' + x } y").equals("'0:4'");
	// TODO crash
	// code("let fs = [] fs.push(s -> {var sum = 0 for v in s {sum += v} sum}) fs[0](<1, 2>)").equals("3");
	// TODO crash
	// code("let fs = [] fs.push(s -> {[for v in s {v}]}) fs[0](<2,1>)").equals("[1, 2]");

	code("for x in null {}").semantic_error(ls::SemanticError::Type::VALUE_NOT_ITERABLE, {"null", ls::Type::NULLL.to_string()});
	code("for x in true {}").semantic_error(ls::SemanticError::Type::VALUE_NOT_ITERABLE, {"true", ls::Type::BOOLEAN.to_string()});
	code("for x in Number {}").semantic_error(ls::SemanticError::Type::VALUE_NOT_ITERABLE, {"Number", ls::Type::CLASS.to_string()});

	/*
	 * Array For
	 */
	header("Array For");
	code("[for var i = 0; i < 5; ++i { i }]").equals("[0, 1, 2, 3, 4]");
	code("[for var i = 1; i <= 10; ++i { [for var j = 1; j <= 3; ++j { if i == 3 break 2 i * j}] }]").equals("[[1, 2, 3], [2, 4, 6]]");
	code("[for x in [1, 2, 3] { x }]").equals("[1, 2, 3]");
	code("let a = ['a': 'b', 'c': 'd'] [for k, x in a { k + x }]").equals("['ab', 'cd']");
	code("[for x in [1, 2, 3] {[ for y in [1, 2, 3] { if y == 2 continue x * y }] }]").equals("[[1, 3], [2, 6], [3, 9]]");
	// TODO set iterators
	// code("let sorted = [for x in <5, 2, 4, 1, 3> { x }] sorted").equals("[1, 2, 3, 4, 5]");
	code("[for i in [1..10] { i }]").equals("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]");

	/*
	 * Break & continue
	 */
	header("Breaks and Continues");
	code("break").semantic_error(ls::SemanticError::Type::BREAK_MUST_BE_IN_LOOP, {});
	code("continue").semantic_error(ls::SemanticError::Type::CONTINUE_MUST_BE_IN_LOOP, {});
	code("while (true) { x -> {x break} }").semantic_error(ls::SemanticError::Type::BREAK_MUST_BE_IN_LOOP, {});
	code("while (true) { x -> {x continue} }").semantic_error(ls::SemanticError::Type::CONTINUE_MUST_BE_IN_LOOP, {});
	code("while (true) { break 2 }").semantic_error(ls::SemanticError::Type::BREAK_MUST_BE_IN_LOOP, {});
	code("while (true) { continue 2 }").semantic_error(ls::SemanticError::Type::CONTINUE_MUST_BE_IN_LOOP, {});
	code("var r = 0 for x in [1, 2] { for y in [3, 4] { r = 10 * x + y if x + y >= 5 break 2 }} r").equals("14");
	code("var r = 0 for x in [1, 2] { for y in [3, 4] { r = 10 * x + y continue 2 } r = 0 } r").equals("23");
	code("for x in ['a'] { let a = 'a' { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for x in ['a'] { let a = 'a' for y in ['a'] { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for x in ['a'] { let a = 'a' for y in ['a'] { let b = 'b' break 2 let c = 'c' } let d = 'd' } 0").equals("0");
	code("for var x = 0; x < 2; ++x { let a = 'a' { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for var x = 0; x < 2; ++x { let a = 'a' for var y = 0; y < 2; ++y { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for var x = 0; x < 2; ++x { let a = 'a' for var y = 0; y < 2; ++y { let b = 'b' break 2 let c = 'c' } let d = 'd' } 0").equals("0");
	code("while (true) { break 0 }").syntaxic_error(ls::SyntaxicalError::Type::BREAK_LEVEL_ZERO, {});
	code("while (true) { continue 0 }").syntaxic_error(ls::SyntaxicalError::Type::CONTINUE_LEVEL_ZERO, {});

	/*
	 * Match
	 */
	header("Match");
	code("match 50 {}").equals("null");
	code("match 50 { ..: 4 }").equals("4");
	code("match 3 { 1 : 1 2 : 2 3 : 3 }").equals("3");
	code("match 3 { 1 : 1 2 : 2 .. : 3 }").equals("3");
	code("match 'a' { 'a' : 1 'b' : 2 .. : 3 }").equals("1");
	code("match 4 { 1 : 1 2 : 2 3 : 3 }").equals("null");
	code("match 'a' { 1 : 1 'a' : 'a' }").equals("'a'");
	code("match 1 { 1 : 1 'a' : 'a' }").equals("1");
	code("match 1 { 1|2 : 1 'a'|[]|{} : 'a' }").equals("1");
	code("match 1 { (4 - 2)|2| |-1| : 1 'a'|[]|{} : 'a' }").equals("1");
	code("match 1 + 5 { (4 + 1)|2|8 : 1 'a'|[]|{} : 'a' }").equals("null");
	code("match 50 { 0..50: 1 50..100: 2 }").equals("2");
	code("match 50 { ..50: 1 50..: 2 }").equals("2");
	code("match 50 { ..10: 1 ..100: 2 }").equals("2");
	code("match 50 { ..100: 1 ..100: 2 }").equals("1");
	code("match 50 { 100..0: 4 }").equals("null");
	code("let e = 'e' match e { ..'b': 1 ..'z': 2 }").equals("2");
	code("match 'e' { 'z'..: 1 'b'..: 2 }").equals("2");
	code("match [1] { ..[]: 1 ..[2, 2]: 2 }").equals("2");
	code("match [1] { [2, 2]..: 1 []..: 2 }").equals("2");
	code("let b = 'b' match 'e' { ..b: 1 1..6|0..9: 2 ..|..: 3}").equals("3");
	code("let a = match 3 { 1 : 1 2 : 2 3 : 3 } a").equals("3");
	code("match 2 { 1 : 1 2 : {} 3 : 3 }").equals("{}");
}
