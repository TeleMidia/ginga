// CompTests.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "util/functions.h"
using namespace ::ginga::util;

#include "ncl/Entity.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <iostream>
using namespace std;

int
main ()
{
  /*********** TELEMIDIA-UTIL-CPP ******************
          double initTime;
          string hms;

          clog << "telemidia-util tests" << endl;

          initTime = getCurrentTimeMillis();
          clog << "initTime = '" << initTime << "'";
          clog << endl;

          hms = "0:2:3.4";
          clog << "str hms = '" << hms << "' converted to '" <<
  strUTCToSec(hms);
          clog << "' seconds" << endl;

          string test = "tEsT";

          while (true) {
                  cout << "upperCase: " << (test = upperCase (test)) << endl;
                  cout << "upperCase: " << (test = lowerCase (test)) << endl;

  #ifndef _MSC_VER
                  ::usleep(1000000);
  #else
                  Sleep(1000);
  #endif
                  clog << "elapsedTime = '" << getCurrentTimeMillis() -
  initTime << "'";
                  clog << endl;
          }
  ***************************************************/
  /*********** NCL30 *****************
          Entity ent("MyEntity");
          string id = ent.getId ();
          cout << "id = " << id << endl;
          cout << "id = \"MyEntity\" ? " << ( id == "MyEntity" ) << endl;
          cout << ent.instanceOf ("media") << endl;
          cout << ent.instanceOf ("MyEntity") << endl;
          cout << ent.toString ()<< endl;

  ***************************************************/

  const int a = 5;
  int v[a];

  for (int i = 0; i < 5; i++)
    v[i] = i;

  return 0;
}
