fun remove_last with list =
	(if isNil #list then
		(Nil)
	else
		((!list) @ (remove_last #list))
	)

in

fun sum with list =
	if isNil list then
		(0)
	else
		((!list) + (sum #list))

in

fun length with list =
	if isNil list then
		(0)
	else
		((1) + (length #list))

in

fun multiply with list1, list2 =
	(if isNil list1 then
		Nil
	else
		((!list1 * !list2) @ (multiply #list1 #list2))
	)

in

fun insert_ordered with list, item =
	(if isNil #list then (* all done, last item is dummy 0 *)
		(item@0)
	else
		(if ((#(!list)) > (#item)) then
			((!list) @ (insert_ordered (#list) item))
		else
			(item@list)
		)
	)

in

fun get_name with _ =
	(let _ = (print "enter name:") in
		(let name = readString in
			name
		)
	)

in

fun get_gradeweights with weights =
	(if isNil weights then
		(0)
	else
		let grade =
			(let _ = (print "enter grade:") in
				(readInt)
			)
		in
		(if (grade > 100) | (grade < 0) then
			(let _ = (print "incorrect grade value") in
				(get_gradeweights weights)
			)
		else
			((grade * !weights) @ (get_gradeweights #weights))
		)
	)

in

fun get_avg_grade with weights =
	let gradeweights = (get_gradeweights weights) in
	((sum (gradeweights)) / (100))

in

fun gather_students with grades_list, weights, continue =
	(if (continue = 0) then
		(remove_last grades_list)
	else
		let name =
			(let _ = (print "enter name:") in
				(readString)
			)
		in
		let avg_grade = (get_avg_grade weights) in
		let new_grades_list =
			(insert_ordered grades_list ((name)@(avg_grade)))
		in
		let cont_str =
			(let _ = (print "enter another student (y/n)?") in
				(readString)
			)
		in
		let continue =
			((cont_str = "y") | (cont_str = "yes") | (cont_str = "ye"))
		in
		(gather_students new_grades_list weights continue)
	)

in

fun get_num_weights with _ =
	(let num_weights =
		(let _ = (print "number of weights?") in
			(readInt)
		)
	in
	(if (num_weights <= 0) then
		(let _ = (print "number of wieghts must be positive") in
			(get_num_weights 0)
		)
	else
		(num_weights)
	)
)

in

fun get_weights with num_weights =
	(if (num_weights = 0) then
		(Nil)
	else
		let weight =
			(let _ = (print "enter weight:") in
				(readInt)
			)
		in
		(if (weight > 100) | (weight < 0) then
			(let _ = (print "incorrect wieght value") in
				(get_weights num_weights)
			)
		else
			(weight @ (get_weights (num_weights - 1)))
		)
)

in

(*
(insert_ordered (0) ((get_name 0)@(get_avg_grade (10@10@10@70))))
(get_avg_grade (10@10@10@70))
(gather_students (0) (10@10@10@70) 1)
(get_num_weights 0)
*)
let num_weights = (get_num_weights 0) in
let weights = (get_weights num_weights) in
(if ((sum weights) <> 100) then
	(let _ = (print "Error: weights do not add to 100") in
		0
	)
else
	(let output = (gather_students (0) weights 1) in
		(let _ = (print output) in
			output
		)
	)
)
