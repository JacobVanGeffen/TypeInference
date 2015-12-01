#include "AlphaType.h"

AlphaType::AlphaType():Type(TYPE_ALPHA) {}

AlphaType* AlphaType::make() {
	AlphaType* t = new AlphaType();
	t = static_cast<AlphaType*>(get_type(t));
	return t;
}

bool AlphaType::operator<(const Type& other) {
	Type& o = (Type&) other;
	if(o.get_kind() != get_kind()) return get_kind() < o.get_kind();
	return true;
}

string AlphaType::to_string() {
	return "AlphaType";
}

Type* AlphaType::get_tl() {
	return this;
}
