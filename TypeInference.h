#ifndef TYPE_INFERENCE_H_
#define TYPE_INFERENCE_H_

#include "SymbolTable.h"

class Expression;

class TypeInference {
private:
	Expression* program;
	SymbolTable sym_tab;
public:
	TypeInference(Expression* p);
	Expression* eval(Expression* e);
	Expression* eval_binop(AstBinOp* b);
	Expression* eval_unop(AstUnOp* b);

};

#endif /* TYPE_INFERENCE_H_ */
