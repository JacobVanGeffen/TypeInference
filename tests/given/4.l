let f = lambda x.if x = 0 then 1 else x * (g x) in let g = lambda x .(f (x-1)) in (f 6)
