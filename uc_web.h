/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#ifndef _UC_WEB_H_
#define _UC_WEB_H_

namespace JAD {
  class UniversalContainer;


  //methods for working with mime-type/uc conversion
  bool can_decode_mime_type(const std::string&);
  UniversalContainer decode_by_mime_type(const std::string&, Buffer*);
  Buffer* encode_by_mime_type(UniversalContainer&, const std::string&);

  //basic cgi support
  UniversalContainer init_cgi(void);

  //http rpc call, with optional mime type (form or json)
  UniversalContainer web_rpc_call(const char*, UniversalContainer&,
				  const char* = NULL);

  //cgi return method
  void web_rpc_return(UniversalContainer&, const char* = NULL);
  

}

#endif
