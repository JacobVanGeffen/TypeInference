(*Identifier .* has not been previously bound*)
let f = lambda x . if x = 0 then 1 else x * (g x) in
let h = lambda x . (f (x-1)) in
(f 6)
