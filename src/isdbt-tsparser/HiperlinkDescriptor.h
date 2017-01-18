/*
 * HiperlinkDescriptor.h
 *
 *  Created on: Nov 10, 2009
 *      Author: user
 */

#ifndef HIPERLINKDESCRIPTOR_H_
#define HIPERLINKDESCRIPTOR_H_

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_BEGIN

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


BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_END
#endif /* HIPERLINKDESCRIPTOR_H_ */
