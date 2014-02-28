/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include <config.h>
#include <dirent.h>

#ifndef WIN32
# define IS_DIRECTORY(st_mode)  (((st_mode) & S_IFMT) == S_IFDIR)
# define IS_LINK(st_mode)	(((st_mode) & S_IFMT) == S_IFLNK)
# define IS_REGULAR(st_mode)	(((st_mode) & S_IFMT) == S_IFREG)
# ifdef __APPLE__
#  include <sys/dir.h>
   typedef struct direct DIRENT;
# else
#  include <dirent.h>
   typedef struct dirent DIRENT;
# endif
#else  // WIN32
# define IS_DIRECTORY(st_mode)	(((st_mode) & S_IFMT) == S_IFDIR)
# define IS_LINK(st_mode)	0
# define IS_REGULAR(st_mode)	(((st_mode) & S_IFMT) == S_IFREG)
# define lstat stat
# include <windows.h>
#endif

#include "util/functions.h"

#if WIN32
# include "atlimage.h"
#else
# if HAVE_JPEG
#  include "jpeglib.h"
# endif
#endif

TELEMIDIA_UTIL_BEGIN_DECLS

	string userCurrentPath;
	static double timeStamp;

	string ultostr(unsigned long int value) {
		string strValue;

		char dst[32];
		char digits[32];
		unsigned long int i = 0, j = 0, n = 0;

		do {
			n = value % 10;
			digits[i++] = (n < 10 ? (char)n+'0' : (char)n-10+'a');
			value /= 10;

			if (i > 31) {
				break;
			}

		} while (value != 0);

		n = i;
		i--;

		while (i >= 0 && j < 32) {
			dst[j] = digits[i];
			i--;
			j++;
		}

		strValue.assign(dst, n);

		return strValue;
	}

	double strUTCToSec(string utcValue) {
		double secs = 0;
		vector<string>* params;

		params = split(utcValue, ":");
		switch (params->size()) {
			case 1:
				if (utcValue.find("s") != std::string::npos) {
					utcValue = utcValue.substr(0, utcValue.length() - 1);
				}

				secs = util::stof(utcValue);
				break;

			case 2:
				secs = 60 * util::stof((*params)[0]) + util::stof((*params)[1]);
				break;

			case 3:
				secs = 3600 * util::stof((*params)[0]) + 60 * util::stof((*params)[1]) +
						util::stof((*params)[2]);
				break;
		}

		delete params;
		params = NULL;

		return secs;
	}

	string cvtPercentual(string value, bool* isPercentual) {
		string newValue;

		newValue      = value;
		*isPercentual = false;

		if (value.find("%") != std::string::npos) {
			*isPercentual = true;
			newValue      = value.substr(0, value.length() - 1);
		}

		return newValue;
	}

	void initTimeStamp() {
		timeStamp = getCurrentTimeMillis();
	}

	void printTimeStamp() {
		double printTS = getCurrentTimeMillis() - timeStamp;
		clog << "TimeStamp: " << (printTS / 1000) << endl;
	}

	int strHexaToInt(string value) {
		std::stringstream str1(value);
		int number;

		if (value == "") {
			number = -1;

		} else if (value.find("0x") != string::npos) {
			if (!(str1 >> std::hex >> number)) {
				number = -1;
			}

		} else {
			if (!(str1 >> number)) {
				number = -1;
			}
		}
		return number;
	}

	string intToStrHexa(int value) {
		stringstream str;
		str << std::hex << std::uppercase << value;
		return "0x" + str.str();
	}

	string lowerCase(string s) {
		for (unsigned int j=0; j<s.length(); ++j) {
			s[j]=tolower(s[j]);
		}
		return s;
	}

	string upperCase(string s) {
		if (s == "") {
			return "";
		}

		for (unsigned int j=0; j<s.length(); ++j) {
			s[j]=toupper(s[j]);
		}
		return s;
	}

	bool isNumeric(void* value) {
		bool r = true;
		char* str = (char*)value;

		for (unsigned int i = 0; i < strlen(str); i++) {
			if (i == 0)
				if ((str[i] < '0' || str[i] > '9') &&
					    (str[i] != '.') &&
					    (str[i] != '+' || str[i] != '-'))
					r = false;

			if ((str[i] < '0' || str[i] > '9') && (str[i] != '.'))
				r = false;
		}

		return r;
	}

	string itos(double i) {
		ostringstream os;
		os << i;
		return os.str();
	}

	float stof(string s) {
		float f = 0.0f;
		stringstream ss;
		ss << s;
		ss >> f;
		return f;
	}

	float itof(int i) {
		string s = itos(i);
		return util::stof(s);
	}

	string getUserCurrentPath() {
		return userCurrentPath;
	}

	vector<string>* localSplit(string str, string delimiter) {
		vector<string>* splited;
		string::size_type lastPos, curPos;

		splited = new vector<string>;

		if (str == "") {
			return splited;
		}

		if (str.find_first_of(delimiter) == std::string::npos) {
			splited->push_back(str);
			return splited;
		}

		lastPos = str.find_first_not_of(delimiter, 0);

		if (lastPos != string::npos) {
			curPos = str.find_first_of(delimiter, lastPos);
			while (string::npos != curPos || string::npos != lastPos) {
				splited->push_back(str.substr(lastPos, curPos - lastPos));
				lastPos = str.find_first_not_of(delimiter, curPos);
				if (lastPos == string::npos) {
					break;
				}
				curPos = str.find_first_of(delimiter, lastPos);
			}
		}
		return splited;
	}

	vector<string>* split(string str, string delimiter, string pos_delimiter) {
		vector<string>* splited;
		splited = new vector<string>;
		string::size_type pos = 0;
		string::size_type lastPos = 0;

		if (pos_delimiter == "") {
			return localSplit(str, delimiter);
		}

		while (string::npos != lastPos) {
			if (str.find_first_of(pos_delimiter, pos) == pos) {
				lastPos = str.find_last_of(pos_delimiter);
				if (string::npos != lastPos) {
					splited->push_back(str.substr(pos + 1, lastPos - pos - 1));
					lastPos = str.find_first_of(delimiter, lastPos);
					if (string::npos == lastPos)
						pos = lastPos;
					else
						pos = lastPos + 1;

				}
				else {
					return splited;

				}

		    }
		    else {
		    	lastPos = str.find_first_of(delimiter, pos);
		    	if (string::npos != lastPos) {
		    		splited->push_back(str.substr(pos, lastPos - pos));
		    		pos = lastPos+1;
		    	}

		    }

		}

		if (string::npos != pos)
			splited->push_back(str.substr(pos));

		return splited;

	}

	string trim(string str) {
		string strR = "";

		string::size_type lastPos = str.find_first_not_of(" ", 0);
		string::size_type pos = str.find_first_of(" ", lastPos);

		while (string::npos != pos || string::npos != lastPos) {
		    strR = strR + str.substr(lastPos, pos - lastPos);
		    lastPos = str.find_first_not_of(" ", pos);
		    pos = str.find_first_of(" ", lastPos);
		}
		return strR;
	}

	float getPercentualValue(string value) {
		string actualValue;
		float floatValue;

		//retirar o caracter percentual da string
		actualValue = value.substr(0, (value.length() - 1));
		//converter para float
		floatValue = util::stof(actualValue);

		//se menor que zero, retornar zero
		if (floatValue < 0)
			floatValue = 0;
		//else if (floatValue > 100)
			//se maior que 100, retornar 100
			//floatValue = 100;

		//retornar valor percentual
		return floatValue;
	}

	/**
	 * Testa se uma string indica um valor percentual
	 * @param value string com um valor
	 * @return true se o valor e' percentual; false caso contrario.
	 */
	bool isPercentualValue(string value) {
		if (value.find_last_of("%") == (value.length() - 1))
			return true;
		else
			return false;
	}

	bool fileExists(string fileName) {
		string currentPath;
		char path[PATH_MAX] = "";

		if (fileName == "") {
			clog << "fileExists is false: null entry" << endl;
			return false;
		}

		try {
			if (fileName.find("<") != std::string::npos) {
				return true;
			}

//TODO: This must be moved to SystemCompat?
#if defined(_WIN32) && !defined(__MINGW32__)
			if (_access(fileName.c_str(), 0) == 0) {
				return true;
			}
#else
			if (access(fileName.c_str(), (int)F_OK) == 0) {
				return true;
			}
#endif

			clog << "fileExists is false: can't access '";
			clog << fileName << "'" << endl;
			return false;

		} catch (...) {
			clog << "util::functions fileExits exception" << endl;
		}

		clog << "fileExists is false: exception for '";
		clog << fileName << "'" << endl;
		return false;
	}

	bool isDirectory(const char* path) {
		struct stat f_stat;

		stat(path, &f_stat);

		/* return IS_DIRECTORY(f_stat.st_mode); */

		if (f_stat.st_mode & S_IFREG) {
			return false;
		}

		if (f_stat.st_mode & S_IFDIR) {
			return true;
		}

		return false;
	}

	static numeric_limits<double> double_info;
	static double notANumber     = double_info.quiet_NaN();
	static double doubleInfinity = double_info.infinity();

	double NaN() {
#ifndef _WIN32
		return notANumber;
#else
		return double_info.quiet_NaN();
#endif
	}

	double infinity() {
#ifndef _WIN32
		return doubleInfinity;
#else
		return double_info.infinity();
#endif
	}

	bool isNaN(double value) {
		string sval;
		sval = itos(value);
		/*clog << "functions::isNaN val = '" << value << "' ";
		clog << "sval = '" << sval << "'";*/
		if (isNumeric((void*)(sval.c_str()))) {
			return false;
		}

#ifdef WIN32
		if (upperCase(sval) == "NAN" || _isnan(value)) {
#else
		if (upperCase(sval) == "NAN" ) {
#endif
			return true;
		}

		/*clog << "isNaN Warning! Value = '" << value << "', ";
		clog << "sval = " << sval << endl;*/
		return false;
	}

	bool isInfinity(double value) {
		string sval;
		sval = itos(value);
		if (isNumeric((void*)(sval.c_str()))) {
			return false;
		}

#ifndef WIN32
		if (upperCase(sval).find("INF") != std::string::npos) {
#else
		if (upperCase(sval).find("INF") != std::string::npos ||
				!_finite(value)) {
#endif
			return true;
		}

		/*clog << "isInfinity Warning! Value = '" << value << "', ";
		clog << "sval = " << sval << endl;*/
		return false;
	}
/*
	int timevalSubtract(
			struct timeval *result, struct timeval *x, struct timeval *y) {

		int nsec;

		if (x->tv_usec < y->tv_usec) {
			nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
			y->tv_usec -= 1000000 * nsec;
			y->tv_sec  += nsec;
		}

		if (x->tv_usec - y->tv_usec > 1000000) {
			nsec = (x->tv_usec - y->tv_usec) / 1000000;
			y->tv_usec += 1000000 * nsec;
			y->tv_sec  -= nsec;
		}

		result->tv_sec  = x->tv_sec - y->tv_sec;
		result->tv_usec = x->tv_usec - y->tv_usec;

		return x->tv_sec < y->tv_sec;
	}

	static struct timeval startTimeMills;
	static bool firstCallTimeMills = true;

	double getCurrentTimeMillis() {
		struct timeval result;
		struct rusage usage;

		if (getrusage(RUSAGE_SELF, &usage) != 0) {
			clog << "getCurrentTimeMillis Warning!";
			clog << " getrusage error" << endl;
			return -1;
		}

		if (firstCallTimeMills) {
			firstCallTimeMills     = false;
			startTimeMills.tv_sec  = usage.ru_utime.tv_sec;
			startTimeMills.tv_usec = usage.ru_utime.tv_usec;
			return 1;
		}

		timevalSubtract(&result, &(usage.ru_utime), &startTimeMills);

		return (double) ((result.tv_sec * 1000) + (result.tv_usec / 1000));
	}
*/

	static double startTimeMills;
	static bool firstCallTimeMills = true;

	double getCurrentTimeMillis() {
		struct timeb t;

		ftime(&t);

		if(firstCallTimeMills) {
			firstCallTimeMills = false;
			startTimeMills = (double)t.time*1000 + (double)t.millitm - 1;
			return 1;
		}

		return (double)t.time*1000 + (double)t.millitm - startTimeMills;
	}

	//factor is not in use. It will be removed.
	double getNextStepValue(
			double initValue,
			double target,
			int factor,
			double time, double initTime, double dur, int durStep) {

		int numSteps;
		double stepSize, nextStepValue;

		//rg
		if (durStep <= 0) { //durStep is negative or not defined
			nextStepValue = initValue + (
					(double)(target-initValue)/dur) * (time-initTime);

		} else if (dur <= 0) {
			return target;

		} else {
			numSteps = dur/(durStep*1000);
			stepSize = (target-initValue)/numSteps;

			//clog << floor((time-initTime)/(durStep*1000)) << endl;
			//clog << stepSize << endl << endl;
			nextStepValue = initValue + (int) (
					(int)((time-initTime)/(durStep*1000))*stepSize);
		}

		return nextStepValue;
	}

	bool readPPMFile(char *fn, int &X, int &Y, unsigned char* &result){
		int i,j,tmpint;

		FILE *in_file = fopen(fn,"r");


		if(in_file == NULL) {
			clog << "readPPMFile: Can't open input file: " << string(fn) << endl;
			return false;
		}

		fscanf(in_file,"P%d\n", &tmpint);

		/* check for file header (ASCII F6 ) */
		if(tmpint != 6) {
			clog << "readPPMFile: Input file is not binary ppm. "<<endl;
			fclose(in_file);
			return false;
		}

		/* ignoring PPM comments */
		char ch;
		fscanf(in_file, "%c", &ch);
		while(ch == '#'){
			while(ch != '\n')
				fscanf(in_file,"%c", &ch);
			fscanf(in_file, "%c", &ch);
		}

		/* reading image size */
		fseek (in_file, -1, 1);
		fscanf(in_file,"%d %d\n%d\n", &X, &Y, &tmpint);

		if(tmpint != 255) clog << "readPPMFile: Warning: maxvalue is not 255 in ppm file"<<endl;

		/* memory allocation for PPM RGB pixel data */

		result = (unsigned char *) malloc(3*X*Y*sizeof(unsigned char));

		if(result == NULL){
			clog << "readPPMFile: Can't allocate memory buffer for PPM image "<<endl;
			fclose(in_file);
			return false;
		} /*else {
			printf("Reading image %s of size %dx%d\n",fn,X,Y);
		}*/

		/* read pixel data from file */
		if (fread(result, 3*X,Y,in_file) != Y) {
			clog << "readPPMFile: error reading ppm file"<<endl;
			fclose(in_file);
			return false;
		}

		fclose(in_file);

		return true;

}

	bool readBMPFile(char *fn, int &X, int &Y, unsigned char* &result) {
		FILE* f = fopen(fn, "rb");

		unsigned char* temp;

		if(f == NULL) {
			clog << "readBMPFile: Can't open input file: " << string(fn) << endl;
			return false;
		}

		unsigned char info[54];

		fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

		X = *(int*)&info[18];
		Y = *(int*)&info[22];

		int size = 3 * X * Y;

		//result = (unsigned char *) malloc(size*sizeof(unsigned char));
		temp = (unsigned char *) malloc(size*sizeof(unsigned char));

		//unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel

		fread(temp, sizeof(unsigned char), size, f); // read the rest of the data at once

		fclose(f);

        int i = 0;
/*
		for(i = 0; i < size; i += 3)
		{
			unsigned char tmp = temp[i];
			temp[i] = temp[i+2];
			temp[i+2] = tmp;
		}
*/
		result = (unsigned char *) malloc(size*sizeof(unsigned char));

        int j = 0;
        for (i = size-1; i >= 0; i--) {
   			result[j++] = temp [i];
		}
		return true;
	}

	bool ppmToJpeg(char *ppmfile, char *jpegfile, int quality) {
#if WIN32
		CImage myImage;

		myImage.Load(ppmfile);
		myImage.Save(jpegfile);

		return true;
#else
#if HAVE_JPEG
		struct jpeg_compress_struct cinfo;
		unsigned char *raw_image = NULL;
		int _w;
		int _h;
		struct jpeg_error_mgr jerr;

		/* this is a pointer to one row of image data */
		JSAMPROW row_pointer[1];

		if (!readPPMFile(ppmfile,_w,_h,raw_image)) {
			clog << "ppmToJPEG: error reading PPM File" << string(ppmfile) << endl;
			return false;
		}

		FILE *outfile = fopen( jpegfile, "wb" );

		if ( !outfile ) {
			clog << "Error opening output jpeg file " << string(jpegfile) << endl;
			//printf("Error opening output jpeg file %s\n!", filename );
			fclose(outfile);
			return false;
		}

		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, outfile);

		/* Setting the parameters of the output file here */
		cinfo.image_width = _w;
		cinfo.image_height = _h;
		cinfo.input_components = 3; //bytes per pixel
		cinfo.in_color_space = JCS_RGB;
		/* default compression parameters, we shouldn't be worried about these */

		cinfo.err = jpeg_std_error(&jerr);
//		jpeg_set_defaults(&cinfo);

		jpeg_set_defaults( &cinfo );
		cinfo.num_components = 3;
		//cinfo.data_precision = 4;
		cinfo.dct_method = JDCT_FLOAT;

		jpeg_set_quality(&cinfo, quality, TRUE);

		jpeg_start_compress( &cinfo, TRUE );

		while( cinfo.next_scanline < cinfo.image_height ) {
			row_pointer[0] = &raw_image[ cinfo.next_scanline * cinfo.image_width * cinfo.input_components];
			jpeg_write_scanlines( &cinfo, row_pointer, 1 );
		}

		jpeg_finish_compress( &cinfo );
		jpeg_destroy_compress( &cinfo );
		fclose( outfile );

		//free(row_pointer[0]);
		//free(raw_image);

		return true;
#endif
#endif
		return false;
		}

	bool bmpToJpeg(char *bmpfile, char *jpegfile, int quality) {
#if WIN32
        //bmpFile points to a char array with the path to the BMP file
		CImage myImage;

		myImage.Load(bmpfile);
		myImage.Save(jpegfile);

		return true;
#else
#if HAVE_JPEG
        //bmpFile points to the array of bytes from the BMP file
		struct jpeg_compress_struct cinfo;
		unsigned char *raw_image = NULL;
		int _w;
		int _h;
		struct jpeg_error_mgr jerr;

		/* this is a pointer to one row of image data */
		JSAMPROW row_pointer[1];

		if (!readBMPFile(bmpfile,_w,_h,raw_image)) {
			clog << "ppmToJPEG: error reading BMP File" << string(bmpfile) << endl;
			return false;
		}

		FILE *outfile = fopen( jpegfile, "wb" );

		if ( !outfile ) {
			clog << "Error opening output jpeg file " << string(jpegfile) << endl;
			//printf("Error opening output jpeg file %s\n!", filename );
			fclose(outfile);
			return false;
		}

		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, outfile);

		/* Setting the parameters of the output file here */
		cinfo.image_width = _w;
		cinfo.image_height = _h;
		cinfo.input_components = 3; //bytes per pixel
		cinfo.in_color_space = JCS_RGB;
		/* default compression parameters, we shouldn't be worried about these */

		cinfo.err = jpeg_std_error(&jerr);
//		jpeg_set_defaults(&cinfo);

		jpeg_set_defaults( &cinfo );
		cinfo.num_components = 3;
		//cinfo.data_precision = 4;
		cinfo.dct_method = JDCT_FLOAT;

		jpeg_set_quality(&cinfo, quality, TRUE);

		jpeg_start_compress( &cinfo, TRUE );

		while( cinfo.next_scanline < cinfo.image_height ) {
			row_pointer[0] = &raw_image[ cinfo.next_scanline * cinfo.image_width * cinfo.input_components];
			jpeg_write_scanlines( &cinfo, row_pointer, 1 );
		}

		jpeg_finish_compress( &cinfo );
		jpeg_destroy_compress( &cinfo );
		fclose( outfile );

		//free(row_pointer[0]);
		//free(raw_image);

		return true;
#endif
#endif
		return false;
	}


	// Replaces ALL occurences in <str> of the string <find_what> with the
	// string <replace_with>
	void str_replace_all(string &str, const string &find_what, const string &replace_with)
	{
		string::size_type pos=0;
		while((pos=str.find(find_what, pos))!=string::npos) {
			str.erase(pos, find_what.length());
			str.insert(pos, replace_with);
			pos+=replace_with.length();
		}
	}

TELEMIDIA_UTIL_END_DECLS
