#include "MultiType.h"
#include "ListType.h"
#include "OmegaType.h"

MultiType::MultiType(const string& name, vector<Type*> allowed_types): Type(TYPE_MULTI), name(name), allowed_types(allowed_types) { }

MultiType* MultiType::make(const string& name, vector<Type*> allowed_types) {
	MultiType* t = new MultiType(name, allowed_types);
	t = static_cast<MultiType*>(get_type(t));
	return t;
}

bool MultiType::operator<(const Type& other) {
	Type& o = (Type&) other;
	if(o.get_kind() != get_kind()) return get_kind() < o.get_kind();
	MultiType& ct = static_cast<MultiType&>(o);
	return name < ct.name;
}

string MultiType::to_string() {
	return "MultiType(" + name + ")";
}

string MultiType::get_name() {
	return name;
}

vector<Type*> MultiType::get_allowed_types() {
	return allowed_types;
}

Type* MultiType::get_hd() {
	if (head == nullptr) make_ht();
	return head;
}

Type* MultiType::get_tl() {
	if (tail == nullptr) make_ht();
	return tail;
}

void MultiType::make_ht() {
	//TODO is this what we really want to do?
	//head = make(name + "-h");
	//tail = make(name + "-t");
	head = OmegaType::make();
	tail = OmegaType::make();
	unify(ListType::make(head, tail));
}
