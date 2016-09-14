let x = [(1,2), (1,2)]

x[0].1 = 1.5
x[1].0 = 'test'

x.push((1,1))

ls.print(x)

let y = [[]]
ls.print(y[0].push('a'))
ls.print(y[0])



let z = []
ls.print(z.push('a'))
ls.print(z)

let x = [for let i = 0; i < 100; ++i { 'x = '+i }]

x = [1,2,4]

ls.string(x)
