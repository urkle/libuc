/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <iostream>
#include "ucsqlite.h"

using namespace std;

/*
  Interface to sqlite3 databases for libuc.
 */

namespace JAD {

  void SQLiteDatabase::real_setup(const char* filename)
  {
    db_info["database_type"] = "SQLite";
    db_info["filename"] = filename;
    own_db = true;
    if (sqlite3_open(filename,&database))
      throw ucexception(uce_DB_Connection);
  }

  SQLiteDatabase::SQLiteDatabase(string& filename)
  {
    real_setup(filename.c_str());
  }
  
  SQLiteDatabase::SQLiteDatabase(const char* filename)
  {
    real_setup(filename);
  }

  //This constructor allows an externally created DB to be used.
  SQLiteDatabase::SQLiteDatabase(sqlite3* db)
  {
    db_info["database_type"] = "SQLite";
    database = db;
    own_db = false;
    
  }
  
  SQLiteDatabase::~SQLiteDatabase(void)
  {
    if (own_db) sqlite3_close(database);
  }

  //return the db handle to enable more complete db operations
  //using sqlite3 libraries.
  sqlite3* SQLiteDatabase::get_db_handle(void)
  {
    return database;
  }  
  
  //for queries that returns a result set, this is called once
  //for each row. It gets a pointer to a uc, and parses the
  //incoming data into that uc.
  int SQLiteDatabase::sqlite_callback(void* context, int argc,
				      char** argv, char** colname)
  {
    UniversalContainer* uc = static_cast<UniversalContainer*>(context);
    int pos = uc->size();
    for (int i = 0; i < argc; i++) 
      (*uc)[pos][colname[i]].string_interpret(argv[i]);
    return 0;
  }
  
  //execute a query and setup the uc to hold the proper return values.
  UniversalContainer SQLiteDatabase::sql_exec(string query)
  {
    UniversalContainer uc;
    char* err = NULL;
    
    int status;
    status = sqlite3_exec(database,query.c_str(),sqlite_callback, &uc,
			  &err);
    if (status) {
      uc["status_code"] = status;
      uc["message"] = err;
      uc["#boolean_value"] = false;
      sqlite3_free(err);
    }
    else if (!uc) { // if operation did nothing with the uc
	uc["#boolean_value"] = true;
	uc["message"] = "success";
	uc["status_code"] = 0;
    }
    return uc;
  }
} //end namespace
