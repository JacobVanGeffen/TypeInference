#include "FunctionType.h"


FunctionType::FunctionType(const string& name, const vector<Type*> & args):Type(TYPE_FUNCTION)
{
  this->name = name;
  this->args = args;
}
   

FunctionType* FunctionType::make(const string& name, const vector<Type*> & args)
{
  FunctionType* t = new FunctionType(name, args);
  t = static_cast<FunctionType*>(get_type(t));
  return t;
  
}

bool FunctionType::operator<(const Type& other)
{
  Type& o = (Type&) other;
  if(o.get_kind() != get_kind()) return get_kind() < o.get_kind();
  FunctionType& ct = static_cast<FunctionType&>(o);
  if(name < ct.name) return true;
  if(name > ct.name) return false;
  if(args.size() < ct.args.size()) return true;
  if(args.size() > ct.args.size()) return false;
  for(unsigned int i=0; i < args.size(); i++)
  {
    if(args[i] < ct.args[i]) return true;
    if(args[i] > ct.args[i]) return false;
  }
  return false;
}

const vector<Type*> & FunctionType::get_args()
{
  return args;
}

const string & FunctionType::get_name()
{
	return name;
}


string FunctionType::to_string()
{
  string res = name + "(";
  for(auto it = args.begin(); it != args.end(); it++)
  {
    res += (*it)->to_string();
    auto next = it;
    next++;
    if(next != args.end()) {
     res += ", "; 
    }
  }
  res += ")";
  return res;
  
  
}
