/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010,2012.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <string>
#include <cstring>
#include "uccontract.h"

namespace JAD {
  unsigned UCContract::compare_map(const UniversalContainer& uc) const
  {
    if (data_type != uc_Map || uc.get_type() != uc_Map) return 0;
    
    unsigned result = 0;
    unsigned req_count = 0;
    
    UniversalMap::iterator it = uc.map_begin();
    UniversalMap::iterator eit = uc.map_end(); 
    
    ContractMap* required = constraints.map_constraints.require_map;
    ContractMap* optional = constraints.map_constraints.optional_map;
    
    for (; it != eit; it++) {
      if (required && required->count(it->first) > 0) {
	req_count++;
	result |= required->operator[](it->first)->compare(it->second);
      }
      else if (optional && optional->count(it->first) > 0) {
	result |= optional->operator[](it->first)->compare(it->second);
      }
      else {
	result |= ucc_EXTRA_MAP_ELEMENT;
      }
    }
    if (req_count != required->size())
      result |= ucc_MISSING_REQUIRED_MAP_ELEMENT;
    
    return result;
  }
  
  unsigned UCContract::compare_array(const UniversalContainer& uc) const
  {
    if (data_type != uc_Array || uc.get_type() != uc_Array) return 0;
    unsigned result = 0;
    UniversalContainer tmp = (long int) uc.size();
    UniversalArray::iterator it;
    UniversalArray::iterator eit = uc.vector_end();
    
    if (constraints.array_constraints.size) 
      result |= constraints.array_constraints.size->compare(tmp);
 
    
    if (constraints.array_constraints.forall) {
      it = uc.vector_begin();
      for (;it < eit; it++)
	result |= constraints.array_constraints.forall->compare(*it);
    }
    
    if (constraints.array_constraints.exists) {
      ContractVector::iterator exists_list_it = constraints.array_constraints.exists->begin();
      ContractVector::iterator exists_list_eit = constraints.array_constraints.exists->end();
      
      for (; exists_list_it < exists_list_eit; exists_list_it++) {	
	it = uc.vector_begin();
	bool found = false;
	for (;it < eit && !found; it++)
	  found = (*exists_list_it)->compare(*it) == 0;
	if (!found)
	  result |= ucc_MISSING_REQUIRED_ARRAY_ELEMENT;
      }
    }
    return result;
  }
  
  unsigned UCContract::compare(const UniversalContainer& uc) const
  {
    int tmp_int;
    double tmp_dbl;
    
    if (data_type != uc.get_type()) return ucc_IMPROPER_TYPE;
    
    switch(data_type) {
    case uc_Integer :
    case uc_Character :
      tmp_int = uc;
      if ((constraints.int_pair.has_lower && tmp_int < constraints.int_pair.low) || 
	  (constraints.int_pair.has_upper && tmp_int > constraints.int_pair.high))
	return ucc_CONSTRAINT_VIOLATION;
      break;
    case uc_Real :
      tmp_dbl = uc;
      if ((constraints.real_pair.has_lower && tmp_dbl < constraints.real_pair.low) ||
	  (constraints.real_pair.has_upper && tmp_dbl > constraints.real_pair.high))
	return ucc_CONSTRAINT_VIOLATION;
      break;
    case uc_String :
      if (constraints.regex && regexec(constraints.regex,uc.c_str(),0,NULL,0))
	return ucc_STRING_DOES_NOT_MATCH;
      break;
    case uc_Map :
      return compare_map(uc);
      break;
    case uc_Array :
      return compare_array(uc);
      break;
    }
    return 0;
  }
  
  UCContract::UCContract(UniversalContainer& uc)
  {
    if (uc.get_type() != uc_Map) throw ucexception(uce_ContractViolation);
    if (!uc.exists("type")) throw ucexception(uce_ContractViolation);
    memset(this,0,sizeof(UCContract));
    std::string* type = uc["type"];
    
    if (!type->compare("string")) {
      data_type = uc_String;
      if (uc.exists("regex")) {
        constraints.regex = new regex_t;
	regcomp(constraints.regex,uc["regex"].c_str(),
	  REG_EXTENDED | REG_NOSUB);
      }
    } //end string
    
    else if (!type->compare( "integer")) {
	data_type = uc_Integer;
	if (uc.exists("lower_bound")) {
	  constraints.int_pair.low = uc["lower_bound"];
	  constraints.int_pair.has_lower = true;
	}
	if (uc.exists("upper_bound")) {
	  constraints.int_pair.high = uc["upper_bound"];
	  constraints.int_pair.has_upper = true;
	}  
    } //end int
    else if (!type->compare("real")) {
      data_type = uc_Real;
      if (uc.exists("lower_bound")) {
	constraints.real_pair.low = uc["lower_bound"];
	constraints.real_pair.has_lower = true;
      }
      if (uc.exists("upper_bound")) {
	constraints.real_pair.high = uc["upper_bound"];
	constraints.real_pair.has_upper = true;
      }    
    } //end real
    
    else if (!type->compare("map")) {
      data_type = uc_Map;
      if (uc.exists("required_members")) {
	constraints.map_constraints.require_map = new ContractMap;
	UniversalMap::iterator b = uc["required_members"].map_begin();
	UniversalMap::iterator e = uc["required_members"].map_end();
	for (;b != e; b++) 
	  (*(constraints.map_constraints.require_map))[b->first] = new UCContract(b->second);
      }
      if (uc.exists("optional_members")) {
	constraints.map_constraints.optional_map = new ContractMap;
	UniversalMap::iterator b = uc["optional_members"].map_begin();
	UniversalMap::iterator e = uc["optional_members"].map_end();
	for (;b != e; b++)
	  (*(constraints.map_constraints.optional_map))[b->first] = new UCContract(b->second);
      }
    } //end map
    else if (!type->compare( "array")) {
	data_type = uc_Array;
	if (uc.exists("forall")) {
	  constraints.array_constraints.forall = new UCContract(uc["forall"]);
	}
	if (uc.exists("size")) {
	  if (!constraints.array_constraints.size)
	    constraints.array_constraints.size = new UCContract(uc["size"]);
	}
	if (uc.exists("exists")) {
	  constraints.array_constraints.exists = new ContractVector;
	  UniversalArray::iterator b = uc["exists"].vector_begin();
	  UniversalArray::iterator e = uc["exists"].vector_end();
	  for(;b != e; b++)
	    constraints.array_constraints.exists->push_back(new UCContract(*b));  
	}
    }// end array
    else if (!type->compare("character")) {
      data_type = uc_Character; 
      if (uc.exists("lower_bound")) {
	constraints.int_pair.low = uc["lower_bound"];
	constraints.int_pair.has_lower = true;
      }
      if (uc.exists("upper_bound")) {
	constraints.int_pair.low = uc["upper_bound"];
	constraints.int_pair.has_upper = true;
      } 
    } //end character
    else if (!type->compare("boolean")) {
      data_type = uc_Boolean;
    } //end boolean
  }

  UCContract::~UCContract(void)
  {
    switch(data_type) {
    case(uc_String): 
      if (constraints.regex) regfree(constraints.regex);
      break;
    case(uc_Map):
      if (constraints.map_constraints.require_map) delete constraints.map_constraints.require_map;
      if (constraints.map_constraints.optional_map) delete constraints.map_constraints.optional_map;
      break;
    case(uc_Array):
      if (constraints.array_constraints.size) delete constraints.array_constraints.size;
      if (constraints.array_constraints.forall) delete constraints.array_constraints.forall;
      if (constraints.array_constraints.exists) delete constraints.array_constraints.exists;
    }
  }
 
  static const char* ucc_IMPROPER_TYPE_str = "Element has wrong type.";
  static const char* ucc_CONSTRAINT_VIOLATION_str = "A constraint was violated.";
  static const char* ucc_EXTRA_MAP_ELEMENT_str = "An map element not specified in the contract is present.";
  static const char* ucc_MISSING_REQUIRED_MAP_ELEMENT_str = "A required map element is missing.";
  static const char* ucc_MISSING_REQUIRED_ARRAY_ELEMENT_str = "An array element that must exist is not present.";
  static const char* ucc_STRING_DOES_NOT_MATCH_str = "A string does not match its given regex expression.";

  std::vector<const char*> UCContract::error_messages(unsigned result)
  {
    std::vector<const char*> mesg;
    
    if (result & ucc_IMPROPER_TYPE) mesg.push_back(ucc_IMPROPER_TYPE_str); 
    if (result & ucc_CONSTRAINT_VIOLATION) mesg.push_back(ucc_CONSTRAINT_VIOLATION_str);
    if (result & ucc_EXTRA_MAP_ELEMENT) mesg.push_back(ucc_EXTRA_MAP_ELEMENT_str);
    if (result & ucc_MISSING_REQUIRED_MAP_ELEMENT) 
      mesg.push_back(ucc_MISSING_REQUIRED_MAP_ELEMENT_str); 
    if (result & ucc_MISSING_REQUIRED_ARRAY_ELEMENT)
      mesg.push_back(ucc_MISSING_REQUIRED_ARRAY_ELEMENT_str);  
    if (result & ucc_STRING_DOES_NOT_MATCH) 
      mesg.push_back(ucc_STRING_DOES_NOT_MATCH_str);
 
   return mesg;
  }

  void UCContract::compare_and_throw(UniversalContainer& uc, unsigned mask) const
  {
    unsigned result = compare(uc) & mask;
    if (result) {
      std::vector<const char*> msgs = error_messages(result);
      UniversalContainer uce = ucexception(uce_ContractViolation);
      for (size_t i = 0; i < msgs.size(); i++)
	uce["violations"][i] = msgs[i];
      uce["compare_result"] = int(result);
      throw uce;
    }
  }
  
} //end namespace
