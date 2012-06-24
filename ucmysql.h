/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

/* 
   UniversalContainer to mysql interface.
*/

#ifndef _UCMYSQL_H_
#define _UCMYSQL_H_

#include <mysql.h>

#include "ucontainer.h"
#include "ucdb.h"

using namespace std;

namespace JAD {
  class MySQLDatabase: public DatabaseInterface {
    MYSQL* connection;
    bool own_db;

    void set_error_condition(UniversalContainer&);
    void real_setup(void);      
  public:
    MySQLDatabase(const char* = NULL, const char* = NULL, const char* = NULL,			  const char* = NULL, unsigned = 0, const char* = NULL,
		  unsigned = 0);    
    MySQLDatabase(UniversalContainer);
    MySQLDatabase(MYSQL*);
    ~MySQLDatabase(void);

    UniversalContainer sql_exec(std::string);
    MYSQL* get_db_handle(void);
  };
}
#endif
