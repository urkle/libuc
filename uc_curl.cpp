/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

/*
  Routines for interfacing libuc with curl. Includes posting and getting
  UniversalContainers.
 */

#include "ucontainer.h"
#include "buffer.h"
#include "uc_web.h"

namespace JAD {
  /*
    HTTP post to the given url. Request is encoded by send_type,
    which defaults to form encoding but may also be json. Parses the
    reutnr 
   */
  UniversalContainer web_rpc_call(const char* url, 
				  UniversalContainer& request,
				  const char* send_type)
  {
    UniversalContainer uc;
    
    if (!send_type) send_type = "application/x-www-form-urlencoded"; 
    Buffer* send = encode_by_mime_type(request,send_type);
    if (!send) throw ucexception(uce_Unknown_mime_type);
 
    string ret_type;
    Buffer* receive = http_post_buffer(url,send,ret_type,send_type);
    if (!receive) {uc = false; return uc;}
    uc = decode_by_mime_type(ret_type,receive); 
    delete receive;
    return uc;
  }

  //pull a url, and try to interpret the results as a uc
  UniversalContainer web_fetch(const char* url)
  {
    UniversalContainer uc;
    string ret_type;
    Buffer* receive = http_get_buffer(url,ret_type);
    if (!receive) throw ucexception(uce_Communication_Error);
    uc = decode_by_mime_type(ret_type,receive); 
    delete receive;
    return uc;
  }

}//end namespace
