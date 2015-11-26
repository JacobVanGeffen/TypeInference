CC = g++
CFLAGS = -g -Wall -std=c++11
INC=-. ./ast
INC_PARAMS=$(foreach d, $(INC), -I$d)
TESTS=$(patsubst tests/given/%.l,test%,$(sort $(wildcard tests/given/?.l)))

OBJs =   parser.tab.o lex.yy.o  Expression.o SymbolTable.o frontend.o TypeInference.o AstRead.o AstNil.o AstList.o AstUnOp.o AstBranch.o AstExpressionList.o AstIdentifierList.o AstBinOp.o  AstIdentifier.o AstInt.o AstLambda.o AstLet.o AstString.o Type.o ConstantType.o VariableType.o FunctionType.o ListType.o

default: parser

parser: ${OBJs}
	${CC} ${CFLAGS} ${INC_PARAMS} ${OBJs} -o l-type-inference -lfl

frontend.o:	frontend.cpp TypeInference.cpp 
	${CC} ${CFLAGS} ${INC_PARAMS} -c frontend.cpp TypeInference.cpp 
	
SymbolTable.o:	SymbolTable.cpp
	${CC} ${CFLAGS} ${INC_PARAMS} -c SymbolTable.cpp 

Expression.o:	ast/*.h ast/*.cpp #ast/Expression.cpp ast/Expression.h ast/AstString.cpp ast/AstString.h
	${CC} ${CFLAGS} ${INC_PARAMS} -c ast/*.cpp 

test: $(TESTS);

$(TESTS): test% : parser
	@echo -n "$*.l ... "
	-@./l-type-inference tests/given/$*.l > $*.out 2>&1
	-@((grep "passed\!" $*.out) && echo "pass") || echo "failed"

clean:
	rm -f l-type-inference  *.o  parser.output

depend:
	makedepend -I. *.c
