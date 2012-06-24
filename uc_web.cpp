/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

/*
  Routines for working with UniversalContainers and the web. This
  includes getting a UC to represent available cgi data, and parsing
  incoming mime-type data to a uc. Note that making a http post
  request with a uc live in uc_curl.cpp because it requires curl.
*/

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "ucontainer.h"
#include "ucio.h"
#include "uc_web.h"
#include "string_util.h"
#include "buffer.h"

using namespace std;

//part of the standard runtime environment, array of key=value env variables
extern char** environ;

namespace JAD {
  
  //not stictly a web routine, but required for init_cgi and lacks a proper home
  UniversalContainer get_env_variables(void)
  {
    int i = 0;
    string tmp;
    vector<string> toks;
    UniversalContainer uc;
    
    while (environ[i]) {
      tmp = environ[i];
      toks = string_split(tmp,"=");
      uc[toks[0]].string_interpret(toks[1]);
      i++;
    }
    return uc;
  }

  /*
    Get cgi data for a cgi program. Top level keys of the return map are :
    env         environmental variables like agent string and ip
    post        post value and pairs, decoded based on type (form or json)
    get         get values and pairs
    cookies     cookie name and pairs
    stdin_available   usually false, but true if stdin was not consumed
                      this happens when the content type is not understood.
  */
  
  UniversalContainer init_cgi(void)
  {
    UniversalContainer uc;
    char* env;
    Buffer* orig;

    uc["env"] = get_env_variables();
    env = getenv("QUERY_STRING");
    if (env && strcmp(env,"")) {
      orig = new Buffer;
      orig->put_data(env,strlen(env));
      uc["get"] = uc_decode_form(orig);
      delete orig;
    }
    else uc["get"] = false;
   
    uc["stdin_available"] = true;
    uc["post"] = false;
    env = getenv("REQUEST_METHOD");
    if (env && !strcmp(env,"POST")) {
      env = getenv("CONTENT_TYPE");
      if (env) {
	if (can_decode_mime_type(env)) {
	  orig = read_to_buffer(stdin);
	  uc["post"].clear();
	  uc["post"] = decode_by_mime_type(env,orig);
	  uc["stdin_available"] = false;
	  delete orig;
	}
      }
    } //end post setup
   
    env = getenv("HTTP_COOKIE");
    vector<string> cookies;
    vector<string> kvpair;
    if (env) {
      string tmp(env);
      cookies = string_split(tmp,";");
      for (size_t i = 0; i < cookies.size(); i++) {
	cookies[i] = string_chomp(cookies[i]);
	kvpair = string_split(cookies[i],"=");
	uc["cookies"][kvpair[0]].string_interpret(kvpair[1]);
      }
    }
    else uc["cookies"] = false;

    return uc;
  }

  /*
    Return print an appropriate mime-type header, and then 
    an appropriate encoded uc to stdin, effectively returning
    a uc in response to an http post request.
    
    Expected usage pattern :
    client                         server
    web_rpc_call(request_uc)
                                   init_cgi (post holds request_uc)
				   processing
				   web_rpc_return(response_uc)
    response_uc returned from web_rpc_call
  */
				  
  void web_rpc_return(UniversalContainer& uc, const char* send_type)
  {
    if (!send_type) send_type = "application/x-www-form-urlencoded";
    printf("Content-type: %s\n\n",send_type);		   
    Buffer* send = encode_by_mime_type(uc,send_type);
    if (!send) throw ucexception(uce_Unknown_mime_type);
    write_from_buffer(send,stdout);
  }

  //we need to know if we can convert a type to a uc before we slurp
  //stdin in init_cgi
  bool can_decode_mime_type(const string& type)
  {
    if (type == "application/x-www-form-urlencoded") return true;
    else if (type == "application/json") return true;
    return false;
  }
  
  //wrappers around mime-type detection and encoding/decoding
  UniversalContainer decode_by_mime_type(const string& type, Buffer* buf)
  {
    UniversalContainer uc;
    
    if (type == "application/x-www-form-urlencoded") 
      uc = uc_decode_form(buf);
    else if (type == "application/json") {
      uc = uc_decode_json(buf);
    } else {
      uc["#boolean_value"] = false;
      uc["mime-type"] = type;
      uc["contents"] = string(buf->data+buf->rpos,buf->length-buf->rpos);
    }
    
    return uc;
  }
  
  Buffer* encode_by_mime_type(UniversalContainer& uc, const string& type)
  {
    Buffer* encode = NULL;
    
    if (type == "application/x-www-form-urlencoded") {
      encode = uc_encode_form(uc);
    }
    else if (type == "application/json") {
      encode = uc_encode_json(uc);
    }
    
    return encode;
  }
 
} //end namespace
