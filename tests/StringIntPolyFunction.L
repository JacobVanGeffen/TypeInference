(*final type rep: ConstantType(String)*)
let f = lambda x. lambda y. x + y in
let g = (f "a") in
(g "b")
