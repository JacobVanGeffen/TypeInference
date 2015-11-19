 #ifndef CONSTANT_TYPE_H_
 #define CONSTANT_TYPE_H_
 
#include "Type.h"
 
 
#include <set>
#include <string>

using namespace std;


 class ConstantType: public Type
 {
 private:
   ConstantType(const string& name);
   string name;
   
 public:
    static ConstantType* make(const string& name);
    virtual bool operator<(const Type& other);
    virtual string to_string();
    
    
 };
 
 
 #endif /* CONSTANT_TYPE_H_ */
