(*Assertion .* failed*)
let g = lambda x. (k x) in
let k = lambda y. y + "a" in
(g 0)
