# Type Inference over L
This project, done for [Dr. Thomas Dillig's](http://www.cs.utexas.edu/~tdillig/) [CS345h (programming languages)](http://www.cs.utexas.edu/~tdillig/cs345H/), demonstrates static type inference over a dummy language, L.

## L
Most of the details of the L programming language, including the formal syntax and semantics, can be found in the L-reference pdf. A brief description of L follows.

### Overview of L
A primitive language heavily based on lambda calculus, L does not quite require use of the Y-combinator. Basic arithmetic expressions evaluate as expected; `(3+6-1)*2` evaluates to `16`. Conditional expressions are of the form `if e1 then e2 else e3`, where a non-zero `e1` causes the whole if statement to evaluate to the value of `e2`, otherwise it evaluates to the value of `e3`. The logical operators `=`, `<>`, `<`, ,`<=`, `>`, `>=`, `&`, and `|` evaluate as expected. Variables can be assigned with a let binding in the form `let x = e1 in e2`. Lambda expressions are of the form `lambda x1, x2, ..., xn . e`. Function definitions are a shortcut for a let binding to a lambda and are of the form `fun f with x1, ..., xn = e in e'`. List operators are `@` for concatenation (`1@2` makes the list `[1, 2]`), `isNil e`, `!e` for head, and `#e` for tail.

### Running L
`make interpreter` to create the l-interpreter binary. `./l-interpreter [L-file]` will print either the value of the program or a runtime error to stdout.

## Type Inference
The type inference is detailed in the TypeInference pdf. We implement basic type checking with type inference over all primitive types. As an example, the program `fun f with x = "hello " + x in (f 5)` will be rejected by the type checker because the type of f will be inferred as `String->String` and will fail when an `Int1 is applied. Our type system also allows for completely polymorphic lists, such as `[5, ["hello", lambda x. x]]`. In addition, our type system is sound, so any program that makes it through our type inference is guaranteed not to have any runtime errors. The proofs are presented in TypeInference pdf. Additionally, there are a couple of changes to the semantics that we make that are presented in the pdf.

### Running Type Inference
`make inferencer` to create the l-type-inference binary. `./l-type-inference [L-file]` will either output passed at the end or will output an error message. Additionally, `make test` will run the tests for the type inferencer located in the test directory.
