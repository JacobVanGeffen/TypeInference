#ifndef FUNCTION_TYPE_H_
#define FUNCTION_TYPE_H_

#include "Type.h"

#include <set>
#include <string>
#include <vector>

using namespace std;


 class ListType: public Type
 {
 private:
   ListType(Type& head, Type& tail);
   Type* head;
   Type* tail;

 public:
    static ListType* make(Type& head, Type& tail);
    virtual bool operator<(const Type& other);
    virtual string to_string();
    const Type* get_head();
    const Type* get_tail();


 };


 #endif /* FUNCTION_TYPE_H_ */
