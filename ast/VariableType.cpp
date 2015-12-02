#include "VariableType.h"
#include "ListType.h"

VariableType::VariableType(const string& name):Type(TYPE_VARIABLE) {
	this->name = name;
}

VariableType* VariableType::make(const string& name) {
	VariableType* t = new VariableType(name);
	t = static_cast<VariableType*>(get_type(t));
	return t;
}

bool VariableType::operator<(const Type& other) {
	Type& o = (Type&) other;
	if(o.get_kind() != get_kind()) return get_kind() < o.get_kind();
	VariableType& ct = static_cast<VariableType&>(o);
	return name < ct.name;
}

string VariableType::to_string() {
	return "VariableType(" + name + ")";
}

string VariableType::get_name() {
	return name;
}

Type* VariableType::get_hd() {
	if (head == nullptr) make_ht();
	return head;
}

Type* VariableType::get_tl() {
	if (tail == nullptr) make_ht();
	return tail;
}

void VariableType::make_ht() {
	head = make(name + "-h");
	tail = make(name + "-t");
	unify(ListType::make(head, tail));
}
