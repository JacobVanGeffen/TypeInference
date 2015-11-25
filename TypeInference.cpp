//TODO CHECK RETURN VALUE OF ALL UNIFYS

#include "ast/ConstantType.h"
#include "ast/VariableType.h"
#include "ast/FunctionType.h"
#include "ast/ListType.h"
#include "ast/expression.h"
#include "TypeInference.h"

int typeVarCount = 0;
void report_error(Expression* e, const string & s) {
	cout << "Run-time error in expression " << e->to_value() << endl;
	cout << s << endl;
	exit(1);
}

Expression* get_wrapper(Type* type) {
	Expression* wrapper = AstInt::make(0);
	wrapper->type = type;
	return wrapper;
}

// TODO make types work w/ lists
Expression* TypeInference::eval_unop(AstUnOp* b) {
	Expression* e = b->get_expression();
	Expression* eval_e = eval(e);

	// TODO fancy list things, should get errors right now
	switch (b->get_unop_type()) {
		case HD: // TODO NOT YET SUPPORTED
		{
			// Assume all types are lists
			// return get_wrapper(eval_e->head);
		}
		case TL: // TODO NOT YET SUPPORTED
		{
			// Assume all types are lists
			// return get_wrapper(eval_e->tail);
		}
		case ISNIL:
		{
			return get_wrapper(ConstantType::make("Int"));
		}
		case PRINT:
		{
			return get_wrapper(ConstantType::make("Int"));
		}
		default:
			assert(false);
	}
}

// TODO check these two

Expression* eval_binop_int(Expression* b, Expression* eval_e1, Expression* eval_e2) {
	// UNIFY
	assert(eval_e1->type->unify(ConstantType::make("Int")));
	assert(eval_e2->type->unify(ConstantType::make("Int")));
	return get_wrapper(ConstantType::make("Int"));
}

// Problem: Allows list + list
Expression* eval_binop_int_or_string(Expression* b, Expression* eval_e1, Expression* eval_e2) {
	// UNIFY
	assert(eval_e1->type->unify(eval_e2->type));
	return eval_e1;
}

Expression* TypeInference::eval_binop(AstBinOp* b) {
	Expression* e1 = b->get_first();
	Expression* e2 = b->get_second();
	Expression* eval_e1 = eval(e1);
	Expression* eval_e2 = eval(e2);

	switch (b->get_binop_type()) {
		case EQ:
		case NEQ:
		case PLUS:
			return eval_binop_int_or_string(b, eval_e1, eval_e2);
		case MINUS:
		case TIMES:
		case DIVIDE:
		case AND:
		case OR:
		case LT:
		case LEQ:
		case GT:
		case GEQ:
			return eval_binop_int(b, eval_e1, eval_e2);
		case CONS:
		{
			return get_wrapper(ListType::make(eval_e1->type, eval_e2->type));
		}
		default:
		{
			assert(false);
			return AstNil::make();
		}
	}
}
/**/

Expression* TypeInference::eval(Expression* e) {
	Expression* res_exp = NULL;
	switch (e->get_type()) {
		case AST_INT:
		{
			res_exp = e;
			if (res_exp->type == nullptr) {
				res_exp->type = ConstantType::make("Int");
			}
			break;
		}

		case AST_STRING:
		{
			res_exp = e;
			res_exp->type = ConstantType::make("String");
			break;
		}

		case AST_NIL:
		{
			res_exp = e;
			res_exp->type = ConstantType::make("Nil");
			break;
		}

		// NOTE: Shouldn't really ever reach this point
		case AST_LIST:
		{
			res_exp = e;
			// Evaluate the head and tail? (probs not)
			AstList* list = static_cast<AstList*>(e);
			res_exp->type = ListType::make(list->get_hd()->type, list->get_tl()->type);
			break;
		}

		case AST_IDENTIFIER:
		{
			AstIdentifier* id = static_cast<AstIdentifier*>(e);
			Expression* found = sym_tab.find(id);
			if (found == NULL) {
				string id_s = id->get_id();
				report_error(e, "Identifier " + id_s + " is not bound in current context "); // Can we make this a compile error?
			}
			res_exp = sym_tab.find(id); // Get type in Gamma
			break;
		}

		case AST_LET:
		{
			AstLet* let = static_cast<AstLet*>(e);
			Expression* e1_prime = eval(let->get_val());
			sym_tab.push();
			sym_tab.add(let->get_id(), e1_prime);
			Expression* e2_prime = eval(let->get_body());
			sym_tab.pop();
			
			// Type Inference 
			Type* id_var = VariableType::make("v" + typeVarCount);
			typeVarCount++;
			
			// UNIFY
			assert(id_var->unify(e1_prime->type));
			
			res_exp = e2_prime;
			break;
		}

		case AST_BRANCH:
		{
			cout << "branch" << endl;
			AstBranch* branch = static_cast<AstBranch*>(e);
			Expression* pred_prime = eval(branch->get_pred());

			// UNIFY
			assert(pred_prime->type->unify(ConstantType::make("Int")));

			Expression* res_exp1 = eval(branch->get_then_exp());
			Expression* res_exp2 = eval(branch->get_else_exp());

			// UNIFY
			assert(res_exp1->type->unify(res_exp2->type));
			
			res_exp = res_exp1; // could be 1 or 2, doesn't matter

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

		// Typed (theoretically)
		case AST_LAMBDA:
		{
			AstLambda* lambda = static_cast<AstLambda*>(e);
			AstIdentifier* formal = lambda->get_formal();
			Expression* body = lambda->get_body();

			Type* id_var = VariableType::make("v" + typeVarCount);
			typeVarCount++;

			Expression* type_wrapper = get_wrapper(id_var);

			// Need to push/pop? (almost def not) (aka sam thinks not)
			sym_tab.add(formal, type_wrapper);

			Expression* body_eval = eval(body);

			vector<Type*> args;
			args.push_back(id_var);
			args.push_back(body_eval->type);
			Type* function_type = FunctionType::make(lambda->to_string(), args);

			type_wrapper = AstInt::make(0);
			type_wrapper->type = function_type;

			res_exp = type_wrapper;
			break;
		}

		case AST_IDENTIFIER_LIST:
		{
			assert(false);
			break;
		}

		// Typed
		case AST_EXPRESSION_LIST:
		{
			AstExpressionList* el = static_cast<AstExpressionList*>(e);
			const vector<Expression*> expressions = el->get_expressions();
			Expression* eval_e1 = eval(expressions[0]);

			// Evaluate expression lists of size 1
			if (expressions.size() == 1) {
				return eval_e1;
			}

			Expression* eval_e2 = eval(expressions[1]);
			FunctionType* funType = static_cast<FunctionType*>(eval_e1->type);

			// UNIFY
			funType->get_args()[0]->unify(eval_e2->type);

			vector<Type*> new_types = vector<Type*>(funType->get_args().begin() + 1, funType->get_args().end());
			Type* resultType = FunctionType::make("f" + typeVarCount, new_types);
			typeVarCount++;

			if (expressions.size() > 2) {
				vector<Expression*> new_expressions = vector<Expression*>(expressions.begin() + 2, expressions.end());
				new_expressions.insert(new_expressions.begin(), get_wrapper(resultType));
				AstExpressionList* new_el = AstExpressionList::make(new_expressions);
				res_exp = eval(new_el);
			} else {
				res_exp = get_wrapper(resultType);
			}
			break;
		}

		// Typed
		case AST_READ:
		{
			AstRead* r = static_cast<AstRead*>(e);
			string input = "0";
			if (r->read_integer()) {
				res_exp = AstInt::make(string_to_int(input));
				res_exp->type = ConstantType::make("Int");
			} else {
				res_exp = AstString::make(input);
				res_exp->type = ConstantType::make("String");
			}
			break;
		}

		default:
			assert(false);
	}
	return res_exp;
}


TypeInference::TypeInference(Expression* e) {
    // TODO CODE GOES HERE
	sym_tab.push();
 	eval(e);
	cout << "Passed!" << endl;
}
