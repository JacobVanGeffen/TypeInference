let f = lambda x. lambda y. x + y in
let g = (f "a") in (* Problem: Now actually unifying g with Str->Str, b/c we don't correctly get the type of (f "a") *)
(g "b")
