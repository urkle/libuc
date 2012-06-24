/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010,2012.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#ifndef __UCCONTAINER_H_
#define __UCCONTAINER_H_
#include <regex.h>
#include <map>
#include <vector>

#include "ucontainer.h"

namespace JAD {

class UCContract;
typedef std::map<std::string,UCContract*> ContractMap;
typedef std::vector<UCContract*> ContractVector;

class UCContract {

  UniversalContainerType data_type;

  union {
    struct {
      bool has_lower;
      bool has_upper;
      long low;
      long high;
    } int_pair;
    struct {
      bool has_lower;
      bool has_upper;
      double low;
      double high;
    } real_pair;
    regex_t* regex;
    struct {
      ContractMap* require_map;
      ContractMap* optional_map;
    } map_constraints;
    struct {
      UCContract* size;
      UCContract* forall;
      ContractVector* exists;
    } array_constraints;  
  } constraints;
  
  unsigned compare_map(const UniversalContainer&) const;
  unsigned compare_array(const UniversalContainer&) const;
  
public:
  UCContract(UniversalContainer&);
  ~UCContract(void);
  unsigned compare(const UniversalContainer&) const;
  void compare_and_throw(UniversalContainer&, unsigned = 0XFFFFFFFF) const;
  static std::vector<const char*> error_messages(unsigned result);
};

} //end namespace

#define ucc_IMPROPER_TYPE                     0x00000001
#define ucc_CONSTRAINT_VIOLATION              0x00000002
#define ucc_EXTRA_MAP_ELEMENT                 0x00000004
#define ucc_MISSING_REQUIRED_MAP_ELEMENT      0x00000008
#define ucc_MISSING_REQUIRED_ARRAY_ELEMENT    0x00000010
#define ucc_STRING_DOES_NOT_MATCH             0x00000020

#endif
