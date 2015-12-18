CC = g++
CFLAGS = -g -Wall -std=c++11
INC=-. ./ast
INC_PARAMS=$(foreach d, $(INC), -I$d)
TESTS=$(patsubst tests/%.L,test%,$(sort $(wildcard tests/*.L)))

OBJs = parser.tab.o lex.yy.o Expression.o AstRead.o AstNil.o AstList.o AstUnOp.o AstBranch.o AstExpressionList.o AstIdentifierList.o AstBinOp.o  AstIdentifier.o AstInt.o AstLambda.o AstLet.o AstString.o Type.o ConstantType.o VariableType.o FunctionType.o ListType.o OmegaType.o MultiType.o
INF_OBJs = l-inference-frontend.o TypeInference.o
INT_OBJs = l-interpreter-frontend.o Evaluator.o

default: inferencer interpreter

inferencer: ${OBJs} ${INF_OBJs}
	${CC} ${CFLAGS} ${INC_PARAMS} ${OBJs} ${INF_OBJs} -o l-type-inference -lfl

interpreter: ${OBJs} ${INT_OBJs}
	${CC} ${CFLAGS} ${INC_PARAMS} ${OBJs} ${INT_OBJs} -o l-interpreter -lfl

l-inference-frontend.o:	l-inference-frontend.cpp TypeInference.cpp 
	${CC} ${CFLAGS} ${INC_PARAMS} -c l-inference-frontend.cpp TypeInference.cpp 
	
l-interpreter-frontend.o: l-interpreter-frontend.cpp Evaluator.cpp 
	${CC} ${CFLAGS} ${INC_PARAMS} -c l-interpreter-frontend.cpp Evaluator.cpp 
	
Expression.o: ast/*.h ast/*.cpp
	${CC} ${CFLAGS} ${INC_PARAMS} -c ast/*.cpp 


test: $(TESTS);

$(TESTS): test% : inference
	@echo -n "$*.L ... "
	-@./l-type-inference tests/$*.L > $*.out 2>&1 || true
	-@((grep -q "`head -1 tests/$*.L | sed 's/(\*//' | sed 's/\*)//'`" $*.out) && echo "pass") || echo "failed ***************"

clean:
	-rm -f l-interpreter l-type-inference  *.o  parser.output || true
	-rm *.out || true

depend:
	makedepend -I. *.c
