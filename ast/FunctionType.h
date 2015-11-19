 #ifndef FUNCTION_TYPE_H_
 #define FUNCTION_TYPE_H_
 
#include "Type.h"
 
 
#include <set>
#include <string>
#include <vector>

using namespace std;


 class FunctionType: public Type
 {
 private:
   FunctionType(const string& name, const vector<Type*> & args);
   string name;
   vector<Type*> args;
   
 public:
    static FunctionType* make(const string& name, const vector<Type*> & args);
    virtual bool operator<(const Type& other);
    virtual string to_string();
    const string & get_name();
    const vector<Type*> & get_args();
    
    
 };
 
 
 #endif /* FUNCTION_TYPE_H_ */
