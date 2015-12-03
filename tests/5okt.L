(*final type rep: \[ConstantType(Int), OmegaType\]*)
fun read_list with n =
	let val = readInt in
	if n = 1 then val else
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

fun add with t, n = 
	if isNil #t then !t+n else
	let hd = !t in
	let tl = #t in
	(hd+n)@(add tl n)
in

(add l 2) 
