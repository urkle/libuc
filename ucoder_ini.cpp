/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <stdio.h>
#include <string>
#include "string_util.h"
#include "ucontainer.h"
#include "buffer.h"

using namespace std;

/*
  These routines handle both encoding a uc as an ini file, and as a
  web form.  The difference is that in a web form we must urlencode
  the key and value pairs, but not the = symbol. The actual work is
  done for both by one pair of routines that check a flag, but the
  public interface hides this behind explict named wrappers.
 */

namespace JAD {

  string urlencode(const string& data)
  {
    int ftype = 0;
    static const char* format[] = {"%c","%%%02X"};
    char hold;
    string enc;
    char tmp[4];
    
    for (size_t i = 0; i < data.length(); i++) {
      ftype = 0;
      hold = data[i];
      if (data[i] == ' ') hold = '+';
      else if (data[i] == '\n') hold = '&';
      else if (data[i] < ',' || data[i] > 'z' ||
	       (data[i] > '9' && data[i] < 'A') ||
	       (data[i] > 'Z' && data[i] < '_') ||
	       (data[i] > '_' && data[i] < 'a'))
	ftype = 1;
      sprintf(tmp,format[ftype],hold);
      enc.append(tmp);
    }
    
    return enc;
  }
  
  string urldecode(const string& data)
  {
    char hold;
    unsigned tmp;
    string dec;
    const char* cstr = data.c_str();
 
    for (size_t i = 0; i < data.length(); i++) {
      if (cstr[i]  == '%') {
	sscanf(cstr+i+1,"%02X",&tmp);
	hold = (char) tmp;
	i+=2;
      }
      else if (data[i] == '+') hold = ' ';
      else if (data[i] == '&') hold = '\n';
      else hold = data[i];
      dec.push_back(hold);
    }
    return dec;
  }
  
  UniversalContainer uc_decode_ini(Buffer* buffer, bool form)
  {
    UniversalContainer uc;
    vector<string> parts;
    string split = "\n";
    
    string line(buffer->data,buffer->length); //read all data
    buffer->rpos = buffer->length;
    if (form) split = "&";
    vector<string> pairs = string_pieces(line,split);
    for (size_t i = 0; i < pairs.size(); i++) {
      if (!pairs[i].length()) continue;
      parts = string_split(pairs[i],"=");
      if (!parts.size()) {
	uc.string_interpret(string_chomp(pairs[i]));
	continue;
      }
      if (form) {
	parts[0] = urldecode(string_chomp(parts[0]));
	parts[1] = urldecode(string_chomp(parts[1]));
      }
      else {
	parts[0] = string_chomp(parts[0]);
	parts[1] = string_chomp(parts[1]);
      }
      uc[parts[0]].string_interpret(parts[1]);
    }
    return uc;
  }

  UniversalContainer uc_decode_form(Buffer* buffer)
  {
    return uc_decode_ini(buffer,true);
  }

  UniversalContainer uc_decode_ini(Buffer* buffer)
  {
    return uc_decode_ini(buffer,false);
  }
  
  void uc_encode_ini(const UniversalContainer& uc, Buffer* buffer, 
		     string prefix, bool form)
  {
    UniversalMap::iterator miter;
    UniversalMap::iterator mend;
    UniversalArray::iterator viter;
    UniversalArray::iterator vend;
    UniversalContainerType type = uc.get_type();

    string pfix;
    string tmp;
    int pos = 0;
    char buf[32];

    switch(type) {
    case uc_Map :				
      pfix = prefix;
      if (pfix.length() > 0) pfix.push_back('.');
      mend = uc.map_end();
      for(miter = uc.map_begin(); miter != mend; miter++) {
	if (miter->first[0] == '#') continue; //skip metadata
	tmp = pfix;
	tmp.append(miter->first);
	uc_encode_ini(miter->second,buffer,tmp,form);
      }
      break;
    case uc_Array :
      pfix = prefix;
      if (pfix.length() > 0) pfix.push_back('.');
      vend = uc.vector_end();
      for(viter = uc.vector_begin(); viter != vend; viter++,pos++) {
	sprintf(buf,"%d",pos);
	tmp = pfix;
	tmp.append(buf);
	uc_encode_ini(*viter,buffer,tmp,form); 
      }
      break;    
    case uc_Integer :
    case uc_Real :
    case uc_Character :
    case uc_Boolean :
    case uc_Null :
    case uc_String :
    case uc_WString :
      tmp.clear();
      if (prefix.length() != 0) {
	tmp.append(prefix);
	if (form) tmp = urlencode(tmp);
	buffer->put_data(tmp.c_str(),tmp.length());
	buffer->put('=');
      }
      if (form) tmp = urlencode(uc);
      else tmp = static_cast<string>(uc);
      buffer->put_data(tmp.c_str(),tmp.length());
      if (form) buffer->put('&');
      else buffer->put('\n');
      break; //if success, stop, else drop through and throw.
    default :
      throw ucexception(uce_Serialization_Error);
    }
  }

  Buffer* uc_encode_ini(const UniversalContainer& uc)
  {
    Buffer* buffer = new Buffer;
    string prefix;
    uc_encode_ini(uc, buffer, prefix, false);
    return buffer;
  }

  Buffer* uc_encode_form(const UniversalContainer& uc)
  {
    Buffer* buffer = new Buffer;
    string prefix;
    uc_encode_ini(uc, buffer, prefix, true);
    buffer->length--; //chop off the trailing &, what a hack
    return buffer;
  }

} //end namespace
