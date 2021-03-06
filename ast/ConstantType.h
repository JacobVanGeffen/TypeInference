 #ifndef CONSTANT_TYPE_H_
 #define CONSTANT_TYPE_H_
 
#include "Type.h"
 
#include <set>
#include <string>

using namespace std;

class ConstantType: public Type {
private:
	ConstantType(const string& name);
	string name;

protected:
	virtual ConstantType* clone() const {
		return new ConstantType(*this);
	}

public:
	static ConstantType* make(const string& name);
	virtual bool operator<(const Type& other);
	virtual string to_string();
	string get_name();
};

#endif /* CONSTANT_TYPE_H_ */
