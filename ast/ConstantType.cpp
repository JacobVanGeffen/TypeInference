#include "ConstantType.h"


ConstantType::ConstantType(const string& name):Type(TYPE_CONSTANT)
{
  this->name = name;
}
   

ConstantType* ConstantType::make(const string& name)
{
  ConstantType* t = new ConstantType(name);
  t = static_cast<ConstantType*>(get_type(t));
  return t;
  
}

bool ConstantType::operator<(const Type& other)
{
  Type& o = (Type&) other;
  if(o.get_kind() != get_kind()) return get_kind() < o.get_kind();
  ConstantType& ct = static_cast<ConstantType&>(o);
  return name < ct.name;
}


string ConstantType::to_string()
{
  return "ConstantType(" + name + ")";
}