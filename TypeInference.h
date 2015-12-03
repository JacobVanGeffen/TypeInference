#ifndef TYPE_INFERENCE_H_
#define TYPE_INFERENCE_H_

#include "ast/VariableType.h"
#include "SymbolTable.h"

class Expression;

class TypeInference {
private:
	SymbolTable<VariableType> gamma;
	Expression* program;
public:
	TypeInference(Expression* p);
	Expression* eval(Expression* e);
	Expression* eval_binop(AstBinOp* b);
	Expression* eval_unop(AstUnOp* b);
	AstLambda* eval_lambda(AstLambda* lambda, const string id);
};

#endif /* TYPE_INFERENCE_H_ */
