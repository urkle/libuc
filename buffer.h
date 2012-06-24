/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */


/*
The buffer class provides a neutral interface between various
serializers and deserializers, and the various forms of I/O in
C/C++. For instance, between FILE*, iostream, raw handles (sockets),
CURL, etc.
 */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdio.h>
#include <iostream>

using namespace std;

namespace JAD {

  struct Buffer {
    char* data;
    size_t size;
    size_t rpos;
    size_t wpos;
    size_t length;
    bool own_data;

    //default to 32k buffer. expansion is costly, memory is so very cheap
    Buffer(int sz = 1024 * 32);
    Buffer(char*);
    Buffer(void*,size_t);
    ~Buffer(void);
   
    bool put_data(const char*, int);
    char* fetch_data(size_t);
    bool ensure_space(size_t);
    size_t copy_out(char*, size_t);
    
    char* get_line(void);
    bool end(void) const;
    void rewind(void);
    void clear(void);
    bool seek(size_t);

    bool put(char);
    bool put(unsigned char);
    bool put(int);
    bool put(long);
    bool put(bool);
    bool put(double);
    
    bool fetch(char&);
    bool fetch(unsigned char&);
    bool fetch(int&);
    bool fetch(long&);
    bool fetch(bool&);
    bool fetch(double&);
  };

  //non-member utilities found in buffer_util.cpp
  bool write_from_buffer(Buffer*, ostream&);
  bool write_from_buffer(Buffer*, FILE*);
  bool write_from_buffer(Buffer*, int);
  bool write_from_buffer(Buffer*, const char*);
  Buffer* read_to_buffer(FILE*);
  Buffer* read_to_buffer(int);
  Buffer* read_to_buffer(istream&);
  Buffer* read_to_buffer(const char*);

  Buffer* base64_decode(Buffer*);
  Buffer* base64_encode(Buffer*);

  //routines requiring curl, found in buffer_curl.cpp
  Buffer* http_post_buffer(const char*, Buffer*, string&, const char* = NULL, const int timeout = -1);
  Buffer* http_get_buffer(const char*, string&, const int timeout = -1);

} //end namespace

#endif




