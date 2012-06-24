/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include "ucdb.h"

/*
  Method methods which are inherited by the concrete implementations of
  the DatabaseInterface, like SQLiteDatabase and MySQLDatabase.
 */

namespace JAD {

  bool DatabaseInterface::start_transaction(void)
  {
    return sql_exec("START TRANSACTION");
  }
  
  bool DatabaseInterface::commit(void)
  {
    return sql_exec("COMMIT");
  }

  bool DatabaseInterface::rollback(void)
  {
    return sql_exec("ROLLBACK");
  }						
  UniversalContainer DatabaseInterface::get_db_info(void)
  {
    return db_info;
  }
}// end namespace
