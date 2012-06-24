/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "buffer.h"

/*
  This file contains member definitions for the buffer
  class. buffer_util.cpp contains definitons for routines to interface
  buffers to actual streams and files. buffer_curl.cpp contains
  routines for sending buffers over curl handles.
*/
namespace JAD {

  Buffer::Buffer(int sz) //sz = 2048, default value in buffer.h
  {
    size = sz;
    data = new char[size];
    wpos = 0;
    rpos = 0;
    length = 0;
    own_data = true;
  }

  Buffer::Buffer(char* str)
  {
    size = strlen(str);
    data = str;
    wpos = size;
    rpos = 0;
    length = size;
    own_data = false;
  };
  
  Buffer::Buffer(void* d, size_t len)
  {
    size = len;
    data = (char*) d;
    wpos = len;
    rpos = 0;
    length = len;
    own_data = false;
  };

  Buffer::~Buffer(void)
  {
    if (own_data) delete[] data;
  }
  
  //This private routine makes sure that there are need bytes
  //available to a write operation. If the current allocation is
  //insufficent, it will keep doubling the allocation size until it is
  //sufficent. Then it allocates the block, copies the data over, and
  //frees the previoud allocation. If it fails, it return false.
  bool Buffer::ensure_space(size_t need)
  {
    size_t total_need = wpos + need;
    if (total_need <= size) return true;

    if (!own_data) return false; //if we don't own this buffer, return false;

    size_t nsize = size << 1;
    while (nsize < total_need) nsize <<= 1;
    char* ndata = new char[nsize];
    if (!ndata) return false;
    memcpy((void *)ndata, (void *)data, size);
    delete[] data;
    data = ndata;
    size = nsize;
    return true;
  }
  
  //copies len bytes from str to the buffer, adjusting
  //the write pointer as needed. returns t/f to indicate
  //success or failure.
  bool Buffer::put_data(const char* str, int len)
  {
    if (!ensure_space(len)) return false;
    for(int i = 0; i < len; i++,wpos++) 
      data[wpos] = str[i];
    if (length < wpos) length = wpos;
    return true;
  }
  
  //If len bytes are available to read, returns a pointer
  //to buffer storage. Then, advances the read pointer to
  //the following byte. If len bytes are not available, returns null.
  char* Buffer::fetch_data(size_t len)
  {
    if ((length-rpos) < len) return NULL;
    char* st = data + rpos;
    rpos += len;
    return st;
  }
  
  //copy up to len bytes to the destination string, advance as needed.
  //returns the number of bytes actualy copied out.
  size_t Buffer::copy_out(char* dest, size_t len)
  {
    size_t to_copy;
    size_t avail = length-rpos;
    to_copy = avail < len ? avail : len;
    if (to_copy) {
      memcpy(dest,data+rpos,to_copy);
      rpos += to_copy;
    }
    return to_copy;
  }

  //read to end of line ('\n') or end of file. Returns a string,
  //which is owned by the caller.
  char* Buffer::get_line(void)
  {
    size_t nl = rpos;
    char* tok;
    size_t len;
    
    if (rpos >= length) return NULL;
    while (nl < length && data[nl] != '\n') nl++;
    len = nl-rpos;
    tok = new char[len+1];
    if (len) memcpy(tok,data+rpos,len);
    tok[len] = '\0';
    rpos = nl+1;
    return tok;
  }
  
  //true if end of file has been reached for read purposes.
  bool Buffer::end(void) const
  {
    return (rpos >= length);
  }
  
  //reset the read pointer to the beginning of the file.
  void Buffer::rewind(void)
  {
    rpos = 0;
  }
  
  //reset the buffer to be empty
  void Buffer::clear(void)
  {
    rpos = 0;
    wpos = 0;
    length = 0;
  }
  
  //position both read and write head to a fied point.
  //not used by libuc, commented out
  /*
  bool Buffer::seek(size_t npos)
  {
    if (npos > length) return false;
    rpos = npos;
    wpos = npos;
    return true;
  }
  */

  //alas, templates don't work for member functions. so we fall back on this
  //hack. which is probably what you would want the compilier to do anyway.
#define PUT_MACRO(T) bool Buffer::put(T x) {if (!ensure_space(sizeof(T))) return false;T* ptr = (T*)(data+wpos);*ptr = x;wpos += sizeof(T);if (length < wpos) length = wpos; return true;}

#define FETCH_MACRO(T) bool Buffer::fetch(T& x) {if ((length-rpos) < sizeof(T)) return false; char* tmp = data+rpos; x=*((T*)(tmp));rpos += sizeof(T); return true;}

  //These define the put and fetch mehtods to put and fetch primitive types.
  PUT_MACRO(char)
  PUT_MACRO(unsigned char)
  PUT_MACRO(double)
  PUT_MACRO(bool)
  PUT_MACRO(int)
  PUT_MACRO(long)
  
  FETCH_MACRO(char)
  FETCH_MACRO(unsigned char)
  FETCH_MACRO(double)
  FETCH_MACRO(bool)
  FETCH_MACRO(int)
  FETCH_MACRO(long)
  
}// end namespace
