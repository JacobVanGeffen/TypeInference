#ifndef ALPHA_TYPE_H_
#define ALPHA_TYPE_H_

#include "Type.h"

#include <set>
#include <string>
#include <vector>

using namespace std;

class AlphaType: public Type {
private:
	AlphaType();

protected:
	virtual AlphaType* clone() const {
		return new AlphaType(*this);
	}

public:
	static AlphaType* make();
	virtual bool operator<(const Type& other);
	virtual string to_string();
	virtual Type* get_tl();
};

#endif /* FUNCTION_TYPE_H_ */
