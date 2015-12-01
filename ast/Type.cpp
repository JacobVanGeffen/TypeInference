#include "Type.h"
#include "FunctionType.h"
#include "ConstantType.h"
#include "ListType.h"
#include <iostream>
using namespace std;

set<Type*, TypeComparator> Type::types;
Type* nil_type = ConstantType::make("Nil"); // Do we need to make this static/constant?

void Type::print_all_types() {
	cout << "************* All current types ********************" << endl;
	for(auto it = types.begin(); it != types.end(); it++) {
		cout << (*it)->to_string() << " Representative: " <<  (*it)->find()->to_string() << endl;
	}
	cout << "****************************************************" << endl;
}

Type* Type::get_type(Type* t) {
    if(types.count(t) > 0) {
		Type* res = (Type*)*types.find(t);
		delete t;
		return res;
    }
    types.insert(t);
    return t;
}

Type::Type(type_kind tk) {
	this->tk = tk;
	parent = this;
}

Type* Type::get_hd() {
	return this;
}

Type* Type::get_tl() {
	return nil_type;
}

type_kind Type::get_kind() {
	return tk;
}

void Type::set_parent(Type* t) {
	parent = t;
}

Type* Type::find() {
	Type* old = parent;
	Type* t = old->parent;
	while(old != t) {
		old = t;
		t = t->parent;
	}
	return t;
}

void Type::compute_union(Type* other) {
	Type* t1 = this->find();
	Type* t2 = other->find();
	if(t1->tk == TYPE_CONSTANT) {
		t2->set_parent(t1);
		return;
	}
	if(t2->tk == TYPE_CONSTANT) {
		t1->set_parent(t2);
		return;
	}
	if(t1->tk == TYPE_LIST) {
		t2->set_parent(t1);
		return;
	}
	if(t2->tk == TYPE_LIST) {
		t1->set_parent(t2);
		return;
	}
	if(t1->tk == TYPE_FUNCTION) {
		t2->set_parent(t1);
		return;
	}
	if(t2->tk == TYPE_FUNCTION) {
		t1->set_parent(t2);
		return;
	}
	t2->set_parent(t1);
}

bool Type::unify(Type* other) {
	cout << "UNIFYING " << to_string() << " with " << other->to_string() << endl;
	Type* t1 = this->find();
	Type* t2 = other->find();
	cout << "reps:    " << t1->to_string() << "  ##  " << t2->to_string() << endl;
	if(t1 == t2) return true;

	if (t1->tk == TYPE_ALPHA) {
		if (t2->tk != TYPE_VARIABLE) return false;
		t1->compute_union(t2);
		return true;
	}
	if (t2->tk == TYPE_ALPHA) {
		if (t1->tk != TYPE_VARIABLE) return false;
		t2->compute_union(t1);
		return true;
	}

	if(t1->tk == TYPE_FUNCTION && t2->tk == TYPE_FUNCTION) {
		t1->compute_union(t2);
		FunctionType* f1 = static_cast<FunctionType*>(t1);
		FunctionType* f2 = static_cast<FunctionType*>(t2);
		const vector<Type*> & arg1 = f1->get_args();
		const vector<Type*> & arg2 = f2->get_args();
		size_t size = min(arg1.size(), arg2.size()) - 1;
		size_t i = 0;
		for(; i < size; ++i) {
			if(arg1[i]->unify(arg2[i]) == false) return false;
		}
		vector<Type*> max;
		vector<Type*> min;
		if (arg1.size() >= arg2.size()) {
			max = (vector<Type*>)arg1;
			min = (vector<Type*>)arg2;
		} else {
			max = (vector<Type*>)arg2;
			min = (vector<Type*>)arg1;
		}
		vector<Type*> args;
		Type* singleton = min[i];
		for (;i<max.size();++i) {
			args.push_back(max[i]);
		}
		Type* rest;
		if (args.size() == 1) {
			rest = args[0];
		} else {
			rest = FunctionType::make("rest", args);
		}
		return singleton->unify(rest);
	}
	if(t1->tk == TYPE_LIST && t2->tk == TYPE_LIST) {
		t1->compute_union(t2);
		Type* hd1 = t1->get_hd();
		Type* tl1 = t1->get_tl();
		Type* hd2 = t2->get_hd();
		Type* tl2 = t2->get_tl();
		return hd1->unify(hd2) && tl1->unify(tl2);
	}
	if (t1->tk == TYPE_LIST || t2->tk == TYPE_LIST) { // only one of them is a list, so the other one is ConstantType or a FunctionType
		if (t2->tk == TYPE_LIST) {
			auto temp = t2;
			t2 = t1;
			t1 = temp;
		}
		// t1 is the list type
		return t1->get_hd()->unify(t2) && t1->get_tl()->unify(ConstantType::make("Nil"));
	}
	if(t1->tk == TYPE_VARIABLE || t2->tk == TYPE_VARIABLE) {
		t1->compute_union(t2);
		return true;
	}


	return false;
}

bool Type::verify(Type* other) {
	set<Type*, TypeComparator> backup;
	for (auto it=types.begin(); it!=types.end(); it++) {
		Type* t = *it;
		Type* t_cp = new Type(t);
		backup.insert(t_cp);
	}
	bool res = unify(other);
	types = backup;
	return res;
}
