#include "ast/ConstantType.h"
#include "ast/VariableType.h"
#include "ast/FunctionType.h"
#include "ast/ListType.h"
#include "ast/expression.h"
#include "TypeInference.h"

Expression* TypeInference::eval_unop(AstUnOp* b) {
	switch (b->get_unop_type()) {
		case HD:
		case TL:
		case ISNIL:
		case PRINT:
		default:
			assert(false);
			return AstNil::make();
	}
}

Expression* eval_binop_int(Expression* b, Expression* eval_e1, Expression* eval_e2) {
	assert(false);
	return AstNil::make();
}

Expression* eval_binop_int_or_string(Expression* b, Expression* eval_e1, Expression* eval_e2) {
	assert(false);
	return AstNil::make();
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
		}
		default:
		{
			assert(false);
			return AstNil::make();
		}
	}
}

Expression* TypeInference::eval_lambda(Expression* e) {
	assert(false);
	return AstNil::make();
}

Expression* TypeInference::eval(Expression* e) {
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

		case AST_LIST:
		{
			assert(false);
			break;
		}

		case AST_IDENTIFIER:
		{
			assert(false);
			break;
		}

		case AST_LET:
		{
			assert(false);
			break;
		}

		case AST_BRANCH:
		{
			assert(false);
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
			assert(false);
			break;
		}

		case AST_IDENTIFIER_LIST:
		{
			assert(false);
			break;
		}

		case AST_EXPRESSION_LIST:
		{
			assert(false);
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

		default:
			assert(false);
	}
	return res_exp;
}


TypeInference::TypeInference(Expression* e) {
 	eval(e);
	cout << "passed!" << endl;
}
