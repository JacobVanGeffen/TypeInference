#include "ast/ConstantType.h"
#include "ast/VariableType.h"
#include "ast/FunctionType.h"
#include "ast/ListType.h"
#include "ast/expression.h"
#include "TypeInference.h"

map<string, VariableType*> renames;

int var_counter = 1;
string get_new_variable() {
	return "v" + to_string(var_counter++);
}

Expression* make_wrapper(Type* type) {
	Expression* wrapper = AstInt::make(0);
	wrapper->type = type;
	return wrapper;
}

Expression* TypeInference::eval_unop(AstUnOp* b) {
	Expression* e = eval(b->get_expression());
	switch (b->get_unop_type()) {
		case HD:
			return make_wrapper(e->type->get_hd());
		case TL:
			return make_wrapper(e->type->get_tl());
		case ISNIL:
		case PRINT:
			return make_wrapper(ConstantType::make("Int"));
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
			return make_wrapper(ListType::make(eval_e1->type, eval_e2->type));
		default:
			assert(false);
			return AstNil::make();
	}
}

AstLambda* TypeInference::eval_lambda(AstLambda* lambda, const string id) {
	// lambda x. S1
	AstIdentifier* formal = lambda->get_formal();
	Expression* body = lambda->get_body();

	// rename x to generic v*
	string rename = get_new_variable();
cout << "rename " << rename << endl;
	VariableType* rename_var = VariableType::make(rename);
	renames[formal->get_id()] = rename_var;

	// make lambda'
	string lambda_prime_str = id + "'";
	VariableType* lambda_prime = VariableType::make(lambda_prime_str);

	// make lambda's function type
	vector<Type*> args;
	args.push_back(rename_var);
	args.push_back(lambda_prime);
	FunctionType* lambda_type = FunctionType::make(lambda->to_value(), args);

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
				res_exp = AstInt::make(1);
				res_exp->type = ConstantType::make("Int");
			} else {
				res_exp = AstString::make("input");
				res_exp->type = ConstantType::make("String");
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
cout << "found old thing" << endl;
cout << rename_ptr->first << " " << rename_ptr->second << endl;
				id_type = rename_ptr->second;
			} else {
cout << "creating new thing" << endl;
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

			// S1 and S2 must be same type
			assert(then_exp_eval->type != nullptr); assert(else_exp_eval->type != nullptr);
			assert(then_exp_eval->type->unify(else_exp_eval->type));

			// can return either S1 or S2, they both have same type (since we sucessfully unified)
			res_exp = then_exp_eval;

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
			// tack size of vector - 1 primes onto it (-1 to not include the function itself)
			for (uint32_t i=0; i<expressions.size()-1; i++) {
				name += "'";
			}
			// finally it all goes to function'
			VariableType* function_eval_type = VariableType::make(name);

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
			assert(expression0_eval->type->unify(function_type));

			// finally the value of the application is function'
			assert(expression0_eval->type != nullptr);
			assert(expression0_eval->type->unify(function_eval_type));

			// set res_exp
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


TypeInference::TypeInference(Expression* e) {
 	eval(e);
	cout << "passed!" << endl;
}
