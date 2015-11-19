#include "Type.h"
#include "FunctionType.h"
#include <iostream>
using namespace std;

set<Type*, TypeComparator> Type::types;

void Type::print_all_types()
{
	cout << "************* All current types ********************" << endl;
	for(auto it = types.begin(); it != types.end(); it++)
	{
		cout << (*it)->to_string() << " Representative: " <<  (*it)->find()->to_string() << endl;
	}
	cout << "****************************************************" << endl;
}


Type* Type::get_type(Type* t)
{
    if(types.count(t) > 0)
    {
	Type* res = (Type*)*types.find(t);
	delete t;
	return res;
    }
    types.insert(t);
    return t;
}

Type::Type(type_kind tk)
{
  this->tk = tk;
  parent = this;
}

type_kind Type::get_kind()
{
  return tk;
}


void Type::set_parent(Type* t)
{
  parent = t;
}
     
Type* Type::find()
{

  Type* old = parent;
  Type* t = old->parent;
  while(old != t)
  {
      old = t;
      t = t->parent;
  }
  return t;
}

void Type::compute_union(Type* other)
{
  Type* t1 = this->find();
  Type* t2 = other->find();
  if(t1->tk == TYPE_CONSTANT)
  {
      t2->set_parent(t1);
      return;
  }
  if(t2->tk == TYPE_CONSTANT)
  {
      t1->set_parent(t2);
      return;
  }
  if(t1->tk == TYPE_FUNCTION)
  {
      t2->set_parent(t1);
      return;
  }
  if(t2->tk == TYPE_FUNCTION)
  {
      t1->set_parent(t2);
      return;
  }
  t2->set_parent(t1);
  
}


bool Type::unify(Type* other)
{
  Type* t1 = this->find();
  Type* t2 = other->find();
  if(t1 == t2) return true;
  
  if(t1->tk == TYPE_FUNCTION && t2->tk == TYPE_FUNCTION)
  {
      t1->compute_union(t2);
	FunctionType* f1 = static_cast<FunctionType*>(t1);
	FunctionType* f2 = static_cast<FunctionType*>(t2);
	if(f1->get_name() != f2->get_name()) return false;
	const vector<Type*> & arg1 = f1->get_args();
	const vector<Type*> & arg2 = f2->get_args();
	if(arg1.size() != arg2.size()) return false;
	for(unsigned int i = 0; i < arg1.size(); i++)
	{
		if(arg1[i]->unify(arg2[i]) == false) return false;
	}
	return true;
  }
  if(t1->tk == TYPE_VARIABLE || t2->tk == TYPE_VARIABLE) {
      t1->compute_union(t2);
      return true;
  }
  
  return false;
  
  
  
}
