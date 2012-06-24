/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

#include <iostream>

//The header univcont.h includes all the different pieces of the libuc library.
//#include "univcont.h"
#include "ucontainer.h"
#include "ucio.h"
#include "buffer.h"

using namespace std;

//UniversalContainer and friends are in the JAD namespace
using namespace JAD;

void print_person(UniversalContainer& uc)
{
  string name = uc["name"];
  string age = uc["age"];
  char sex = uc["sex"];
  
  cout << name << '\t' << sex << '\t' << age << endl;
}

/*
  A simple example that declares a record holding the personal records
  of a solider and his family.
 */
int main(int argc, char** argv)
{
  
  try {
    UniversalContainer solider;
    
    //Note all the different types of primitives that can be assigned to
    //arbitrary keys.
    solider["name"] = "Kansas Smith";
    solider["age"] = 23;
    solider["officer"] = false;
    solider["serial"] = 789456;
    solider["sex"] = "M";
    solider["wage"] = 25.75;
    
    //We can easily nest UniversalContainers
    solider["spouse"]["name"] = "Sue Smith";
    solider["spouse"]["sex"] = "F";
    solider["spouse"]["age"] = "Won't Say";
    
    //Or have a container act as an array.
    //In this case, the key "dependants" maps to a UniversalContainer that is an array of
    //other UniversalContainers, which in turn have their own keys.
    solider["dependants"][0]["name"] = "Joe Smith";
    solider["dependants"][0]["age"] = 3;
    solider["dependants"][0]["sex"] = "M";
    solider["dependants"][1]["name"] = "Ann Smith";
    solider["dependants"][1]["age"] = 1;
    solider["dependants"][1]["sex"] = "F";
    solider["dependants"][2]["name"] = "On the way";
    
    cout << "NAME" << "\t\t" << "SEX" << "\t" << "AGE" << endl;
    print_person(solider);
    
    //A UC can be a map between strings and other UCs.
    print_person(solider["spouse"]);
    
    //We can loop through the array of dependants
    for (unsigned i = 0; i < solider["dependants"].size(); i++)
      print_person(solider["dependants"][i]);
    
    //Here is an example of using dot notation to get at nested elements
    cout << "The litte girls name is " << 
      (string)solider["dependants.1.name"] << endl;
    
    //Here we show how a container may be cast as the type it is holding
    double combat_pay = (double)solider["wage"] + 5.35;
    cout << "Combat pay is " << combat_pay << endl;
    
    //This is essential what the print statement does,
    //we show it here as an example of using the serializer code.
    Buffer* buf = uc_encode_ini(solider);
    write_from_buffer(buf,stdout);
    delete buf;
   
  }
  //We shouldn't ever be here, but if we are, the exception will contain
  //key/value pairs describing what went wrong.
  catch(UniversalContainer uce) {
    cout << "An exception was thrown." << endl;
    print(uce);
  }
}
