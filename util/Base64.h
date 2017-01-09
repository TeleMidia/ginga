/*
   Base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#ifndef BASE64_H_
#define BASE64_H_

#include "util/functions.h"

#include <string>
#include <iostream>
using namespace std;

TELEMIDIA_UTIL_BEGIN_DECLS

string base64_encode(unsigned char const* , unsigned int len);
string base64_decode(std::string const& s);
string getBase64FromFile(string file_path);
int writeFileFromBase64(string payload, char * file_path);

TELEMIDIA_UTIL_END_DECLS

#endif /* BASE64_H_ */
