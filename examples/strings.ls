let s = "1010111010" × 20
let l = s.size()

let r1 = ''
let r2 = ''

for var i = 0; i < 200; i += 1 {
	r1 += s[i]
	r2 = s[i] + r2
}
r2
