/*
 *  ___               __ __                             _ 
 * / __> ___ ._ _ _  |  \  \ ___  _ _ ._ _  ___  _ _  _| |
 * \__ \<_> || ' ' | |     |<_> || | || ' |<_> || '_>/ . |
 * <___/<___||_|_|_| |_|_|_|<___|`_. ||_|_|<___||_|  \___|
 *                               <___'                     
 * swm668, samwmaynard@gmail.com
 *    ___                                              ___                     _               _  _  
 *   /   \     _ _   _ _     __ _    __ __     o O O  / __|   __ _     ___    | |_      _ _   | || | 
 *   | - |    | '_| | ' \   / _` |   \ V /    o       \__ \  / _` |   (_-<    |  _|    | '_|   \_, | 
 *   |_|_|   _|_|_  |_||_|  \__,_|   _\_/_   TS__[O]  |___/  \__,_|   /__/_   _\__|   _|_|_   _|__/  
 * _|"""""|_|"""""|_|"""""|_|"""""|_|"""""| {======|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_| """"| 
 * "`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'./o--000'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-' 
 * as67648, arnavsastry@gmail.com
 */
#include <functional>
#include "Evaluator.h"
#include "ast/expression.h"

/*
 * This skeleton currently only contains code to handle integer constants, print and read. 
 * It is your job to handle all of the rest of the L language.
 */

/*
 * Call this function to report any run-time errors
 * This will abort execution.
 */
void report_error(Expression* e, const string & s) {
	cout << "Run-time error in expression " << e->to_value() << endl;
	cout << s << endl;
	exit(1);
}

Evaluator::Evaluator() {
	sym_tab.push();
	c = 0;
}

Expression* Evaluator::eval_unop(AstUnOp* b) {
	Expression* e = b->get_expression();
	Expression* eval_e = eval(e);

	switch (b->get_unop_type()) {
		case HD:
		{
			if (eval_e->get_type() == AST_LIST) {
				AstList* l = static_cast<AstList*>(eval_e);
				return l->get_hd();
			} else {
				return eval_e;
			}
		}
		case TL:
		{
			if (eval_e->get_type() == AST_LIST) {
				AstList* l = static_cast<AstList*>(eval_e);
				return l->get_tl();
			} else {
				return AstNil::make();
			}
		}
		case ISNIL:
		{
			if (eval_e->get_type() == AST_NIL) {
				return AstInt::make(1);
			} else {
				return AstInt::make(0);
			}
		}
		case PRINT:
		{
			if(eval_e->get_type() == AST_STRING) {
				AstString* s = static_cast<AstString*>(eval_e);
				cout << s->get_string() << endl;
			} else {
				cout << eval_e->to_value() << endl;
			}
			return AstInt::make(0);
		}
		default:
			assert(false);
	}
}

Expression* eval_binop_int(Expression* b, Expression* eval_e1, Expression* eval_e2, string x, const std::function<int(int, int)>& lambda) {
	if (eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT) {
		AstInt* i1 = static_cast<AstInt*>(eval_e1);
		AstInt* i2 = static_cast<AstInt*>(eval_e2);
		int c1 = i1->get_int();
		int c2 = i2->get_int();

		return AstInt::make(lambda(c1, c2));
	} else {
		if (eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST) {
			report_error(b, "Binpo @ is the only legal binop for lists");
		} else if (eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL) {
			report_error(b, "Nil can only be used with binop @");
		} else if (eval_e1->get_type() != eval_e2->get_type()) {
			report_error(b, "Binop can only be applied to expressions of same type");
		} else if (eval_e1->get_type() == AST_STRING && eval_e2->get_type() == AST_STRING) {
			report_error(b, "Binop " + x + " cannot be applied to strings");
		}
		assert(false);
	}
}

Expression* eval_binop_int_or_string(Expression* b, Expression* eval_e1, Expression* eval_e2, const std::function<int(int, int)>& int_lambda, const std::function<bool(string, string)>& string_lambda) {
	if (eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT) {
		AstInt* i1 = static_cast<AstInt*>(eval_e1);
		AstInt* i2 = static_cast<AstInt*>(eval_e2);
		int c1 = i1->get_int();
		int c2 = i2->get_int();

		return AstInt::make(int_lambda(c1, c2));
	} else if (eval_e1->get_type() == AST_STRING && eval_e2->get_type() == AST_STRING){
		AstString* s1 = static_cast<AstString*>(eval_e1);
		AstString* s2 = static_cast<AstString*>(eval_e2);
		string ss1 = s1->get_string();
		string ss2 = s2->get_string();

		return AstInt::make(string_lambda(ss1, ss2));
	} else {
		if (eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST) {
			report_error(b, "Binpo @ is the only legal binop for lists");
		} else if (eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL) {
			report_error(b, "Nil can only be used with binop @");
		} else if (eval_e1->get_type() != eval_e2->get_type()) {
			report_error(b, "Binop can only be applied to expressions of same type");
		}
		assert(false);
	}
}

Expression* Evaluator::eval_binop(AstBinOp* b) {
	Expression* e1 = b->get_first();
	Expression* e2 = b->get_second();
	Expression* eval_e1 = eval(e1);
	Expression* eval_e2 = eval(e2);

	switch (b->get_binop_type()) {
		case PLUS:
		{
			if (eval_e1->get_type() == AST_INT && eval_e2->get_type() == AST_INT) {
				AstInt* i1 = static_cast<AstInt*>(eval_e1);
				AstInt* i2 = static_cast<AstInt*>(eval_e2);
				int c1 = i1->get_int();
				int c2 = i2->get_int();
				return AstInt::make(c1 + c2);
			} else if (eval_e1->get_type() == AST_STRING && eval_e2->get_type() == AST_STRING){
				AstString* s1 = static_cast<AstString*>(eval_e1);
				AstString* s2 = static_cast<AstString*>(eval_e2);
				string ss1 = s1->get_string();
				string ss2 = s2->get_string();

				return AstString::make(ss1 + ss2);
			} else {
				if (eval_e1->get_type() == AST_LIST || eval_e2->get_type() == AST_LIST) {
					report_error(b, "Binpo @ is the only legal binop for lists");
				} else if (eval_e1->get_type() == AST_NIL || eval_e2->get_type() == AST_NIL) {
					report_error(b, "Nil can only be used with binop @");
				} else if (eval_e1->get_type() != eval_e2->get_type()) {
					report_error(b, "Binop can only be applied to expressions of same type");
				}
				assert(false);
			}
			break;
		}

		case MINUS:
			return eval_binop_int(b, eval_e1, eval_e2, "-", [](int x, int y) { return x - y; });
		case TIMES:
			return eval_binop_int(b, eval_e1, eval_e2, "*", [](int x, int y) { return x * y; });
		case DIVIDE:
			return eval_binop_int(b, eval_e1, eval_e2, "/", [](int x, int y) { return x / y; });
		case AND:
			return eval_binop_int(b, eval_e1, eval_e2, "&", [](int x, int y) { return x && y; });
		case OR:
			return eval_binop_int(b, eval_e1, eval_e2, "|", [](int x, int y) { return x || y; });
		case EQ:
			return eval_binop_int_or_string(b, eval_e1, eval_e2, [](int x, int y) { return x == y; }, [](string x, string y) { return x == y; });
		case NEQ:
			return eval_binop_int_or_string(b, eval_e1, eval_e2, [](int x, int y) { return x != y; }, [](string x, string y) { return x != y; });
		case LT:
			return eval_binop_int(b, eval_e1, eval_e2, "<", [](int x, int y) { return x < y; });
		case LEQ:
			return eval_binop_int(b, eval_e1, eval_e2, "<=", [](int x, int y) { return x <= y; });
		case GT:
			return eval_binop_int(b, eval_e1, eval_e2, ">", [](int x, int y) { return x > y; });
		case GEQ:
			return eval_binop_int(b, eval_e1, eval_e2, ">=", [](int x, int y) { return x >= y; });
		case CONS:
		{
			if (eval_e2->get_type() != AST_NIL) {
				return AstList::make(eval_e1, eval_e2);
			} else {
				return eval_e1;
			}
		}
		default:
		{
			assert(false);
			return AstNil::make();
		}
	}
}

Expression* Evaluator::eval(Expression* e) {
	Expression* res_exp = NULL;
	switch (e->get_type()) {
		case AST_INT:
		{
			res_exp = e;
			break;
		}

		case AST_STRING:
		{
			res_exp = e;
			break;
		}

		case AST_NIL:
		{
			res_exp = e;
			break;
		}

		case AST_LIST:
		{
			res_exp = e;
			break;
		}

		case AST_IDENTIFIER:
		{
			AstIdentifier* id = static_cast<AstIdentifier*>(e);
			Expression* found = sym_tab.find(id);
			if (found == NULL) {
				string id_s = id->get_id();
				report_error(e, "Identifier " + id_s + " is not bound in current context ");
			}
			res_exp = sym_tab.find(id);
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

			res_exp = e2_prime;
			break;
		}

		case AST_BRANCH:
		{
			AstBranch* branch = static_cast<AstBranch*>(e);
			Expression* pred_prime = eval(branch->get_pred());
			if (pred_prime->get_type() != AST_INT) {
				report_error(e, "Predicate in conditional must be an integer");
			}
			AstInt* pred_int = static_cast<AstInt*>(pred_prime);

			if (pred_int->get_int() != 0) {
				res_exp = eval(branch->get_then_exp());
			} else {
				res_exp = eval(branch->get_else_exp());
			}
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
			res_exp = e;
			break;
		}

		case AST_IDENTIFIER_LIST:
		{
			assert(false);
			break;
		}

		case AST_EXPRESSION_LIST:
		{
			AstExpressionList* el = static_cast<AstExpressionList*>(e);
			const vector<Expression*> expressions = el->get_expressions();
			Expression* eval_e1 = eval(expressions[0]);

			// Evaluate expression lists of size 1
			if (expressions.size() == 1) {
				return eval_e1;
			}

			if (eval_e1->get_type() != AST_LAMBDA) {
				report_error(e, "Only lambda expressions can be applied to other expressions");
			}

			AstLambda* lambda = static_cast<AstLambda*>(eval_e1);
			AstIdentifier* identifier = lambda->get_formal();
			Expression* body = lambda->get_body();
			Expression* sub = body->substitute(identifier, expressions[1]);
			Expression* sub_eval = eval(sub);

			if (expressions.size() > 2) {
				vector<Expression*> new_expressions = vector<Expression*>(expressions.begin() + 2, expressions.end());
				new_expressions.insert(new_expressions.begin(), sub_eval);
				AstExpressionList* new_el = AstExpressionList::make(new_expressions);
				res_exp = eval(new_el);
			} else {
				res_exp = sub_eval;
			}
			break;
		}

		case AST_READ:
		{
			AstRead* r = static_cast<AstRead*>(e);
			string input;
			getline(cin, input);
			if (r->read_integer()) {
				res_exp = AstInt::make(string_to_int(input));
			} else {
				res_exp = AstString::make(input);
			}
			break;
		}

		default:
			assert(false);
	}
	return res_exp;
}
