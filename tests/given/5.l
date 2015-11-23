fun read_list with n =
	if n = 0 then Nil else
	let val = readInt in
	let rest = (read_list (n-1)) in
	val@rest
in
let _ = print "Enter number of integers in list" in
let num = readInt in
let _ = print "Please enter the list values:" in
let l = (read_list num) in
let _ = print "List entered" in
let _ = print l in
let _ = print "Adding 2 to each element: " in

fun add with l, n = 
	if isNil #l then l+n else
	let hd = !l in
	let tl = #l in
	(hd+n)@(add tl n)
in

(add l 2) 
