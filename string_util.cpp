/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <string>
#include <map>
#include <vector>
#include <typeinfo>

/*
  Generic string processing that I find useful.
 */

using namespace std;

//strip whitespace off either end of strings
std::string string_chomp(const std::string& str)
{
  string ret;
  int start = str.find_first_not_of("\n \t");
  int end = str.find_last_not_of("\n \t");
  if (start >= 0)
    ret = str.substr(start, end-start+1);
  return ret;
}

/*
  Like string_tokens, but splits when any character in delim is found.
  This is equivalent to k&r str_tok
 */
std::vector<std::string> string_tokens(const std::string& str, 
				       const std::string& delim)
{
  std::vector<std::string> list;
  std::string next;
  size_t start = 0;
  size_t pos = str.find_first_of(delim);
  bool end = false;

  while (!end) {
    next = str.substr(start,pos-start);
    list.push_back(next);
    start = pos+1;
    if (pos == std::string::npos) end = true;
    else pos = str.find_first_of(delim,start);
  }

  return list;
}

/*
  Split a string into tokens, deliminated by the compelete
  string delim. Deliminators not output.
 */
std::vector<std::string> string_pieces(const std::string& str, 
				       const std::string& delim)
{
  std::vector<std::string> list;
  std::string next;
  size_t start = 0;
  size_t pos = str.find(delim);
  bool end = false;
  size_t inc = delim.length();

  while (!end) {
    next = str.substr(start,pos-start);
    list.push_back(next);
    start = pos+inc;
    if (pos == std::string::npos) end = true;
    else pos = str.find(delim,start);
  }

  return list;
}

/*
  Split a string into two parts
 */
std::vector<std::string> string_split(const std::string& str,
				   const std::string& delim)
{
  std::vector<std::string> list;
  size_t pos = str.find_first_of(delim);

  if (pos != string::npos) {
    list.push_back(str.substr(0,pos));
    list.push_back(str.substr(pos+1));
  }
  return list;
}
