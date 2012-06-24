/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

/*
  Routines to match buffers with FILE*, system handles (sockets), iostreams.
  Also includes facilites for base64 encoding.
 */

#include <stdio.h>
#include <iostream>
#include "buffer.h"

using namespace std;

namespace JAD {
  
  bool write_from_buffer(Buffer* buffer, ostream& fout)
  {
    if (buffer->rpos >= buffer->length) return false;
    fout.write(buffer->data+buffer->rpos,buffer->length-buffer->rpos);
    buffer->rpos = buffer->length;
    return !(fout.bad());
  }
  
  bool write_from_buffer(Buffer* buffer, FILE* fout)
  {
    if (buffer->rpos >= buffer->length) return false;
    size_t sent = fwrite(buffer->data+buffer->rpos,1,
			 buffer->length-buffer->rpos,fout);
    buffer->rpos += sent;
    return (buffer->length-buffer->rpos == sent);
  }
  
  bool write_from_buffer(Buffer* buffer, int fout)
  {
    int sent = 1;
    int total = 0;
    if (buffer->rpos >= buffer->length) return false;

    while (sent > 0) {
      sent = write(fout,buffer->data+total+buffer->rpos,
		   buffer->length-total-buffer->rpos);
      total += sent;
    }
    buffer->rpos += total;
    if (sent == -1) return false;
    return true;
  }
  
  bool write_from_buffer(Buffer* buffer, const char* filename)
  {
    FILE* fout = fopen(filename,"w");
    bool result = write_from_buffer(buffer,fout);
    fclose(fout);
    return result;
  }

  Buffer* read_to_buffer(FILE* fin)
  {
    char tmp[2048];
    size_t got;
    Buffer* buffer = new Buffer;
    
    while (!feof(fin) && !ferror(fin)) {
      got = fread(tmp,1,2048,fin);
      buffer->put_data(tmp,got);
    }
    
    if (ferror(fin)) {
      delete buffer;
      return NULL;
    }
    
    return buffer;
  }
  
  Buffer* read_to_buffer(int fin)
  {
    char tmp[2048];
    ssize_t got = 1;
    Buffer* buffer = new Buffer;
    
    while (got > 0) {
      got = read(fin,tmp,2048);
      buffer->put_data(tmp,got);
    }
    
    if (got == -1) {
      delete buffer;
      return NULL;
    }
    
    return buffer;
  }
  
  Buffer* read_to_buffer(istream& fin)
  {
    char tmp[2048];
    size_t got = 1;
    Buffer* buffer = new Buffer;
    
    while (got > 0) {
      fin.read(tmp,2048);
      got = fin.gcount();
      buffer->put_data(tmp,got);
    }
    
    if (fin.bad()) {
      delete buffer;
      return NULL;
    }
    
    return buffer;
  }

  Buffer* read_to_buffer(const char* filename)
  {
    FILE* fin = fopen(filename,"r");
    Buffer* result = read_to_buffer(fin);
    fclose(fin);
    return result;
  }

  char base64_char_for_byte(unsigned char byte)
  {
    if (byte <= 25) return 'A' + byte;
    if (byte <= 51) return 'a' + byte - 26;
    if (byte <= 61) return '0' + byte - 52;
    if (byte == 62) return '+';
    if (byte == 63) return '/';
    return -1;
  }
  
  unsigned char base64_byte_for_char(char b64)
  {
    if (b64 >= 'A' && b64 <= 'Z') return b64 - 'A';
    if (b64 >= 'a' && b64 <= 'z') return b64 - 'a' + 26;
    if (b64 >= '0' && b64 <= '9') return b64 - '0' + 52;
    if (b64 == '+') return 62;
    if (b64 == '/') return 63;
    else if (b64 == '=') return 64; //this is the padding character
    return 127; //invalid char return
  }
  
  Buffer* base64_decode(Buffer* orig)
  {
    char next;
    unsigned char byte = 0;
    unsigned char p1, p2, p3 = 0;
    int taken = 0;
    unsigned accum = 0;
    Buffer* result = new Buffer(((orig->length / 4) + 1) * 3);
    
    while (!orig->end()) {
      if (!orig->fetch(next)) continue;
      if (next == '\0') continue;
      
      byte = base64_byte_for_char(next);
      if (byte == 64) continue;
      if (byte == 127) { delete result; return NULL;}

      accum <<= 6;
      accum += byte;
      taken++;
	
      if (taken == 4) {
	p3 = (accum & 0xFF); accum >>= 8;
	p2 = (accum & 0xFF); accum >>= 8;
	p1 = (accum & 0xFF); accum >>= 8;  //last shift zeros accum
	result->put(p1);
	result->put(p2);
	result->put(p3);
	taken = 0;
      }
    }

    if (taken == 1) {
      delete result;
      return NULL;
    }

    if (taken == 2) {
      accum >>= 4;
      p1 = accum & 0xFF;
      result->put(p1);
    }

    if (taken == 3) {
      accum >>= 2;
      p2 = accum & 0xFF;
      accum >>= 8;
      p1 = accum & 0xFF;
      result->put(p1);
      result->put(p2);
    }

    return result;
  }
  
  Buffer* base64_encode(Buffer* orig)
  {
    Buffer* result = new Buffer(((orig->length / 3) + 1) * 4 + 1);
    unsigned char next;
    char p1, p2, p3, p4;
    unsigned accum = 0;
    int taken = 0;

    while (!orig->end()) {
      if (!orig->fetch(next)) continue;
      accum <<= 8;
      accum += next;
      taken++;
      if (taken == 3) {
	p4 = base64_char_for_byte(accum & 0x3F); accum >>= 6;
	p3 = base64_char_for_byte(accum & 0x3F); accum >>= 6;
	p2 = base64_char_for_byte(accum & 0x3F); accum >>= 6;
	p1 = base64_char_for_byte(accum & 0x3F); accum >>= 6;
	//last shift zeros accum
	taken = 0;
	result->put(p1);
	result->put(p2);
	result->put(p3);
	result->put(p4);
      }
    }
    if (taken == 1) {
      accum <<= 4;
      p2 = base64_char_for_byte(accum & 0x3F); accum >>= 6;
      p1 = base64_char_for_byte(accum & 0x3F); 
      result->put(p1);
      result->put(p2);
      result->put('=');
    }

    if (taken == 2) {
      accum <<= 2;
      p3 = base64_char_for_byte(accum & 0x3F); accum >>= 6;
      p2 = base64_char_for_byte(accum & 0x3F); accum >>= 6;
      p1 = base64_char_for_byte(accum & 0x3F);
      result->put(p1);
      result->put(p2);
      result->put(p3);
      result->put('=');
    }

    result->put('\0'); //properly terminate the string, just in case;
    return result;
  }
} //end namespace
