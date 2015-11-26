#include "ListType.h"

ListType::ListType(Type* head, Type* tail):Type(TYPE_LIST) {
	this->head = head;
	this->tail = tail;
}

ListType* ListType::make(Type* head, Type* tail) {
	ListType* t = new ListType(head, tail);
	t = static_cast<ListType*>(get_type(t));
	return t;
}

bool ListType::operator<(const Type& other) {
	Type& o = (Type&) other;
	if(o.get_kind() != get_kind()) return get_kind() < o.get_kind();
	ListType& ct = static_cast<ListType&>(o);
	if(head < ct.head) return true;
	return tail < ct.tail;
}

const Type* ListType::get_head() {
	return head;
}

const Type* ListType::get_tail() {
	return tail;
}

string ListType::to_string() {
	string res = "[";
	res += head->to_string();
	res += ", ";
	res += tail->to_string();
	res += "]";
	return res;
}
