/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <string>
#include "ucontainer.h"
#include "buffer.h"

using namespace std;

/* 
   UniversalContainer binary encoder. Probably less useful than the
   others, expect perhaps for shoving ucs in to external databases.
*/

namespace JAD {

//Binary encoder users sn1 style size fields. If high bit of first
//byte is not set, then that byte is the size. If the high bit is
//set, then clear the bit to determine the number of bytes that
//encode the size.

  void put_size_field(Buffer* buffer, size_t size)
  {
    unsigned char bt;
    unsigned tmp = 0;
    
    if (size < 128) {
      bt = (unsigned char) size;
      if (!buffer->put(bt)) throw ucexception(uce_Serialization_Error);
      return;
    }
    
    unsigned char nb; //number of size bytes needed
    if (size < 255) nb = 1;
    else if (size < 255 * 255 - 1) nb = 2;
    else if (size < 255 * 255 * 255 - 1) nb = 3;
    else nb = 4;
    //write the number of bytes needed to encode size
    if (!buffer->put(128 + nb))
       throw ucexception(uce_Serialization_Error); 
    
    for (;nb > 0; nb--) {
      tmp >>= 8 * (nb-1); //shift top bit of size to bottom
      tmp &= 0x000000FF; //clear out the rest
      bt = (unsigned char) tmp;
      if (!buffer->put(bt)) throw ucexception(uce_Serialization_Error); 
    }
  }
  
  size_t get_size_field(Buffer* buffer)
  {
    unsigned char tmp;
    int nb;
    size_t size = 0;
    
    if (!buffer->fetch(tmp)) throw ucexception(uce_Deserialization_Error);
    if (tmp < 128) return (size_t) tmp;
    
    nb = tmp - 128;
    for (int i = 0; i < nb; i++) {
      if (!buffer->fetch(tmp)) throw ucexception(uce_Deserialization_Error);
      size <<= 8;
      size += tmp;
    }
    return size;
  }
  
  UniversalContainer uc_decode_binary(Buffer* buffer)
  {
    UniversalContainerType type;
    if (!buffer->fetch(type)) throw ucexception(uce_Deserialization_Error);
    char* tmp;

    UniversalContainer uc;
    long l;
    char c;
    bool b;
    double r;
    string s;
    wstring w; 
    size_t len;
    size_t sz;
    
    switch(type) {
    case uc_Integer :
      if (!buffer->fetch(l)) throw ucexception(uce_Deserialization_Error);
      uc = l;
      break;
    case uc_Boolean :
      if (!buffer->fetch(b)) throw ucexception(uce_Deserialization_Error);
      uc = b;
      break;
    case uc_Character :
      if (!buffer->fetch(c)) throw ucexception(uce_Deserialization_Error);
      uc = c;
      break;
    case uc_Real :
      if (!buffer->fetch(r)) throw ucexception(uce_Deserialization_Error);
      uc = r;
      break;
    case uc_String :
      sz = get_size_field(buffer);
      tmp = buffer->fetch_data(sz);
      if (!tmp) throw ucexception(uce_Deserialization_Error);
      s.clear();
      s.insert(0,tmp,sz);
      uc = s;
      break;
    case uc_WString :
      sz = get_size_field(buffer);
      tmp = buffer->fetch_data(sz*sizeof(wchar_t));
      if (!tmp) throw ucexception(uce_Deserialization_Error);
      w.clear();
      w.insert(0,(wchar_t*)tmp,sz);
      uc = w;    
      break;
    case uc_Map :
      len = get_size_field(buffer);
      for (size_t j = 0; j < len; j++) {
	sz = get_size_field(buffer);
	tmp = buffer->fetch_data(sz);
	if (!tmp) throw ucexception(uce_Deserialization_Error);
	s.clear();
	s.insert(0,tmp,sz);
	uc[s] = uc_decode_binary(buffer);
      }
      break;
    case uc_Array :
      len = get_size_field(buffer);
      for (size_t j = 0; j < len; j++) 
	uc[j] = uc_decode_binary(buffer);
      break;
    case uc_Null :
      break;
    default :
      throw ucexception(uce_Deserialization_Error);
    }
    return uc;
  }
  
  void uc_encode_binary(const UniversalContainer& uc, Buffer* buffer)
  {
    UniversalContainerType type = uc.get_type();
    
    UniversalMap::iterator miter;
    UniversalMap::iterator mend;
    UniversalArray::iterator viter;
    UniversalArray::iterator vend;
    bool tmp;

    if (!buffer->put(type)) throw ucexception(uce_Serialization_Error);
 
    switch(type) {
    case uc_Integer :
      if (!buffer->put(static_cast<int>(uc))) 
	throw ucexception(uce_Serialization_Error);
      break;
    case uc_Real :
      if (!buffer->put(static_cast<double>(uc)))
	  throw ucexception(uce_Serialization_Error);
      break;
    case uc_Boolean :
      tmp = uc ? true : false;
      if(!buffer->put(tmp))
	throw ucexception(uce_Serialization_Error);	
      break;
    case uc_Character :
      if (!buffer->put(static_cast<char>(uc)))
	throw ucexception(uce_Serialization_Error);
      break;
    case uc_String :
      put_size_field(buffer,uc.length());
      if (!buffer->put_data(static_cast<string*>(uc)->c_str(),uc.length()))
	throw ucexception(uce_Serialization_Error);
      break;
    case uc_WString :
      put_size_field(buffer,uc.length());
      if (!buffer->put_data((char*)(static_cast<wstring*>(uc)->c_str()),
			    uc.length()*sizeof(wchar_t)))
	throw ucexception(uce_Serialization_Error);
      break;
    case uc_Null :
      break;
    case uc_Map :
      put_size_field(buffer,uc.length());
      mend = uc.map_end();
      for (miter = uc.map_begin(); miter != mend; miter++) {
	put_size_field(buffer,miter->first.length());
	if (!buffer->put_data(miter->first.c_str(),miter->first.length()))
	  throw ucexception(uce_Serialization_Error);
	uc_encode_binary(miter->second,buffer);
      }
      break;
    case uc_Array :
      put_size_field(buffer,uc.length());
      vend = uc.vector_end();
      for (viter = uc.vector_begin(); viter != vend; viter++) 
	uc_encode_binary(*viter,buffer);
      break;
    case uc_Unknown :
      throw ucexception(uce_Serialization_Error);
    }
  }
  
  Buffer* uc_encode_binary(const UniversalContainer& uc)
  {
    Buffer* buffer = new Buffer;
    uc_encode_binary(uc,buffer);
    return buffer;
  }

} //end namespace
