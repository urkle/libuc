/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <iostream>
#include "ucmysql.h"

using namespace std;

/*
  UniversalContainer interface to MySQL DB
*/

namespace JAD {
  
  //maximum info constructor
  MySQLDatabase::MySQLDatabase(const char* hostname, const char* username,
			       const char* password, const char* db_name,
			       unsigned port, const char* socket,
			       unsigned flags)
  {
    db_info["hostname"] = hostname;
    db_info["username"] = username;
    db_info["password"] = password;
    db_info["db_name"] = db_name;
    db_info["socket_name"] = socket;
    db_info["port"] = (long) port;
    db_info["optflags"] = (long) flags;
    real_setup();
  }
  
  //constructor which uses a uc which containers the needed parameters
  MySQLDatabase::MySQLDatabase(UniversalContainer uc)
  {
    db_info = uc.clone();
    real_setup();
  }

  //setup an object based on an existing db handle
  MySQLDatabase::MySQLDatabase(MYSQL* h)
  {
    connection = h;
    own_db = false;
  }
  
  //real setup work happens here
  void MySQLDatabase::real_setup(void)
  {  
    db_info["database_type"] = "MySQL";
    connection = mysql_init(NULL);
    if (!connection)
      throw ucexception(uce_DB_Connection);
    own_db = true;
    string hostname;
    if (db_info.exists("hostname"))
      hostname = (string)db_info["hostname"];

    string user;
    if (db_info.exists("username"))
      user = (string)db_info["username"];

    string password;
    if (db_info.exists("password"))
      password = (string)db_info["password"];

    string dbname;
    if (db_info.exists("db_name"))
      dbname = (string)db_info["db_name"];

    string socket;
    if (db_info.exists("socket_name"))
      socket = (string)db_info["socket_name"];

    long port;
    if (db_info.exists("port"))
      port = db_info["port"];
    else port = 0;

    long flags;
    if (db_info.exists("optflags"))
      flags = db_info["optflags"];
    else flags = 0;
 
    if (!mysql_real_connect(connection, hostname.c_str(), user.c_str(), 
			   password.c_str(), dbname.c_str(),
			    (unsigned)port, socket.c_str(), (unsigned)flags)) {
      UniversalContainer exp = ucexception(uce_DB_Connection);
      exp["error"] = mysql_error(connection);
      throw exp;
    }
  }
  
  MySQLDatabase::~MySQLDatabase(void)
  {
    if (own_db) mysql_close(connection);
  }
  
  void MySQLDatabase::set_error_condition(UniversalContainer& uc)
  {
      uc["message"] = mysql_error(connection);
      uc["error_code"] = mysql_error(connection);
      uc["#boolean_value"] = false;
  }
  
  //execute a query against the db, parse results into a uc for return
  UniversalContainer MySQLDatabase::sql_exec(string q)
  {
    UniversalContainer uc;
    MYSQL_RES* result_set;
    MYSQL_ROW row;
    MYSQL_FIELD *fields;
    int num_fields;
    int idx = 0;
    int rc;

    
    rc = mysql_query(connection,q.c_str());
   
    if (rc != 0) {
      set_error_condition(uc);
      return uc;
    }
    
    result_set = mysql_use_result(connection);
    if (result_set == NULL) {
      if (mysql_field_count(connection)) { //we should have gotten results
	set_error_condition(uc);
	return uc;
      }
      else { //that query did not need to return results
	uc["#boolean_value"] = true;
	uc["message"] = "success";
	uc["status_code"] = 0;
	return uc;
      }
    }

    num_fields = mysql_num_fields(result_set);
    fields = mysql_fetch_fields(result_set);

    while ((row = mysql_fetch_row(result_set)) != NULL) {
      for (int i = 0; i < num_fields; i++) 
	uc[idx][fields[i].name].string_interpret(row[i]);
      idx++;
    }

    mysql_free_result(result_set);
    return uc;
  }

  //access to the underlying db handle
  MYSQL* MySQLDatabase::get_db_handle(void) {return connection;}
} //end namespace
