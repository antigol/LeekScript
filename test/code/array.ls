var a = []
let n = 12345

for (var i = 0; i < n; ++i) {
	a += i
}

a = a.filter(x -> x > 11111)
a = a.map(x -> x + 12).filter(x -> x < 11999)
a = a.map(x -> x - 9999)

a.foldLeft(x, y -> x + y, -1000)
