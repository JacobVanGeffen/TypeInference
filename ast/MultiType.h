#ifndef MUTLI_TYPE_H_
#define MUTLI_TYPE_H_

#include "VariableType.h"

#include <set>
#include <string>
#include <set>

using namespace std;

class MultiType: public Type {
private:
	MultiType(const string& name, set<Type*> allowed_types);
	Type* head = nullptr;
	Type* tail = nullptr;
	void make_ht();
    string name;
	string get_name();
	set<Type*> allowed_types;

protected:
	virtual MultiType* clone() const {
		return new MultiType(*this);
	}

public:
	static MultiType* make(set<Type*> allowed_types);
	set<Type*> get_allowed_types();
    virtual bool operator<(const Type& other);
    virtual string to_string();
    virtual Type* get_hd();
    virtual Type* get_tl();
};

#endif /* MUTLI_TYPE_H_ */
