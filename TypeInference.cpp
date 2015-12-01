#include "ast/ConstantType.h"
#include "ast/VariableType.h"
#include "ast/FunctionType.h"
#include "ast/ListType.h"
#include "ast/AlphaType.h"
#include "ast/expression.h"
#include "TypeInference.h"

map<string, VariableType*> renames;

int var_counter = 1;
string get_new_variable() {
	return "v" + to_string(var_counter++);
}

Expression* TypeInference::eval_unop(AstUnOp* b) {
	Expression* e = eval(b->get_expression());
	switch (b->get_unop_type()) {
		case HD:
			b->type = e->type->get_hd();
			return b;
		case TL:
			b->type = e->type->get_tl();
			return b;
		case ISNIL:
		case PRINT:
			b->type = ConstantType::make("Int");
			return b;
		default:
			assert(false);
			return AstNil::make();
	}
}

Expression* eval_binop_to_int(Expression* b, Expression* eval_e1, Expression* eval_e2) {
	// e1 and e2 must have same type
	assert(eval_e1->type != nullptr); assert(eval_e2->type != nullptr);
	assert(eval_e1->type->unify(eval_e2->type));
	// b's type is an int as implied by the function name
	b->type = ConstantType::make("Int");
	return b;
}

Expression* eval_binop_to_int_or_string(Expression* b, Expression* eval_e1, Expression* eval_e2) {
	// e1 and e2 must have same type
	assert(eval_e1->type != nullptr); assert(eval_e2->type != nullptr);
	assert(eval_e1->type->unify(eval_e2->type));
	// b is e1 or e2's type, doesn't really matter since types match
	b->type = eval_e1->type;
	return b;
}

Expression* TypeInference::eval_binop(AstBinOp* b) {
	Expression* e1 = b->get_first();
	Expression* e2 = b->get_second();
	Expression* eval_e1 = eval(e1);
	Expression* eval_e2 = eval(e2);


	    // Only look at type of head (produces new VariableType of e is variable)
		    eval_e1->type = eval_e1->type->get_hd();
		    eval_e2->type = eval_e2->type->get_hd();

	    // If head of e is a list, get its head
	    while (eval_e1->type->get_kind() == TYPE_LIST)
	        eval_e1->type = eval_e1->type->get_hd();
	    while (eval_e2->type->get_kind() == TYPE_LIST)
	        eval_e2->type = eval_e2->type->get_hd();

		// After this point, eval_e1/2 have the types of their heads (or recursive heads, if head is a list)

	switch (b->get_binop_type()) {
		// for these cases e1 and e2 must be ints
		case MINUS:
		case TIMES:
		case DIVIDE:
		case AND:
		case OR:
		case LT:
		case LEQ:
		case GT:
		case GEQ:
			assert(eval_e1->type != nullptr);
			assert(eval_e1->type->unify(ConstantType::make("Int")));
			assert(eval_e2->type->unify(ConstantType::make("Int")));
		// for these two it could be ints or strings or lists
		case EQ:
		case NEQ:
			return eval_binop_to_int(b, eval_e1, eval_e2);

		// could also be ints or strings here, but so could the result (whereas before the result had to be an int)
		case PLUS:
			return eval_binop_to_int_or_string(b, eval_e1, eval_e2);

		case CONS:
			b->type = ListType::make(eval_e1->type, eval_e2->type);
			return b;
		default:
			assert(false);
			return AstNil::make();
	}
}

Type* compute_msu(Type* t1, Type* t2) {
	// find representatitives of thet types
	Type* t1_rep = t1->find();
	Type* t2_rep = t2->find();

	// case by case basis over { ConstantType, VariableType, ListType, FunctionType }
	if (t1_rep->get_kind() == TYPE_VARIABLE || t2_rep->get_kind() == TYPE_VARIABLE) {
		return AlphaType::make();
	} else if (t1_rep->get_kind() == TYPE_ALPHA || t2_rep->get_kind() == TYPE_ALPHA) {
		return AlphaType::make();
	} else if (t1_rep->get_kind() == TYPE_CONSTANT && t2_rep->get_kind() == TYPE_CONSTANT) {
		ConstantType* t1_const = static_cast<ConstantType*>(t1_rep);
		ConstantType* t2_const = static_cast<ConstantType*>(t2_rep);
		if (t1_const->get_name() == t2_const->get_name()) {
			return t1_const; // or t2_const doesn't matter
		} else {
			return AlphaType::make();
		}
	} else if (t1_rep->get_kind() == TYPE_FUNCTION && t2_rep->get_kind() == TYPE_FUNCTION) {
		// look through all the args of each function until they don't match, turn that into an alpha type
		FunctionType* t1_fun = static_cast<FunctionType*>(t1_rep);
		FunctionType* t2_fun = static_cast<FunctionType*>(t2_rep);

		vector<Type*> t1_args = t1_fun->get_args();
		vector<Type*> t2_args = t2_fun->get_args();

		size_t size = min(t1_args.size(), t2_args.size());
		vector<Type*> msu_args;
		for (size_t i=0; i<size; ++i) {
			Type* msu = compute_msu(t1_args[i], t2_args[i]);
			msu_args.push_back(msu);
		}

		// if lists aren't the same size then the last type actually needs to be an alpha
		if (t1_args.size() != t2_args.size()) {
			msu_args.back() = AlphaType::make();
		}

		string name = "msu(" + t1_fun->get_name() + ", " + t2_fun->get_name() + ")";
		return FunctionType::make(name, msu_args);
	}

	// Types are both lists
	Type* head = compute_msu(t1_rep->get_hd(), t2_rep->get_hd());
	Type* tail = compute_msu(t1_rep->get_tl(), t2_rep->get_tl());
	return ListType::make(head, tail);
}


AstLambda* TypeInference::eval_lambda(AstLambda* lambda, const string id) {
	// lambda x. S1
	AstIdentifier* formal = lambda->get_formal();
	Expression* body = lambda->get_body();

	// rename x to generic v*
	string rename = get_new_variable();
	VariableType* rename_var = VariableType::make(rename);
	renames[formal->get_id()] = rename_var;

	// make lambda'
	string lambda_prime_str = id + "'";
	VariableType* lambda_prime = VariableType::make(lambda_prime_str);

	// make lambda's function type
	vector<Type*> args;
	args.push_back(rename_var);
	args.push_back(lambda_prime);
	FunctionType* lambda_type = FunctionType::make(id, args);

	// evaluate body, as a lambda with name lambda' if needed
	Expression* body_eval;
	if (body->get_type() == AST_LAMBDA) {
		body_eval = eval_lambda(static_cast<AstLambda*>(body), lambda_prime_str);
	} else {
		body_eval = eval(body);
	}
	// unify the result with lambda'
	assert(body_eval->type != nullptr);
	assert(lambda_prime->unify(body_eval->type));

	// return lambda with it's type
	lambda->type = lambda_type;
	return lambda;
}

Expression* TypeInference::eval(Expression* e) {
	cout << "- evaluating " << e->to_value() << endl;
	Expression* res_exp = NULL;
	switch (e->get_type()) {
		case AST_INT:
		{
			e->type = ConstantType::make("Int");
			res_exp = e;
			break;
		}

		case AST_STRING:
		{
			e->type = ConstantType::make("String");
			res_exp = e;
			break;
		}

		case AST_NIL:
		{
			e->type = ConstantType::make("Nil");
			res_exp = e;
			break;
		}

		case AST_READ:
		{
			AstRead* r = static_cast<AstRead*>(e);
			if (r->read_integer()) {
				e->type = ConstantType::make("Int");
				res_exp = e;
			} else {
				e->type = ConstantType::make("String");
				res_exp = e;
			}
			break;
		}

		case AST_IDENTIFIER: // TODO make sure id's don't overwrite eachother
		{
			AstIdentifier* id = static_cast<AstIdentifier*>(e);
			// if id has been renamed get it's renamed type, else create var(id)
			VariableType* id_type;
			map<string, VariableType*>::iterator rename_ptr = renames.find(id->get_id());
			if (rename_ptr != renames.end()) { // found it
				id_type = rename_ptr->second;
			} else {
				id_type = VariableType::make(id->get_id());
			}

			e->type = id_type;
			res_exp = e;
			break;
		}

		case AST_LET:
		{
			// let id = S1 in S2
			AstLet* let = static_cast<AstLet*>(e);

			// eval id
			AstIdentifier* id = let->get_id();
			Expression* id_eval = eval(id);

			// eval S1, as a lambda if needed
			Expression* val = let->get_val();
			Expression* val_eval;
			if (val->get_type() == AST_LAMBDA) {
				val_eval = eval_lambda(static_cast<AstLambda*>(val), id->get_id());
			} else {
				val_eval = eval(val);
			}

			// unify var(id) and type of S1
			assert(id_eval->type != nullptr); assert(val_eval->type != nullptr);
			assert(id_eval->type->unify(val_eval->type));

			// eval S2 to keep moving forward
			res_exp = eval(let->get_body());

			break;
		}

		case AST_BRANCH:
		{
			// if P then S1 else S2
			AstBranch* branch = static_cast<AstBranch*>(e);

			// eval P
			Expression* pred = branch->get_pred();
			Expression* pred_eval = eval(pred);
			// P must be an Int
			assert(pred_eval->type != nullptr);
			assert(pred_eval->type->unify(ConstantType::make("Int")));

			// eval S1
			Expression* then_exp = branch->get_then_exp();
			Expression* then_exp_eval = eval(then_exp);

			// eval S2
			Expression* else_exp = branch->get_else_exp();
			Expression* else_exp_eval = eval(else_exp);

			// return MSU (most specific union) of S1 and S2
			assert(then_exp_eval->type != nullptr); assert(else_exp_eval->type != nullptr);
			Type* msu = compute_msu(then_exp_eval->type, else_exp_eval->type);
			e->type = msu;
			res_exp = e;

			break;
		}

		case AST_UNOP:
		{
			AstUnOp* b = static_cast<AstUnOp*>(e);
			res_exp = eval_unop(b); 
			break;
		}

		case AST_BINOP:
		{
			AstBinOp* b = static_cast<AstBinOp*>(e);
			res_exp = eval_binop(b); 
			break;
		}

		case AST_LAMBDA:
		{
			AstLambda* lambda = static_cast<AstLambda*>(e);
			res_exp = eval_lambda(lambda, get_new_variable());
			break;
		}

		// aka application
		case AST_EXPRESSION_LIST:
		{
			// (f 1 2 3 ...)
			AstExpressionList* list = static_cast<AstExpressionList*>(e);
			vector<Expression*> expressions = list->get_expressions();

			Expression* expression0 = expressions[0];
			Expression* expression0_eval = eval(expression0);

			// there are two cases here
			// case 1: somebody just decided to put parens around a non-application just to be safe
			if (!(expression0->get_type() == AST_LAMBDA || expression0->get_type() == AST_IDENTIFIER)) {
				if (expressions.size() != 1) assert(false); // user doing some weird stuff
				res_exp = expression0_eval;
				break;
			}
			// case 2: we have an actual application and can proceed as normal

			// return value is the variable type with appropriate number of primes tacked onto it
			// if expression0 was an identifier use that as the name, otherwise we don't really care
			string name;
			if (expression0->get_type() == AST_IDENTIFIER) {
				name = static_cast<AstIdentifier*>(expression0)->get_id();
			} else {
				name = get_new_variable();
			}
			// tack size of (vector - 1) 's onto it (-1 to not include the function itself)
			for (uint32_t i=0; i<expressions.size()-1; i++) {
				name += "'";
			}
			// finally it all goes to function'''...
			VariableType* function_eval_type = VariableType::make(name);

			/*
			// put the types of the rest of the expressions into a function type
			vector<Type*> args;
			for (auto it = expressions.begin() + 1; it != expressions.end(); ++it) {
				Expression* expression = *it;
				Expression* expression_eval = eval(expression);
				args.push_back(expression_eval->type);
			}
			// finally it all evaluates to function_eval_type
			args.push_back(function_eval_type);
			// create function type arg->arg->arg->...->function'
			FunctionType* function_type = FunctionType::make(expression0->to_value(), args);
			// now take that function type and unify it with expression0's type
			assert(expression0_eval->type != nullptr);
			Type* verified_function_type = expression0_eval->type->verify(function_type);
			*/


			// the function type to verify every expression against
			FunctionType* fun = static_cast<FunctionType*>(static_cast<AstLambda*>(expression0_eval)->type);
			// for every expression being applies
			for (auto it = expressions.begin() + 1; it != expressions.end(); ++it) {
				Expression* expression = *it;
				// first evaluate it (call by value)
				Expression* expression_eval = eval(expression);
				// create a function type of (expression_eval_type -> don't care) which will be verified with fun
				vector<Type*> args;
				args.push_back(expression_eval->type);
				args.push_back(VariableType::make(get_new_variable()));
				FunctionType* expression_fun = FunctionType::make("expfun", args);
				// now verify that function's first type is the same as expfun's first type aka that function accepts the expression
				Type* new_fun = fun->verify(expression_fun);
				assert(fun != nullptr);
				// new_fun is what fun is thought to be after having expression evaluated on it (aka a more typed fun')
				assert(new_fun->get_kind() == TYPE_FUNCTION);
				fun = static_cast<FunctionType*>(new_fun);
			}


			// finally the value of the application is function'''...
			e->type = function_eval_type;
			res_exp = e;

			break;
		}

		// pretty sure this never applies cause we don't create lists, we only do cons's
		case AST_LIST:
		{
			assert(false);
			break;
		}

		// pretty sure this never applies
		case AST_IDENTIFIER_LIST:
		{
			assert(false);
			break;
		}

		default:
			assert(false);
	}
	
	cout << "+ evaluated to " << res_exp->to_value() << " with type " << res_exp->type->to_string() << endl;

	return res_exp;
}

Expression* get_test() {
	AstBranch* b = AstBranch::make(AstInt::make(1), AstInt::make(2), AstString::make("duck"));
	AstLambda* l = AstLambda::make(AstIdentifier::make("x"), b);
	vector<Expression*> exps;
	exps.push_back(AstIdentifier::make("a"));
	exps.push_back(AstInt::make(1));
	AstExpressionList* apply = AstExpressionList::make(exps);
	AstLet* ll = AstLet::make(AstIdentifier::make("a"), l, apply);
	return ll;
}

TypeInference::TypeInference(Expression* e) {
	Expression* ee = get_test();
	eval(ee);
	/*
 	Expression* eval_e = eval(e);
	cout << "final state of unification:" << endl;
	Type::print_all_types();
	cout << "passed!" << endl;
	cout << "final type rep: " << eval_e->type->find()->to_string() << endl;
	*/
}
