fun my_print with x =
  let _ = print "@@" in print x 
in
let _ = (my_print "duck") in
let _ = (my_print 7) in
let _ = (my_print 1@2) in 
0
