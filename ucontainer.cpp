/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <iostream>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <limits>
#include "ucontainer.h"
#include "stl_util.h"

/*
  Limitations :
  All maps, vectors, and strings are references, and copies copy the reference.
  
  The array behavior is strange. If an index has not been previously
  used, it must be equal to the current size of the array. That is, it must
  be one past the previously used indicies.

*/


#ifndef NDEBUG
#define internal_ucexception(C) construct_exception(C,__FUNCTION__,__FILE__,__LINE__,this)
#else
#define internal_ucexception(C) construct_exception(C,NULL,NULL,0,this)
#endif

using namespace std;

namespace JAD {

  static const char* BOOL_VAL_STR = "#boolean_value";
  static const char* TRUE_STR = "TRUE";
  static const char* FALSE_STR = "FALSE";
  static const char* true_str = "true";
  static const char* false_str = "false";

  /*
    The set routines are mostly called by the assignment operators
    to do the acutal work of setting a particular type and value into
    the UniversalContainer (uc).
   */

  void UniversalContainer::set_value_integer(long l)
  {
    type = uc_Integer;
    data.num = l;
    dirty = true;
  }

  void UniversalContainer::set_value_char(char c)
  {
    type = uc_Character;
    data.chr = c;
    dirty = true;
  }

  void UniversalContainer::set_value_double(double d)
  {
    type = uc_Real;
    data.real = d;
    dirty = true;
  }

  void UniversalContainer::set_value_bool(bool b)
  {
    type = uc_Boolean;
    data.tf = b;
    dirty = true;
  }

  void UniversalContainer::set_value_string(const string& s)
  {
    //if we already have a string reference
    if (refcount) {
      if (*refcount > 1) { //someone else point at this string;
	(*refcount)--;
	refcount = new unsigned;
	(*refcount) = 1;
      }
      else if (*refcount == 1)
	delete data.str;
      //else we are in trouble
    }
    else { //starting fresh
      refcount = new unsigned;
      *refcount = 1;
    }
    
    type = uc_String;
    dirty = true;
    data.str = new string(s);
  }

  void UniversalContainer::set_value_wstring(const wstring& s)
  {
    //if we already have a string reference
    if (refcount) {
      if (*refcount > 1) { //someone else point at this string;
	(*refcount)--;
	refcount = new unsigned;
	(*refcount) = 1;
      }
      else if (*refcount == 1)
	delete data.wstr;
      //else we are in trouble
    }
    else { //starting fresh
      refcount = new unsigned;
      *refcount = 1;
    }
    
    type = uc_WString;
    dirty = true;
    data.wstr = new wstring(s); 
  }

  void UniversalContainer::set_value_cstr(const char* s)
  {
    if (s)
      set_value_string(string(s));
    else {
      string tmp;
      set_value_string(tmp);
    }
  }

  /*
    Create new container types.
   */ 
  void UniversalContainer::init_map(void)
  {
    type = uc_Map;
    data.map = new UniversalMap;
    refcount = new unsigned;
    (*refcount) = 1;
    dirty = true;
  }

  void UniversalContainer::init_array(void)
  {
    type = uc_Array;
    data.ray = new UniversalArray;
    refcount = new unsigned;
    (*refcount) = 1;
    dirty = true;
  }

  /*
    The various containers create containers of the appropriate type
    for the various parameter types that can be passed.
   */

  UniversalContainer::UniversalContainer(void)
  {
    type = uc_Null;
    refcount = NULL;
    data.str = NULL;
    dirty = true;
  }

  UniversalContainer::UniversalContainer(int i)
  {
    refcount = NULL;
    set_value_integer(i);
  }

  UniversalContainer::UniversalContainer(long l)
  {
    refcount = NULL;
    set_value_integer(l);
  }

  UniversalContainer::UniversalContainer(char c)
  {
    refcount = NULL;
    set_value_char(c);
  }

  UniversalContainer::UniversalContainer(bool b)
  {
    refcount = NULL;
    set_value_bool(b);
  }

  UniversalContainer::UniversalContainer(double d)
  {
    refcount = NULL;
    set_value_double(d);
  }

  UniversalContainer::UniversalContainer(const string s)
  {
    refcount = NULL;
    set_value_string(s);
    return;
  }

  UniversalContainer::UniversalContainer(const wstring s)
  {
    refcount = NULL;
    set_value_wstring(s);
    return;
  }

  UniversalContainer::UniversalContainer(char* s)
  {
    refcount = NULL;
    set_value_cstr(s);
    return;
  }

  /*
    Deletion is tricky. If we are the referning container, clean up
    both the container and refcount.
   */
  UniversalContainer::~UniversalContainer(void)
  {
    if (refcount) {
      (*refcount)--;
      if ((*refcount) == 0) {
	switch(type) {
	case uc_String :
	  delete data.str;
	  break;
	case uc_WString :
	  delete data.wstr;
	  break;
	case uc_Map :
	  delete data.map;
	  break;
	case uc_Array :
	  delete data.ray;
	  break;
	default : ;
	}
	delete refcount;
      } //refcount==0
    } //if refcount
  }

  //designated code for copy constructor and operator=(UniversalContainer) 
  void UniversalContainer::duplicate(const UniversalContainer& uc)
  {
    type = uc.type;
    refcount = uc.refcount;
    if (refcount) (*refcount)++;

    switch(type) {
    case uc_Map :
      data.map = uc.data.map;
      break;
    case uc_Array :
      data.ray = uc.data.ray;
      break;
    case uc_Integer :
      data.num = uc.data.num;
      break;
    case uc_Real :
      data.real = uc.data.real;
      break;
    case uc_Boolean :
      data. tf = uc.data.tf;
      break;
    case uc_Character : 
      data.chr = uc.data.chr;
      break;
    case uc_String :
      data.str = uc.data.str;
      break;
    case uc_WString :
      data.wstr = uc.data.wstr;
      break;
    default : ;
    }

    dirty = uc.dirty;
  }

  UniversalContainer::UniversalContainer(const UniversalContainer& uc)
  {
    duplicate(uc);
  }

  //map brackets does the actual work of operator[string]
  //it understand . notation to reach nested maps
  UniversalContainer& UniversalContainer::map_brackets(const string s)
  {
    unsigned long pos;
    bool ismap;
    int idx;
    std::string piece1;
    std::string piece2;
	
    //split input into first argument and rest
    pos = s.find('.');
    piece1 = s.substr(0,pos);
    if (pos != string::npos) piece2 = s.substr(pos+1);
	
    //double check that this isn't really supposed to be a string
    if (type == uc_Map) ismap = true;
    else {
      errno = 0;
      idx = strtol(piece1.c_str(),NULL,10);
      if (errno) ismap = true;
      else ismap = false;
    }

    if (ismap) {
      if (type == uc_Null) init_map(); //if this is my first apparence, setup
      if (type != uc_Map)
	throw internal_ucexception(uce_Scalar_as_Collection);
      if (piece2 == "") return (*(data.map))[piece1];
      else return (*(data.map))[piece1][piece2];
    }
    else { //isarray
      if (type == uc_Null) init_array();
      if (type != uc_Array)
	throw internal_ucexception(uce_Scalar_as_Collection);
      if (piece2 == "") return (this->operator[](idx));
      else return (this->operator[](idx))[piece2];
    }
  }

  UniversalContainer& UniversalContainer::operator[](const string s)
  {
    return map_brackets(s);
  }

  UniversalContainer& UniversalContainer::operator[](const wstring w)
  {
    return map_brackets(convert_wstring_to_string(&w));
  }

  UniversalContainer& UniversalContainer::operator[](char* s)
  {
    string str(s);
    return map_brackets(str);
  }


  UniversalContainer& UniversalContainer::operator[](const char* s)
  {
    string str(s);
    return map_brackets(str);
  }


  UniversalContainer& UniversalContainer::operator[](int i)
  {
    if (type == uc_Null) init_array();
    if (type != uc_Array) 
      throw internal_ucexception(uce_Non_Array_as_Array);
	
    int sz = data.ray->size();
    if (i == -1) i = sz;
    if (i == sz) {
      dirty = true;
      UniversalContainer uc;
      data.ray->push_back(uc);
    }
    if (i > sz || i < 0)
      throw internal_ucexception(uce_Array_Subscript_Out_of_Bounds);
    return data.ray->at(i);
  }

  /*
    Assignment operators. They mostly do their job by calling the
    appropriate set_* routines.
   */
  UniversalContainer& UniversalContainer::operator=(int i)
  {
    if (!(type == uc_Integer || type == uc_Real || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
	
    set_value_integer(i);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(long l)
  {
    if (!(type == uc_Integer || type == uc_Real || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
	
    set_value_integer(l);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(double d)
  {
    if (!(type == uc_Integer || type == uc_Real || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
  
    set_value_double(d);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(bool b)
  {
    if (!(type == uc_Boolean || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
  
    set_value_bool(b);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(const string& s)
  {
    if (!(type == uc_String || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
  
    set_value_string(s);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(const wstring& s)
  {
    if (!(type == uc_WString || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
	
    set_value_wstring(s);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(const char* s)
  {
    if (!(type == uc_String || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
     
    set_value_cstr(s);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(char c)
  {
    if (!(type == uc_Character || type == uc_Null))
      throw internal_ucexception(uce_TypeMismatch_Write);
	
    set_value_char(c);
    return *this;
  }

  UniversalContainer& UniversalContainer::operator=(const UniversalContainer& uc)
  {
    if (this != &uc) duplicate(uc);
    return *this;
  }

  //utilities for casting strings to longs
  long UniversalContainer::convert_string_to_long(const string* s) 
  {
    errno = 0;
    long l = strtol(s->c_str(),NULL,10);
    if (errno) throw ucexception(uce_TypeMismatch_Read);
    return l;
  }

  long UniversalContainer::convert_wstring_to_long(const wstring* s)
  {
    string tmp = convert_wstring_to_string(s);
    return convert_string_to_long(&tmp);
  }

  /*
    The various convert routines are mostly used by the casting operators,
    to cast the various types to what the cast calls for.
   */

  int UniversalContainer::convert_int(void) const
  {
    long retval;

    switch (type) {
    case uc_Integer :
      retval = data.num;
      break;
    case uc_Boolean :
      if (data.tf == true) return 1;
      else return 0;
    case uc_Real :
      retval = static_cast<long>(data.real);
      break;
    case uc_Character :
      return static_cast<long>(data.chr);
    case uc_String :
      retval = convert_string_to_long(data.str);
      break;
    case uc_WString :
      retval = convert_wstring_to_long(data.wstr);
      break;
    case uc_Map :
    case uc_Array :
      throw internal_ucexception(uce_Collection_as_Scalar);
    case uc_Null :
      return 0;
    default :
      throw internal_ucexception(uce_Unknown);
    }

    if (retval > std::numeric_limits<int>::max() ||
	retval < std::numeric_limits<int>::min())
      throw internal_ucexception(uce_TypeMismatch_Read);
    else return static_cast<int>(data.num);
  
    return retval;
  }

  long UniversalContainer::convert_long(void) const
  {
    switch (type) {
    case uc_Integer :
      return data.num;
    case uc_Boolean :
      if (data.tf) return 1;
      else return 0;
    case uc_Real :
      return static_cast<int>(data.real);
    case uc_Character :
      return static_cast<char>(data.chr);
    case uc_String :
      return convert_string_to_long(data.str);
    case uc_WString :
      return convert_wstring_to_long(data.wstr);
    case uc_Map :
    case uc_Array :
      throw internal_ucexception(uce_Collection_as_Scalar);
    case uc_Null :
      return 0;
    default :
      throw internal_ucexception(uce_Unknown);
    }
	
    return 0L;
  }

  std::string UniversalContainer::convert_string(void) const
  {
    char buf[32];
	
    switch (type) {
    case uc_String :
      return *(data.str);
    case uc_WString :
      return convert_wstring_to_string(data.wstr);
    case uc_Integer :
      snprintf(buf,32,"%ld",data.num);
      return string(buf);
      break;
    case uc_Real :
      snprintf(buf,32,"%g",data.real);
      return string(buf);
      break;
    case uc_Boolean :
      if (data.tf) return string(true_str);
      else return string(false_str);
    case uc_Character :
      snprintf(buf,32,"%c",data.chr);
      return(buf);
      break;
    case uc_Null :
      return string("null");
      break;
    case uc_Map :
    case uc_Array :
      throw internal_ucexception(uce_Collection_as_Scalar);
    default :
      throw internal_ucexception(uce_Unknown);
    }
	
    return string("error");
  }

  std::wstring UniversalContainer::convert_wstring(void) const
  {
    wstring retval;
    char buf[32];
    string tmp;
  
    switch (type) {
    case uc_WString :
      retval = *(data.wstr);
      return retval;
    case uc_String :
      tmp = *(data.str);
      break;
    case uc_Integer :
      snprintf(buf,32,"%ld",data.num);
      tmp = buf;
      break;
    case uc_Real :
      snprintf(buf,32,"%g",data.real);
      tmp = buf;
      break;
    case uc_Boolean :
      if (data.tf) tmp = true_str;
      else tmp = false_str;
      break;
    case uc_Character :
      snprintf(buf,32,"%c",data.chr);
      tmp = buf;
      break;
    case uc_Null :
      tmp = "null";
      break;
    case uc_Map :
    case uc_Array :
      throw internal_ucexception(uce_Collection_as_Scalar);
    default :
      throw internal_ucexception(uce_Unknown);
    }

    return convert_string_to_wstring(&tmp);
  }
  
  UniversalContainer::operator std::string*(void) const
  {
    if (type == uc_String) return data.str;
    if (type == uc_Null) return NULL;
    throw internal_ucexception(uce_TypeMismatch_Read);
  }

  const char* UniversalContainer::c_str(void) const
  {
    if (type == uc_String) return data.str->c_str();
    if (type == uc_Null) return NULL;
    throw internal_ucexception(uce_TypeMismatch_Read);    
  }

  UniversalContainer::operator std::wstring*(void) const
  {
    if (type == uc_WString) return data.wstr;
    if (type == uc_Null) return NULL;
    throw internal_ucexception(uce_TypeMismatch_Read);
  }
  
  char UniversalContainer::convert_char(void) const
  {	
    switch (type) {
    case uc_Character :
      return data.chr;
    case uc_Boolean :
      if (data.tf) return 't';
      else return 'f';
    case uc_String :
      return data.str->c_str()[0];
    case uc_WString :
    case uc_Integer :
    case uc_Real :
      throw internal_ucexception(uce_TypeMismatch_Read);
    case uc_Map :
    case uc_Array :
      throw internal_ucexception(uce_Collection_as_Scalar);
    case uc_Null :
      return '\0';
    default :
      throw internal_ucexception(uce_Unknown);
    }
	
    return '\0';
  }

  bool UniversalContainer::convert_bool(void) const
  {	
    switch (type) {
    case uc_Boolean :
      return data.tf;
    case uc_Character :
      if (data.chr != 0) return true;
      else return false;
    case uc_Integer :
      return (data.num != 0);
      break;
    case uc_Real :
      return (data.real != 0.0);
      break;
    case uc_String :
      if (!data.str->compare(TRUE_STR) || !data.str->compare(true_str)) return true;
      else if(!data.str->compare(FALSE_STR) || !data.str->compare(false_str)) return false;
      throw internal_ucexception(uce_TypeMismatch_Read);
    case uc_WString :
      throw internal_ucexception(uce_TypeMismatch_Read);
    case uc_Map :
      if (exists(BOOL_VAL_STR)) return (*(data.map))[BOOL_VAL_STR];
      else return true;
    case uc_Array :
      return true;
    case uc_Null :
      return false;
      break;
    default :
      throw internal_ucexception(uce_Unknown);
    }
	
    return false;
  }

  double UniversalContainer::convert_string_to_double(const string* s) 
  {
    double retval;
    char* ptr;
    errno = 0;

    retval = strtod(s->c_str(),&ptr);
    if (errno || (retval == 0.0 &&
		  ptr != (s->c_str() + s->length())))
      throw ucexception(uce_TypeMismatch_Read);
    return retval;
  }

  double UniversalContainer::convert_wstring_to_double(const wstring* s) 
  {
    string tmp = convert_wstring_to_string(s);
    return convert_string_to_double(&tmp);
  }

  double UniversalContainer::convert_double(void) const
  {
    switch (type) {
    case uc_Real :
      return data.real;
    case uc_Integer :
      return static_cast<double>(data.num);
    case uc_Boolean :
      if (data.tf) return 1.0;
      else return 0.0;
    case uc_Character :
      return static_cast<double>(data.chr);
    case uc_String :
      return convert_string_to_double(data.str);
    case uc_WString :
      return convert_wstring_to_double(data.wstr);
    case uc_Map :
    case uc_Array :
      throw internal_ucexception(uce_Collection_as_Scalar);
    case uc_Null :
      return 0.0;
    default :
      throw internal_ucexception(uce_Unknown);
    }
       
    return 0.0;
  }

  UniversalContainer::operator int(void) const
  {
    return convert_int();
  }

  UniversalContainer::operator double(void) const
  {
    return convert_double();
  }

  UniversalContainer::operator char(void) const
  {
    return convert_char();
  }

  UniversalContainer::operator bool(void) const
  {
    return convert_bool();
  }

  UniversalContainer::operator long(void) const
  {
    return convert_long();
  }

  UniversalContainer::operator std::string(void) const
  {
    return convert_string();
  }

  UniversalContainer::operator std::wstring(void) const
  {
    return convert_wstring();
  }


  UniversalContainerType UniversalContainer::get_type(void) const
  {
    return type;
  };

  //kind of a string constructor. Callable only on a null container.
  //works through various options to figure out if the string 
  //is an integer, number, boolean, or character.
  void UniversalContainer::string_interpret(const string s)
  {
    const char* str = s.c_str();
    char* end;
    char* up;
    int len;
	
    if (type != uc_Null)
      throw internal_ucexception(uce_TypeMismatch_Write);

    dirty = true;
    refcount = NULL;
    len = strlen(str);
    const char* last = str + len;

    //special empty string case
    if (str[0] == '\0') {
      set_value_string(s);
      return;
    }
	
    errno = 0;
    data.num = strtol(str,&end,10);
    if (!errno && last == end) {
      type = uc_Integer;
	return;
    }
	
    errno = 0;
    data.real = strtod(str,&end);
    if (!errno && last == end) {
      type = uc_Real;
      return;
    }
	
    if (len == 1) {
      type = uc_Character;
      data.chr = str[0];
      return;
    }
	
    up = new char[len+1];
    strncpy(up,str,len+1); //ensure null terminator
    for (int i = 0; i < len; i++)
      up[i] = toupper(up[i]);

    if (!strncmp(up,"TRUE",len)) {
      type = uc_Boolean;
      data.tf = true;
      delete[] up;
      return;
    }
    if (!strncmp(up,"FALSE",len)) {
      type = uc_Boolean;
      data.tf = false;
      delete[] up;
      return;
    }
    if (!strncmp(up,"NULL",len)) {
      delete[] up;
      return;
    }

    delete[] up;
	
    set_value_string(s);
  }

  //deep copy operation
  UniversalContainer UniversalContainer::clone(void) const
  {
    UniversalContainer clone;
    bool done = true;
	
    clone.type = type;
    if (type == uc_String || type == uc_WString || type == uc_Map ||
	type == uc_Array) {
      clone.refcount = new unsigned;
      *(clone.refcount) = 1;
    }
    clone.dirty = dirty;

    switch(type) {
    case uc_Integer :
      clone.data.num = data.num;
      break;
    case uc_Real :
      clone.data.real = data.real;
      break;
    case uc_Boolean :
      clone.data.tf = data.tf;
      break;
    case uc_Character : 
      clone.data.chr = data.chr;
      break;
    case uc_String :
      clone.data.str = new std::string(*(data.str));
      break;
    case uc_WString :
      clone.data.wstr = new std::wstring(*(data.wstr));
      break;
    case uc_Null :
      break;
    default :
      done = false;
    }
	
    if (!done) {
      if (type == uc_Map) {
	clone.data.map = new UniversalMap;
	UniversalMap::iterator im;
	UniversalMap::iterator mend = data.map->end();
	for (im = data.map->begin(); im != mend; im++) 
	  (*(clone.data.map))[im->first] = im->second.clone();
      }
      else if (type == uc_Array) {
	clone.data.ray = new UniversalArray;
	UniversalArray::iterator ia;
	UniversalArray::iterator aend = data.ray->end();
	for (ia = data.ray->begin(); ia != aend; ia++)
	  (clone.data.ray)->push_back(ia->clone());
      }
      else throw internal_ucexception(uce_Unknown);
    }
	
    return clone;
  }

  //get real c++ iterators
  UniversalMap::iterator UniversalContainer::map_begin(void) const
  {
    if (type == uc_Map) return data.map->begin();
    else throw internal_ucexception(uce_TypeMismatch_Read);
  }

  UniversalMap::iterator UniversalContainer::map_end(void) const
  {
    if (type == uc_Map) return data.map->end();
    else throw internal_ucexception(uce_TypeMismatch_Read);
  }

  UniversalArray::iterator UniversalContainer::vector_begin(void) const
  {
    if (type == uc_Array) return data.ray->begin();
    else throw internal_ucexception(uce_TypeMismatch_Read);
  }

  UniversalArray::iterator UniversalContainer::vector_end(void) const
  {
    if (type == uc_Array) return data.ray->end();
    else throw internal_ucexception(uce_TypeMismatch_Read);
  }

  //null out an object. logically, uc = NULL
  void UniversalContainer::clear(void)
  {
    if (refcount) {
      if ((*refcount) > 1) (*refcount)--;
      if (!(*refcount)) {
	delete refcount;
	if (type == uc_String) delete data.str;
	else if (type == uc_WString) delete data.wstr;
	else if (type == uc_Map) delete data.map;
	else if (type == uc_Array) delete data.ray;
      }
    }
	
    refcount = NULL;
    dirty = true;
    type = uc_Null;
  }

  //true if this is a map, and it contains the given key
  bool UniversalContainer::exists(const string key) const
  {
    if (type == uc_Map) return (data.map->count(key) > 0);
    else return false;
  }

  //size of map or array
  size_t UniversalContainer::size(void) const
  {
    return length();
  }

  size_t UniversalContainer::length(void) const
  {
    if (type == uc_Map) return data.map->size();
    else if (type == uc_Array) return data.ray->size();
    else if (type == uc_String) return data.str->length();
    else if (type == uc_WString) return data.wstr->length();
    else if (type == uc_Null) return 0;
    else throw internal_ucexception(uce_TypeMismatch_Read);
  }

  //get routines, to expose the underlying stl objects just in case
  UniversalMap* UniversalContainer::get_map(void) const
  {
    if (type == uc_Map) return data.map;
    else throw internal_ucexception(uce_TypeMismatch_Read);
  }

  UniversalArray* UniversalContainer::get_vector(void) const
  {
    if (type == uc_Array) return data.ray;
    else throw internal_ucexception(uce_TypeMismatch_Read);
  }

  //delete a key from a map
  bool UniversalContainer::remove(const string key)
  {
    if (type != uc_Map) throw internal_ucexception(uce_TypeMismatch_Read);
    UniversalMap::iterator pos = data.map->find(key);
    if (pos == data.map->end()) return false;
    data.map->erase(pos);
    return true;
  }

  //add an element to the vector and return a reference to it
  UniversalContainer& UniversalContainer::added_element(void)
  {
    if (type == uc_Null) init_array();
    if (type != uc_Array) 
      throw internal_ucexception(uce_Non_Array_as_Array);
       
    UniversalContainer uc;
    int sz = data.ray->size();
    data.ray->push_back(uc);
    dirty = true;
	
    return data.ray->at(sz);
  }

  //do a logical comparison of two containers. Types must match.
  //arrays must match in order. Maps must contain the same keys
  //with the same values.
  bool UniversalContainer::operator==(const UniversalContainer& uc) const
  {
    if (type != uc.type) return false;
	
    switch(type) {
    case uc_Integer :
      return data.num == uc.data.num;
    case uc_Real :
      return data.real == uc.data.real;
    case uc_Boolean :
      return data.tf == uc.data.tf;
    case uc_Character :
      return data.chr == uc.data.chr;
    case uc_String :
      return *(data.str) == *(uc.data.str);
    case uc_WString :
      return *(data.wstr) == *(uc.data.wstr);
    case uc_Map :
      if (data.map == uc.data.map) return true;
      return compare_map(*(data.map),*(uc.data.map));
    case uc_Array :
      if (data.ray == uc.data.ray) return true;
      return compare_vector(*(data.ray),*(uc.data.ray));
    case uc_Null :
      return true;
    }
    return false;
  }

  //equivalance tests for primitives.
  bool UniversalContainer::operator==(int i) const
  {
    int comp;
    try {
      comp = convert_int();
      return comp == i;
    }
    catch (UniversalContainer& uce) {
      return false;
    }
  }

  bool UniversalContainer::operator==(long l) const
  {
    long comp;
    try {
      comp = convert_long();
      return comp == l;
    }
    catch (UniversalContainer& uce) {
      return false;
    }
  }

  bool UniversalContainer::operator==(std::string s) const
  {
    std::string comp;
    try {
      comp = convert_string();
      return comp == s;
    }
    catch (UniversalContainer& uce) {
      return false;
    }
  }

  bool UniversalContainer::operator==(double d) const
  {
    double comp;
    try {
      comp = convert_double();
      return comp == d;
    }
    catch (UniversalContainer& uce) {
      return false;
    }
  }

  bool UniversalContainer::operator==(bool b) const
  {
    bool comp;
    try {
      comp = convert_bool();
      return comp == b;
    }
    catch (UniversalContainer& uce) {
      return false;
    }
  }

  bool UniversalContainer::operator==(char c) const
  {
    char comp;
    try {
      comp = convert_char();
      return comp == c;
    }
    catch (UniversalContainer& uce) {
      return false;
    }
  }

  //get an array of all the keys in the map
  std::vector<std::string> UniversalContainer::keys_for_map(void) const
  {
    if (type != uc_Map)
      throw internal_ucexception(uce_Non_Map_as_Map);

    return ::keys_for_map(*(data.map));
  }

  //sizeof(wchar_t) differs for windows/unix, so we go through some
  //gyrations in an attempt to make this work everywhere
  string UniversalContainer::convert_wstring_to_string(const wstring* wstr)
  {
    string s;
    wchar_t w,t;

    wchar_t mask = -1;
    mask >>= 8; 
    mask <<= 8; //bottom 8 bits are 0, top bits are 1

    for (size_t i = 0; i < wstr->length(); i++) {
      w = (*wstr)[i];
      for (size_t j = sizeof(wchar_t); j > 0; j--) {
	t = w >> (8 * j-1);
	t &= mask;
	if (t) s.push_back(static_cast<char>(t));
      }
    }
    return s;
  }
 
  wstring UniversalContainer::convert_string_to_wstring(const string* s) 
  {
    wchar_t* ws = new wchar_t[s->length()];
    for (unsigned i = 0; i < s->length(); i++)
      ws[i] = static_cast<wchar_t>((*s)[i]);
    wstring wstr(ws);
    return wstr;
  }

  //logically, setting a container makes it dirty, as does adding an element
  //containers are only cleaned when clean is called explictly.
  //libuc doesn't use this functionality yet, but future versions
  //might if there are classes to work with external key stores
  bool UniversalContainer::is_dirty(void) const
  {
    UniversalMap::iterator im;
    UniversalMap::iterator mend;
    UniversalArray::iterator ia;
    UniversalArray::iterator aend;
    
    if (dirty) return true;

    switch(type) {
    case uc_Map :

	mend = data.map->end();
	for (im = data.map->begin(); im != mend; im++) 
	  if (im->second.is_dirty()) return true;
	return false;
    case uc_Array :
	aend = data.ray->end();
	for (ia = data.ray->begin(); ia != aend; ia++)
	  if (ia->is_dirty()) return true;
	return false;
    default :
      return false; //since dirty == false, and not a collection
    }
    return false; //should not get here.
  }

  void UniversalContainer::clean(void)
  {
    UniversalMap::iterator im;
    UniversalMap::iterator mend;
    UniversalArray::iterator ia;
    UniversalArray::iterator aend;
    
    dirty = false;
    switch(type) {
    case uc_Map :
      mend = data.map->end();
      for (im = data.map->begin(); im != mend; im++) 
	im->second.clean();
    case uc_Array :
      aend = data.ray->end();
      for (ia = data.ray->begin(); ia != aend; ia++)
	ia->clean(); 
    }
  }

  /* This code handles exceptions for universal containers. */
#define KNOWN_EXCEPTIONS 14

  static const char* uce_messages[KNOWN_EXCEPTIONS] =
    {"Unknown UniversalContainer exception.",
     "Attempt to assign a variable to a container which already contains a variable of another type.",
     "Attempt to cast a variable to an incompatible type.",
     "Attempt to treat a collection as a scalar variable.",
     "Attempt to treat a scalar variable as a collection.",
     "Attempt to treat a non-map container as a map.",
     "Attempt to treat a non-array container as an array.",
     "Array subscript out of bounds.",
     "Input is not a valid serialized UniversalContainer.",
     "Unable to serialize containers with elements of an unknown type.",
     "Error connecting to the database.",
     "Unknown mime type.",
     "Communications Error.",
     "Contract violation."
    };

  /* Should only be invoked through the macro ucexception, found in ucontainer.h */

  UniversalContainer UniversalContainer::construct_exception(
		    const int code, const char* function, const char* file,
		    int line, const UniversalContainer* bad)
  {
    UniversalContainer uc;
    uc["code"] = code;
    uc["message"] = uce_messages[(code < KNOWN_EXCEPTIONS ? code : 0)];
    uc["function"] = function;
    uc["file"] = file;
    uc["line"] = line;
    if (bad) uc["container"] = bad->clone();
    return uc;
  }
} //end namespace
/* End Notes
   
 * On many machine/compilier combinations int and long have the same
 * size. This code assumes that sizeof(int) != sizeof(long), just in
 * case. Note, that we need seperate int and long types to avoid
 * ambiguity in the assignment and casting operators.
 */
