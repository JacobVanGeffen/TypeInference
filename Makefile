CC = g++
CFLAGS = -g -Wall -std=c++11
INC=-. ./ast
INC_PARAMS=$(foreach d, $(INC), -I$d)
TESTS=$(patsubst tests/%.L,test%,$(sort $(wildcard tests/*.L)))

OBJs = parser.tab.o lex.yy.o Expression.o frontend.o TypeInference.o AstRead.o AstNil.o AstList.o AstUnOp.o AstBranch.o AstExpressionList.o AstIdentifierList.o AstBinOp.o  AstIdentifier.o AstInt.o AstLambda.o AstLet.o AstString.o Type.o ConstantType.o VariableType.o FunctionType.o ListType.o AlphaType.o

default: inference

inference: ${OBJs}
	${CC} ${CFLAGS} ${INC_PARAMS} ${OBJs} -o l-type-inference -lfl

frontend.o:	frontend.cpp TypeInference.cpp 
	${CC} ${CFLAGS} ${INC_PARAMS} -c frontend.cpp TypeInference.cpp 
	
Expression.o: ast/*.h ast/*.cpp #ast/Expression.cpp ast/Expression.h ast/AstString.cpp ast/AstString.h
	${CC} ${CFLAGS} ${INC_PARAMS} -c ast/*.cpp 


test: $(TESTS);

$(TESTS): test% : inference
	@echo -n "$*.L ... "
	-@./l-type-inference tests/$*.L > $*.out 2>&1 || true
	-@((grep -q "`head -1 tests/$*.L | sed 's/(\*//' | sed 's/\*)//'`" $*.out) && echo "pass") || echo "failed"

clean:
	-rm -f l-type-inference  *.o  parser.output || true
	-rm *.out || true

depend:
	makedepend -I. *.c
