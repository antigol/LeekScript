#include "Test.hpp"

void Test::test_numbers() {

	header("Numbers");

	section("Basic numbers");
	code("0").equals("0");
	code("-1").equals("-1");
	code("-(-1)").equals("1");
	code("π").almost(3.141592653589793116);

	section("Basic operations");
	code("0 + 5").equals("5");
	code("5 + 5").equals("10");
	code("10 - 3").equals("7");
	code("-2 + 3").equals("1");
	code("5 * 5").equals("25");
	code("15 / 3").equals("5");
	code("15 / 2").equals("7.5");
	code("12 ** 2").equals("144");
	code("2 ** 5").equals("32");
	code("2 < 5").equals("true");
	code("12 < 5").equals("false");
	code("5 == 12").equals("false");
	code("12 == 12").equals("true");
	code("0.2 + 0.1").almost(0.3);
	code("|-12|").equals("12");
	code("-12 * 2").equals("-24");
	code("(-12) * 2").equals("-24");
	code("-12 ** 2").equals("-144");
	code("-12 ** 2").equals("-144");
	code("(-12) ** 2").equals("144");
	code("-12 + 2").equals("-10");
	code("let a = [2, 'a'] [-a[0], +a[0], ~a[0]] == [-2, 2, ~2]").equals("true");

	section("Hexadecimal representation");
	code("0x0").equals("0");
	code("0x00000000").equals("0");
	code("0x1").equals("1");
	code("0x00000001").equals("1");
	code("0xf").equals("15");
	code("0x0000000f").equals("15");
	code("-0xf").equals("-15");
	code("0xff").equals("255");
	code("0x10").equals("16");
	code("-0xffff").equals("-65535");
	code("0xffffffff").equals("4294967295");

	section("Binary representation");
	code("0b0").equals("0");
	code("0b00001").equals("1");
	code("0b1001010110").equals("598");
	code("-0b0101101001111").equals("-2895");

	section("null must not be considered as 0");
	code("null == 0").equals("false");
	code("null < 0").equals("true");
	code("null + 5").equals("null");
	code("5 + null").equals("null");
	code("5 / null").equals("null");
	code("null / 12").equals("null");
	code("null * 5").equals("null");
	code("5 * null").equals("null");

	section("Numbers with variables");
	code("let a = 2 a++").equals("2");
	code("let a = 2; ++a").equals("3");
	code("let a = 2 a--").equals("2");
	code("let a = 2; --a").equals("1");
	code("let a = 2 a += 5").equals("7");
	code("let a = 2 a -= 5").equals("-3");
	code("let a = 2 a *= 5").equals("10");
	code("let a = 100 a /= 5").equals("20");
	code("let a = 56 a %= 17").equals("5");
	code("let a = 15 a **= 2").equals("225");
	code("let a = 1.5 a * 0.5").equals("0.75");

	section("multiple operations");
	code("(33 - 2) / 2").equals("15.5");
	code("12 < (45 / 4)").equals("false");
	code("12 == (24 / 2)").equals("true");
	code("2.5 + 4.7").almost(7.2);
	code("2.5 × 4.7").equals("11.75");
	code("5 * 2 + 3 * 4").equals("22");

	section("Binary operators");
	code("0 & 0").equals("0");
	code("1 & 0").equals("0");
	code("1 & 1").equals("1");
	code("5 & 12").equals("4");
	code("87619 & 18431").equals("17987");
	code("let a = 87619 a &= 18431").equals("17987");
	code("let a = 87619 a &= 18431 a").equals("17987");
	code("0 | 0").equals("0");
	code("1 | 0").equals("1");
	code("1 | 1").equals("1");
	code("5 | 12").equals("13");
	code("87619 | 18431").equals("88063");
	code("let a = 87619 a |= 18431").equals("88063");
	code("let a = 87619 a |= 18431 a").equals("88063");
	code("0 ^ 0").equals("0");
	code("1 ^ 0").equals("1");
	code("1 ^ 1").equals("0");
	code("5 ^ 12").equals("9");
	code("87619 ^ 18431").equals("70076");
	code("let a = 87619 a ^= 18431").equals("70076");
	code("let a = 87619 a ^= 18431 a").equals("70076");

	section("Binary shift operators");
	code("0 << 0").equals("0");
	code("1 << 0").equals("1");
	code("123456 << 0").equals("123456");
	code("0 << 1").equals("0");
	code("0 << 12").equals("0");
	code("1 << 8").equals("256");
	code("123 << 12").equals("503808");
	code("let a = 123 a <<= 11").equals("251904");
	code("let a = 123 a <<= 13 a").equals("1007616");
	code("0 >> 0").equals("0");
	code("1 >> 0").equals("1");
	code("123456 >> 0").equals("123456");
	code("0 >> 1").equals("0");
	code("0 >> 12").equals("0");
	code("155 >> 3").equals("19");
	code("-155 >> 3").equals("-20");
	code("12345 >> 8").equals("48");
	code("123123123 >> 5").equals("3847597");
	code("let a = 123123123 a >>= 6").equals("1923798");
	code("let a = 123123123 a >>= 7 a").equals("961899");
	code("155 >>> 3").equals("19");
	code("-155 >>> 3").equals("536870892");
	code("let a = -155 a >>>= 4").equals("268435446");
	code("let a = -155 a >>>= 5 a").equals("134217723");

	section("String conversions");
	code("65.char()").equals("'A'");
	code("Number.char(65)").equals("'A'");
	code("126.char()").equals("'~'");
	code("Number.char(128040)").equals("'🐨'");

	section("Multiple precision numbers");
	code("12344532132423").equals("12344532132423");
	code("12344532132423m * 987657897613412m").equals("12192174652930109838844857276");
	code("0m").equals("0");
	code("2 ** 50").equals("-2147483648");
	code("2m ** 50").equals("1125899906842624");
	code("0xf45eab5c9d13aab44376beff").equals("75628790656539575381594128127");
	code("1000m - 12").equals("988");
	code("1000m - (-12)").equals("1012");

	section("Overflow");
	code("123m ** 1900").exception(ls::VM::Exception::NUMBER_OVERFLOW);

	section("Integer division by zero");
	code("1 \\ 0").exception(ls::VM::Exception::DIVISION_BY_ZERO);
	code("1 % 0").exception(ls::VM::Exception::DIVISION_BY_ZERO);

	/*
	 * Number standard library
	 */
	header("Number standard library");
	section("Constructor");
	code("Number").equals("<class Number>");
	code("Number()").equals("0");
	code("[Number(), 'str']").equals("[0, 'str']");
	code("new Number").equals("0");
	code("new Number()").equals("0");
	code("new Number(12)").equals("12");

	section("Constants");
	code("Number.pi").almost(3.141592653589793116);
	code("2 × Number.pi").almost(6.283185307179586232);
	code("Number.e").almost(2.718281828459045091);
	code("Number.phi").almost(1.618033988749894903);
	code("Number.epsilon").almost(0.000000000000000222);

	section("Number.abs()");
//	code("Number.abs").equals("<function>");
	code("Number.abs(-12)").equals("12");
	code("Number.abs(-12.67)").almost(12.67);
	code("Number.abs(['a', -15][1])").equals("15");
	code("(-17).abs()").equals("17");
	code("(-19.5).abs()").equals("19.5");

	section("Number.exp()");
	code("Number.exp(0)").equals("1");
	code("Number.exp(1)").almost(M_E);
	code("Number.exp(4)").almost(54.598150033144236204);
	code("Number.exp(4.89)").almost(132.953574051282743085);
	code("Number.exp(-2.97)").almost(0.051303310331919108);
	code("Number.exp(['a', 7.78][1])").almost(2392.274820537377763685);
	code("0.exp()").equals("1");
	code("1.exp()").almost(M_E);
	code("7.exp()").almost(1096.633158428458500566);
	code("(-7).exp()").almost(0.000911881965554516);
	code("(-3.33).exp()").almost(0.035793105067655297);
	code("Number.e ** 5").almost(148.413159102576571513);

	section("Number.floor()");
	code("Number.floor(5.9)").equals("5");
	code("var a = 5 Number.floor(a)").equals("5");
	code("var a = 5.4 Number.floor(a)").equals("5");
	code("Number.floor(['a', -14.7][1])").equals("-15");

	section("Number.round()");
	code("Number.round(5.7)").equals("6");
	code("Number.round(5.4)").equals("5");
	code("Number.round(['a', -15.89][1])").equals("-16");

	section("Number.ceil()");
	code("Number.ceil(5.1)").equals("6");

	section("Number.max()");
	code("Number.max(5, 12)").equals("12");
	code("Number.max(75.7, 12)").almost(75.7);
	code("Number.max(5, 12.451)").almost(12.451);
	code("Number.max([5, 'a'][0], 4)").equals("5");
	code("Number.max([5, 'a'][0], 76)").equals("76");
	code("Number.max(4, [5, 'a'][0])").equals("5");
	code("Number.max(77, [5, 'a'][0])").equals("77");
	code("Number.max([55, 'a'][0], [5, 'a'][0])").equals("55");

	section("Number.cos()");
	code("Number.cos(0)").equals("1");
	code("Number.cos(π)").equals("-1");
	code("Number.cos(π / 2)").almost(0.0);
	code("π.cos()").equals("-1");
//	code("['', π][1].cos()").equals("-1");
	code("Number.cos(['', π][1])").equals("-1");

	section("Number.acos()");
	code("Number.acos(1)").equals("0");
	code("Number.acos(-1)").almost(M_PI);
	code("Number.acos(0)").almost(M_PI / 2);
	code("(-0.33).acos()").almost(1.907099901948877019);
	code("Number.acos(['y', 0][1])").almost(M_PI / 2);

	section("Number.sin()");
	code("Number.sin(0)").equals("0");
	code("Number.sin(π)").almost(0.0);
	code("Number.sin(π / 2)").equals("1");
	code("Number.sin(- π / 2)").equals("-1");
	code("Number.sin(['', π / 2][1])").equals("1");

	section("Number.tan()");
	code("Number.tan(0)").equals("0");
	code("Number.tan(π)").almost(0.0);
	code("Number.tan(π / 4)").almost(1.0);
	code("Number.tan(- π / 4)").almost(-1.0);
	code("Number.tan(['', π / 4][1])").almost(1.0);

	section("Number.asin()");
	code("Number.asin(0)").equals("0");
	code("Number.asin(-1)").almost(-M_PI / 2);
	code("Number.asin(1)").almost(M_PI / 2);
	code("0.33.asin()").almost(0.33630357515398035);
	code("Number.asin(['y', -1][1])").almost(-M_PI / 2);

	section("Number.atan()");
	code("Number.atan(1)").almost(M_PI / 4);
	code("Number.atan(-1)").almost(-M_PI / 4);
	code("Number.atan(0.5)").almost(0.463647609000806094);
	code("0.atan()").equals("0");
	code("Number.atan(['y', 0.5][1])").almost(0.463647609000806094);

	section("Number.atan2()");
	code("Number.atan2(1, 1)").almost(M_PI / 4);
	code("Number.atan2(150.78, 150.78)").almost(M_PI / 4);
	code("Number.atan2(1, 0)").almost(M_PI / 2);
	code("Number.atan2(-1, 0)").almost(-M_PI / 2);
	code("Number.atan2(0, 1)").equals("0");
	code("Number.atan2(0, -1)").almost(M_PI);
	code("Number.atan2(12.12, 42.42)").almost(0.278299659005111333);
	code("1.atan2(1)").almost(M_PI / 4);
	//code("['', -1][1].atan2(1)").almost(-M_PI / 4);
	code("1.atan2(['', -1][1])").almost(3 * M_PI / 4);
	//code("['', -1][1].atan2(['', -1][1])").almost(-3 * M_PI / 4);
	code("Number.atan2(1, 1)").almost(M_PI / 4);
	code("Number.atan2(['', -1][1], 1)").almost(-M_PI / 4);
	code("Number.atan2(1, ['', -1][1])").almost(3 * M_PI / 4);
	code("Number.atan2(['', -1][1], ['', -1][1])").almost(-3 * M_PI / 4);

	section("Number.cbrt()");
	code("Number.cbrt(1728)").almost(12.0, 1e-14);
	code("1728.cbrt()").almost(12.0, 0.00000000000001);
	code("Number.cbrt(['', 1728][1])").almost(12.0, 0.00000000000001);
//	code("['', 1728][1].cbrt()").almost(12.0, 0.00000000000001);

	section("Number.isInteger()");
	code("Number.isInteger(12)").equals("true");
	code("Number.isInteger(0)").equals("true");
	code("Number.isInteger(-5)").equals("true");
	code("Number.isInteger(12.9)").equals("false");
	code("Number.isInteger(-5.2)").equals("false");
	code("Number.isInteger(π)").equals("false");

	section("Object-like calls");
	code("(-12).abs()").equals("12");
	code("π.cos()").equals("-1");
	code("(π / 2).sin()").equals("1");
	code("12.sqrt()").almost(3.464101615137754386);
	code("12.8.floor()").equals("12");
	code("-12.8.floor()").equals("-12");
	code("(-12.8).floor()").equals("-13");
	code("12.2.ceil()").equals("13");
	code("12.8.round()").equals("13");
	code("-12.8.round()").equals("-13");
	/*
	code("2.pow(10)").equals("1024");
	code("0.isInteger()").equals("true");
	code("56.7.isInteger()").equals("false");
	code("(-56.7).isInteger()").equals("false");
	code("3.max(5)").equals("5");
	code("5.max(3)").equals("5");
	*/

	section("Combinated");
	code("3.max(5).min(2)").equals("2");
	code("3.max(5).max(10).max(12)").equals("12");
	code("10.max(5).max(8.7).max(-3.91)").equals("10");
	code("10.sqrt().cos()").almost(-0.99978607287932586);
}
