 #ifndef VARIABLE_TYPE_H_
  #define VARIABLE_TYPE_H_

#include "Type.h"

#include <set>
#include <string>

using namespace std;

class VariableType: public Type {
private:
    VariableType(const string& name);
	Type* head = nullptr;
	Type* tail = nullptr;
	void make_ht();
    string name;

protected:
	virtual VariableType* clone() const {
		return new VariableType(*this);
	}

public:
    static VariableType* make(const string& name);
	string get_name();
    virtual bool operator<(const Type& other);
    virtual string to_string();
    virtual Type* get_hd();
    virtual Type* get_tl();
};

#endif /* VARIABLE_TYPE_H_ */
