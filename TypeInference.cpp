//TODO CHECK RETURN VALUE OF ALL UNIFYS

#include "ast/ConstantType.h"
#include "ast/VariableType.h"
#include "ast/FunctionType.h"
#include "ast/expression.h"
#include "TypeInference.h"

int typeVarCount = 0;

void report_error(Expression* e, const string & s) {
	cout << "Run-time error in expression " << e->to_value() << endl;
	cout << s << endl;
	exit(1);
}

/*
Evaluator::Evaluator() {
	sym_tab.push();
	c = 0;
}
*/

/*
Expression* TypeInference::eval_unop(AstUnOp* b) {
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

Expression* TypeInference::eval_binop(AstBinOp* b) {
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
*/

Expression* TypeInference::eval(Expression* e) {
	Expression* res_exp = NULL;
	switch (e->get_type()) {
		case AST_INT:
		{
			res_exp = e;
			res_exp->type = ConstantType::make("Int");
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

		case AST_LIST:
		{
			res_exp = e;
			// Do our cool list type stuff here 
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
			id_var->unify(e1_prime->type);
			
			res_exp = e2_prime;
			break;
		}

		case AST_BRANCH:
		{
			AstBranch* branch = static_cast<AstBranch*>(e);
			Expression* pred_prime = eval(branch->get_pred());

			pred_prime->type->unify(ConstantType::make("Int"));

			Expression* res_exp1 = eval(branch->get_then_exp());
			Expression* res_exp2 = eval(branch->get_else_exp());

			res_exp1->type->unify(res_exp2->type);
			
			res_exp = res_exp1; // could be 1 or 2, doesn't matter

			break;
		}

		case AST_UNOP:
		{
			//AstUnOp* b = static_cast<AstUnOp*>(e);
			//res_exp = eval_unop(b); TODO
			break;
		}

		case AST_BINOP:
		{
			//AstBinOp* b = static_cast<AstBinOp*>(e);
			//res_exp = eval_binop(b); TODO
			break;
		}

		case AST_LAMBDA:
		{
			AstLambda* lambda = static_cast<AstLambda*>(e);
			AstIdentifier* formal = lambda->get_formal();
			Expression* body = lambda->get_body();

			Type* id_var = VariableType::make("v" + typeVarCount);
			typeVarCount++;

			Expression* type_wrapper = AstInt::make(0);
			type_wrapper->type = id_var;

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


TypeInference::TypeInference(Expression * e) {
// CODE GOES HERE
}

/*
this->program = e;

Type* t1 = ConstantType::make("Int");
Type* t2 = ConstantType::make("String");
Type* t3 = ConstantType::make("Int");
Type* var1 = VariableType::make("x");
cout << t1->to_string() << " " << t1 << endl;
cout << t2->to_string() << " " << t2 << endl;
cout << t3->to_string() << " " << t3 << endl;
vector<Type*> v1;
v1.push_back(var1);
v1.push_back(t2);

vector<Type*> v2;
v2.push_back(t3);
v2.push_back(t2);

Type* t4 = FunctionType::make("fun", v1);
Type* t5 = FunctionType::make("fun", v2);
cout << t4->to_string() << " " << t4 << endl;
cout << t5->to_string() << " " << t5 << endl;

//prints all types & reps in table
Type::print_all_types();

Type* t1 = t4;
Type* t2 = t5;

cout << "Type 1:" << t1->to_string() << "Rep: " << t1->find()->to_string() << endl;
cout <<	"Type 2:" << t2->to_string() << "Rep: " << t2->find()->to_string() << endl;

cout << "unify: " << t1->unify(t2) << endl;

cout << "Type 1:" << t1->to_string() << "Rep: " << t1->find()->to_string() << endl;
cout <<	"Type 2:" << t2->to_string() << "Rep: " << t2->find()->to_string() << endl;

cout << "Type 1:" << t1->to_string() << "Rep: " << t1->find()->to_string() << endl;
cout <<	"Type 2:" << var1->to_string() << "Rep: " << var1->find()->to_string() << endl;


Type::print_all_types();
*/

