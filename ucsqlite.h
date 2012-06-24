/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

/*
  Header for UniversalContainer SQLite3 interface.
 */

#ifndef _UCSQLITE_H_
#define _UCSQLITE_H_

#include <sqlite3.h>
#include "ucontainer.h"
#include "ucdb.h"

using namespace std;

namespace JAD {

  class SQLiteDatabase: public DatabaseInterface {
  private:
    sqlite3* database;
    static int sqlite_callback(void*,int,char**, char**);
    void real_setup(const char* filename);
    bool own_db;

  public:
    SQLiteDatabase(std::string&);
    SQLiteDatabase(const char*);
    SQLiteDatabase(sqlite3*);
    ~SQLiteDatabase(void);
    
    UniversalContainer sql_exec(std::string);
    sqlite3* get_db_handle(void);
  };
}

#endif
