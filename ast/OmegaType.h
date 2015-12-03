#ifndef OMEGA_TYPE_H_
#define OMEGA_TYPE_H_

#include "Type.h"

#include <set>
#include <string>
#include <vector>

using namespace std;

class OmegaType: public Type {
private:
	OmegaType();

protected:
	virtual OmegaType* clone() const {
		return new OmegaType(*this);
	}

public:
	static OmegaType* make();
	virtual bool operator<(const Type& other);
	virtual string to_string();
	virtual Type* get_tl();
};

#endif /* OMEGA_TYPE_H_ */
