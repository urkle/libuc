/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

/*
  Abstract base class for interfacing SQL and UC.
 */

#ifndef _UCDB_H_
#define _UCDB_H_

#include "ucontainer.h"

using namespace std;

namespace JAD {

  class DatabaseInterface {
  protected:
    UniversalContainer db_info;
  public:
    virtual UniversalContainer sql_exec(std::string) = 0;
    virtual UniversalContainer get_db_info(void);
    virtual bool start_transaction(void);
    virtual bool commit(void);
    virtual bool rollback(void);
    virtual ~DatabaseInterface(void) {};
  };
}

#endif
