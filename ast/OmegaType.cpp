#include "OmegaType.h"

OmegaType::OmegaType():Type(TYPE_OMEGA) {}

OmegaType* OmegaType::make() {
	OmegaType* t = new OmegaType();
	t = static_cast<OmegaType*>(get_type(t));
	return t;
}

bool OmegaType::operator<(const Type& other) {
	Type& o = (Type&) other;
	if(o.get_kind() != get_kind()) return get_kind() < o.get_kind();
	return true;
}

string OmegaType::to_string() {
	return "OmegaType";
}

Type* OmegaType::get_tl() {
	return this;
}
