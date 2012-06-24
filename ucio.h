/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */
/*
  Serializiation and deserializiation declarations, and convinence wrappers.
 */

#ifndef _UCIO_H_
#define _UCIO_H_

#include <string>

namespace JAD {

  class UniversalContainer;
  class Buffer;

  //prototypes for serializers and deserializers
  UniversalContainer uc_decode_ini(Buffer*);
  Buffer* uc_encode_ini(const UniversalContainer& uc);

  UniversalContainer uc_decode_form(Buffer*);
  Buffer* uc_encode_form(const UniversalContainer& uc);
  
  UniversalContainer uc_decode_binary(Buffer*);
  Buffer* uc_encode_binary(const UniversalContainer&);

  UniversalContainer uc_decode_json(Buffer*);
  Buffer* uc_encode_json(const UniversalContainer&);

  //basic print function
  void print(UniversalContainer&);

  //wrapper functions for ini decode/encode for ini files
  UniversalContainer load_ini_file(const char*);
  bool write_ini_file(const char*, UniversalContainer&);
  UniversalContainer uc_from_json_file(const char*);
}

#endif

  
