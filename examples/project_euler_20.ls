let digits = [1]

for let p = 1; p <= 100; p++ {
	for let j = 0; j < digits.size(); ++j {
		digits[j] *= p
	}
	for let i = 0; i < digits.size(); ++i {
		if (9 < digits[i]) {
			let m = digits[i] % 10
			let q = (digits[i] - m) / 10
			digits[i] = m
			if (i < digits.size() - 1) {
				digits[i + 1] += q
			} else {
				digits.push(q)
				break
			}
		}
	}
	let n = digits.size() - 1
	while (9 < digits[n]) {
		let mo = digits[n] % 10
		let qu = (digits[n] - mo) / 10
		digits[n] = mo
		digits.push(qu)
		n++
	}
}
// print(~digits)
let sum = 0
for let i = 0; i < digits.size(); ++i {
	sum += digits[i]
}
sum
