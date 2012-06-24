/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string>
#include <vector>

//Prototype declarations
std::string string_chomp(const std::string& str);
std::vector<std::string> string_tokens(const std::string&,const std::string&);
std::vector<std::string> string_pieces(const std::string&,const std::string&);
std::vector<std::string> string_split(const std::string&,const std::string&);
#endif
