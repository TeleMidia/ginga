/*
 * HiperlinkDescriptor.h
 *
 *  Created on: Nov 10, 2009
 *      Author: user
 */

#ifndef HIPERLINKDESCRIPTOR_H_
#define HIPERLINKDESCRIPTOR_H_

#include "IMpegDescriptor.h"
using namespace ::ginga::tsparser;

GINGA_TSPARSER_BEGIN

	class HiperlinkDescriptor : public IMpegDescriptor{
		protected:
			unsigned char hyperLinkageType;
			unsigned char linkDestinationType;
			unsigned char selectorLength;
			char* selectorByte;

		public:
			HiperlinkDescriptor();
			virtual ~HiperlinkDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			void print();
			size_t process(char* data, size_t pos);
	};


GINGA_TSPARSER_END

#endif /* HIPERLINKDESCRIPTOR_H_ */
