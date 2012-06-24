/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#ifndef _UCONTAINER_HEADER_
#define _UCONTAINER_HEADER_

#include <string>
#include <map>
#include <vector>

/*
 * Used internally by UniversalContainer. Almost always you will
 * want to wrap these in a UniversalContainer rather than using these types.
 */

namespace JAD {

  const char uc_Null       = 0;
  const char uc_Integer    = 1;
  const char uc_Boolean    = 2;
  const char uc_Character  = 3;
  const char uc_String     = 4;
  const char uc_WString    = 5;
  const char uc_Real       = 6;
  const char uc_Map        =10;
  const char uc_Array      =11;
  const char uc_Unknown    =-1;

  
  typedef char UniversalContainerType;
  class UniversalContainer;
  
  typedef std::vector<UniversalContainer> UniversalArray;
  typedef std::map<std::string,UniversalContainer> UniversalMap;

  //todo
  //set reference
  //size reduction
  //dirty flag
  class UniversalContainer
  {
  protected :
    
    //member variables
    UniversalContainerType type;
    bool dirty;
    unsigned* refcount;
    
    union {
      double real;
      bool tf;
      char chr;
      long num;
      std::string* str;
      std::wstring* wstr;
      UniversalArray* ray;
      UniversalMap* map;
      void* reference;
    } data;
    
    //internal setter methods
    //used by constructors and assignment operators
    inline void set_value_integer(long);
    inline void set_value_char(char);
    inline void set_value_double(double);
    inline void set_value_bool(bool);
    inline void set_value_string(const std::string&);
    inline void set_value_wstring(const std::wstring&);
    inline void set_value_cstr(const char*);
    
    //internal conversion methods
    //used by casting and equality testing operators
    inline int convert_int(void) const;
    inline double convert_double(void) const;
    inline char convert_char(void) const;
    inline bool convert_bool(void) const;
    inline long convert_long(void) const;
    inline std::string convert_string(void) const;
    inline std::wstring convert_wstring(void) const;
    
    //internal designates. These routines are generally
    //exposed through multiple interfaces, which are
    //more user friendly
    void duplicate(const UniversalContainer&);
    inline UniversalContainer& map_brackets(std::string);
    
    //utility functions
    static std::string convert_wstring_to_string(const std::wstring* w);
    static std::wstring convert_string_to_wstring(const std::string* s);
    static double convert_string_to_double(const std::string* s);
    static double convert_wstring_to_double(const std::wstring* s);
    static long convert_string_to_long(const std::string* s);
    static long convert_wstring_to_long(const std::wstring* s);
   
  public: 
    //constructors
    UniversalContainer(void);
    UniversalContainer(int);
    UniversalContainer(long);
    UniversalContainer(double);
    UniversalContainer(bool);
    UniversalContainer(char);
    UniversalContainer(const std::string);
    UniversalContainer(const std::wstring);
    UniversalContainer(char*);
    UniversalContainer(const UniversalContainer&);
    void string_interpret(const std::string s);
    
    //destructor
    ~UniversalContainer(void);
    
    //cast operators
    operator int(void) const;
    operator double(void) const;
    operator char(void) const;
    operator bool(void) const;
    operator long(void) const;
    operator std::string(void) const;
    operator std::string*(void) const;
    operator std::wstring(void) const;
    operator std::wstring*(void) const;
    const char* c_str(void) const;
    
    //container access
    UniversalContainer& operator[](int);
    UniversalContainer& operator[](std::string);
    UniversalContainer& operator[](std::wstring);
    UniversalContainer& operator[](char*);
    UniversalContainer& operator[](const char*);
    
    //assignment operators
    UniversalContainer& operator=(long);
    UniversalContainer& operator=(int);
    UniversalContainer& operator=(double);
    UniversalContainer& operator=(bool);
    UniversalContainer& operator=(char);
    UniversalContainer& operator=(const std::string&);
    UniversalContainer& operator=(const std::wstring&);
    UniversalContainer& operator=(const char*);
    UniversalContainer& operator=(const UniversalContainer&);
    
    //logical operators
    bool operator==(const UniversalContainer&) const;
    bool operator==(int) const;
    bool operator==(long) const;
    bool operator==(char) const;
    bool operator==(double) const;
    bool operator==(std::string) const;
    bool operator==(std::wstring) const;
    bool operator==(bool) const;
    
    //support operations
    UniversalContainerType get_type(void) const;
    UniversalContainer clone(void) const;
    bool remove(const std::string key);
    void clear(void);
    bool exists(const std::string key) const;
    size_t size(void) const;
    size_t length(void) const;
    bool is_dirty(void) const;
    void clean(void);
    UniversalContainer& added_element(void);
    std::vector<std::string> keys_for_map(void) const;
    
    //allow the user to container type
    void init_map(void);
    void init_array(void);

    //Support for maps, vectors, and iterators
    UniversalMap::iterator map_begin(void) const; 
    UniversalMap::iterator map_end(void) const;
    UniversalArray::iterator vector_begin(void) const;
    UniversalArray::iterator vector_end(void) const;
    UniversalMap* get_map(void) const;
    UniversalArray* get_vector(void) const;
    
    static UniversalContainer construct_exception(const int, const char* = NULL,
						  const char* = NULL,
						  int = 0, 
						  const UniversalContainer* = NULL);
  };
  
} //end namespace

#ifndef NDEBUG
#define ucexception(C) UniversalContainer::construct_exception(C,__FUNCTION__,__FILE__,__LINE__,NULL)
#else
#define ucexception(C) UniversalContainer::construct_exception(C,NULL,NULL,0,NULL)
#endif

/* Defines for exception error codes */
#define uce_Unknown 0
#define uce_TypeMismatch_Write 1
#define uce_TypeMismatch_Read 2
#define uce_Collection_as_Scalar 3
#define uce_Scalar_as_Collection 4
#define uce_Non_Map_as_Map 5
#define uce_Non_Array_as_Array 6
#define uce_Array_Subscript_Out_of_Bounds 7 
#define uce_Deserialization_Error 8
#define uce_Serialization_Error 9
#define uce_DB_Connection 10
#define uce_Unknown_mime_type 11
#define uce_Communication_Error 12
#define uce_ContractViolation 13
#endif
