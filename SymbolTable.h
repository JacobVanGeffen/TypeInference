#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

class AstIdentifier;
#include <vector>
#include <map>
#include <iostream>
#include <assert.h>

using namespace std;

template<typename T>
class SymbolTable {
private:
	vector<map<AstIdentifier*, T*> > values;
public:
	SymbolTable() { }

	void push() {
		map<AstIdentifier*, T*> cur_map;
		values.push_back(cur_map);
	}

	void pop() {
		if(values.size() == 0) {
			cout << "You can only pop from a symbol table that you have pushed on first" << endl;
			assert(false);
		}
		values.pop_back();
	}

	void add(AstIdentifier* id, T* t) {
		if(values.size() == 0) {
			cout << "You can only add to a symbol table that you have pushed on first" << endl;
			assert(false);
		}
		values[values.size()-1][id] = t;
	}

	T* find(AstIdentifier* id) {
		for(typename vector<map<AstIdentifier*, T*> >::reverse_iterator it = values.rbegin(); it != values.rend(); it++) {
			map<AstIdentifier*, T*> & cur = *it;
			if(cur.count(id) > 0) return cur[id];
		}
		return NULL;
	}

	void print_contents() {
		cout << "******** Symbol Table ******** " << endl;
		int i = 0;
		for(typename vector<map<AstIdentifier*, T*> >::iterator it = values.begin(); it != values.end(); it++, i++) {
			cout << "Bindings at level " << i <<  ":" << endl;
			map<AstIdentifier*, T*> & cur = *it;
			for(typename map<AstIdentifier*, T*>::iterator it2 = cur.begin(); it2 != cur.end(); it2++) {
				cout << "\t" << it2->first->to_string(1) << "   -> " << it2->second->to_string(1) << endl;
			}
		}
		cout << "******** ********* ******** " << endl;
	}

};

#endif /* SYMBOLTABLE_H_ */
