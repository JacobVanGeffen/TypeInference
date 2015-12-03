#include "MultiType.h"
#include "ListType.h"
#include "OmegaType.h"

MultiType::MultiType(const string& name, set<Type*> allowed_types): Type(TYPE_MULTI), name(name), allowed_types(allowed_types) { }

MultiType* MultiType::make(set<Type*> allowed_types) {
	// name is congolmeration of allowed type's to_string()'s
	string name = "";
	string pad = "";
	for (auto it = allowed_types.begin(); it != allowed_types.end(); it++) {
		string type_name = (*it)->to_string();
		name += pad + type_name;
		pad = ",";
	}

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

set<Type*> MultiType::get_allowed_types() {
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
	// Assuming that this containts no MultiTypes
	set<Type*> head_types;
	set<Type*> tail_types;
	for (auto it = allowed_types.begin(); it != allowed_types.end(); it++) {
		head_types.insert((*it)->get_hd());
		tail_types.insert((*it)->get_tl());
	}
	head = make(head_types);
	tail = make(tail_types);
	unify(ListType::make(head, tail));
}
