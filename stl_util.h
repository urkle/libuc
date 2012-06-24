/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */


#ifndef _STD_UTIL_H_
#define _STD_UTIL_H_

#include <map>
#include <vector>
#include <typeinfo>

template <typename A, typename B>
std::vector<A> keys_for_map(const std::map<A,B>& map)
{
	std::vector<A> list;
	typename std::map<A,B>::const_iterator iter = map.begin();
	typename std::map<A,B>::const_iterator end = map.end();
	
	for (;iter != end; iter++) 
		list.push_back(iter->first);
	
	return list;
}

template <typename A, typename B>
	std::vector<B> values_in_map(const std::map<A,B>& map)
{
	std::vector<B> list;
	typename std::map<A,B>::const_iterator iter = map.begin();
	typename std::map<A,B>::const_iterator end = map.end();
	
	for (;iter != end; iter++) 
		list.push_back(iter->second);
	
	return list;
}

template <typename A>
std::vector<A> vector_sublist(const std::vector<A>& list, int start, int len)
{
	std::vector<A> slist;
	
	if (len == -1)
		len = list.size() - start;
	
	for (int i = 0; i < len; i++)
		slist.push_back(list[start+i]);
	
	return slist;
}

template<typename A, typename B>
	bool compare_map(std::map<A,B>& map1, std::map<A,B>& map2)
{
	if (map1.size() != map2.size()) return false;
	
	typename std::map<A,B>::const_iterator iter = map1.begin();
	typename std::map<A,B>::const_iterator end = map1.end();
	
	bool same = true;
	while (same && iter != end) {
		same = (map1[iter->first] == map2[iter->first]);
		iter++;
	}
	return same;
}

template<typename A>
	bool compare_vector(std::vector<A>& list1, std::vector<A>& list2)
{
	size_t sz = list1.size();
	if (sz != list2.size()) return false;
	
	bool same = true;
	for (size_t i = 0; same && i < sz; i++) 
		same = (list1[i] == list2[i]);

	return same;
}

#endif
