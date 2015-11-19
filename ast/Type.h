 #ifndef TYPE_H_
 #define TYPE_H_
#include <set>
#include <string>

 
 class Type;

 class TypeComparator;

using namespace std;
 enum type_kind {TYPE_CONSTANT, TYPE_FUNCTION, TYPE_VARIABLE};


 class Type
 {
 private:
   static set<Type*, TypeComparator> types;
   protected:
     type_kind tk;
     Type* parent;
     Type(type_kind tk);
     void set_parent(Type* t);
     void compute_union(Type* other);
     
     static Type* get_type(Type* t);
     
   public:

    Type* find();
    bool unify(Type* other);
    type_kind get_kind();
    virtual bool operator<(const Type& other) = 0;
    virtual string to_string() = 0;
    static void print_all_types();
    
 };
 
 
 
 class TypeComparator
{
public:
    bool operator()( Type*  s1, Type*  s2) const
    {
     
	return *s1 < *s2;
	return true;
    }
};


 
 
 #endif /* TYPE_H_ */
