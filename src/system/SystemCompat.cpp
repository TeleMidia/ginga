/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "SystemCompat.h"
#if HAVE_MULTIDEVICE
# include <zip.h>
#endif
extern "C" float machInfo(const char *name);

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace compat {

#if HAVE_MULTIDEVICE
	bool getZipError(zip* file, string* strError) {
		bool hasError = false;
		int zipErr, sysErr;
		char buff[2048];

		zip_error_get(file, &zipErr, &sysErr);
		if (zipErr != 0) {
			zip_error_to_str(buff, 2048, zipErr, sysErr);
			strError->assign(buff, strlen(buff));
			hasError = true;
		}

		return hasError;
	}

	static void printZipError(string function, string strError) {
		clog << function << " Warning! libzip error: '";
		clog << strError << "'" << endl;
	}

	int zipwalker(void* zipfile, string initdir, string dirpath, string iUriD) {
		DIR           *d;
		struct dirent *dir;
		FILE *fp;
		string relpath;
		string fullpath;
		struct zip* file;
		int len_dirpath;
		int len_initdir;
		int ret = 0;
		string strDirName;
		string strError;
		bool hasError;

		d = opendir(initdir.c_str());
		if (d == NULL) {
			return -1;
		}

		file = (struct zip*)zipfile;
		len_dirpath = dirpath.length();
		len_initdir = initdir.length();

		while ((dir = readdir(d))) {

			if (strcmp(dir->d_name, ".") == 0 ||
					strcmp(dir->d_name, "..") == 0) {

				continue;
			}

			strDirName.assign(dir->d_name, strlen(dir->d_name));
			fullpath = initdir + iUriD + strDirName;
			if (fullpath.length() > len_dirpath) {
				// Uses "/" as separator because is the default zip funcion separator.
				relpath = SystemCompat::updatePath(fullpath.substr(len_dirpath + 1), "/");
			} else {
				continue;
			}

			if (isDirectory(fullpath.c_str())) {
				// \fixme We should not use that!
				chdir(fullpath.c_str());

				clog << "Directory ( " << relpath << " ) " << endl;
				if (zip_dir_add(file, relpath.c_str(), ZIP_FL_ENC_GUESS) < 0) {
					getZipError(file, &strError);
					printZipError("zipwalker", strError);
					ret = -1;
					break;
				}

				if (zipwalker(file, fullpath, dirpath, iUriD) < 0) {
					getZipError(file, &strError);
					printZipError("zipwalker", strError);
					ret = -1;
					break;
				}

				// \fixme We should not use that!
				clog << "Returning to dir '" << initdir << "'" << endl;
				chdir(initdir.c_str());

			} else {

				clog << ":: full uri: " << fullpath << endl;
				clog << ":: init dir: " << initdir << endl;
				clog << ":: file name: " << string(dir->d_name) << endl;
				clog << ":: relpath ( " << relpath << " ) " << endl;

				fp = fopen(fullpath.c_str(), "rb");
				if (fp == NULL) {
					clog << ":: can't open "<< string(relpath) << endl;

				} else {
					struct zip_source *s;

					s = zip_source_filep(file, fp, 0, -1);
					if (s == NULL) {
						clog << ":: error [" << string(relpath) << "]: " << string(zip_strerror(file)) << endl;
						ret = -1;
						break;
					}

					if (zip_add(file, relpath.c_str(), s) == -1) {
						zip_source_free(s);
						clog << ":: error [" << string(relpath) << "]: " << string(zip_strerror(file)) << endl;
						ret = -1;
						break;
					}
				}
			}
		}

		clog << "zipwalker closing dir" << endl;
		closedir(d);
		clog << "zipwalker all done!" << endl;
		return ret;
	}
#endif

	string SystemCompat::filesPref        = "";
	string SystemCompat::ctxFilesPref     = "";
	string SystemCompat::installPref      = "";
	string SystemCompat::userCurrentPath  = "";
	string SystemCompat::gingaCurrentPath = "";
	string SystemCompat::pathD            = "";
	string SystemCompat::iUriD            = "";
	string SystemCompat::fUriD            = "";
	string SystemCompat::gingaPrefix      = "";
	bool SystemCompat::initialized        = false;
	void* SystemCompat::cmInstance        = NULL;

	void SystemCompat::checkValues() {
		if (!initialized) {
			initialized = true;
			initializeGingaPath();
			initializeGingaPrefix();
			initializeUserCurrentPath();
			initializeGingaConfigFile();
		}
	}

	void SystemCompat::initializeGingaPrefix() {
		SystemCompat::gingaPrefix = STR_PREFIX;

		if (gingaPrefix == "NONE") {
			gingaPrefix =  iUriD + "usr" + iUriD + "local" + iUriD;
		}
	}

	void SystemCompat::initializeGingaConfigFile() {
		ifstream fis;
		string line, key, partial, value;

#if defined(_WIN32) && !defined(__MINGW32__)
		filesPref = gingaCurrentPath + "files\\";
		installPref = gingaCurrentPath;

		pathD = ";";
		iUriD = "\\";
		fUriD = "/";

		return;
#else
		string gingaini = GINGA_INI_PATH;

		fis.open(gingaini.c_str(), ifstream::in);

		if (!fis.is_open()) {
			clog << "SystemCompat::initializeGingaConfigFile ";
			clog << "Warning! Can't open input file: '" << gingaini;
			clog << "' loading default configuration!" << endl;

			gingaCurrentPath = gingaPrefix + iUriD + "bin" + iUriD;
			installPref      = gingaPrefix;
			filesPref        = gingaPrefix + iUriD + "etc" +
					 iUriD + "ginga" + iUriD + "files" + iUriD;

			return;
		}

		value = "";

		while (fis.good()) {
			getline(fis, line);
			if (line.find("#") == std::string::npos &&
					(line.find("=") != std::string::npos || value != "")) {

				if (value == "") {
					key     = line.substr(0, line.find_last_of("="));
					partial = line.substr(
							(line.find_first_of("=") + 1),
							line.length() - (line.find_first_of("=") + 1));

				} else {
					partial = line;
				}

				value = value + partial;

				if (value.substr(value.length() - 1, 1) == "\"") {
					if (key == "system.internal.delimiter") {
						iUriD       = value.substr(1, value.length() - 2);

					} else if (key == "system.foreign.delimiter") {
						fUriD       = value.substr(1, value.length() - 2);

					} else if (key == "system.files.prefix") {
						filesPref   = value.substr(1, value.length() - 2);

					} else if (key == "system.install.prefix") {
						installPref = value.substr(1, value.length() - 2);
					}

					value = "";

				} else {
					value = value + " ";
				}
			}

			clog << "SystemCompat::initializeGingaConfigFile " << endl;
			clog << "line    = '" << line << "'" << endl;
			clog << "key     = '" << key << "'" << endl;
			clog << "partial = '" << partial << "'" << endl;
			clog << "value   = '" << value << "'" << endl;
		}

		fis.close();
#endif
	}

	void SystemCompat::initializeGingaPath() {
		string path, currentPath;
		string gingaBinary = "ginga";

#ifndef WIN32
		pathD            = ";";
		iUriD            = "/";
		fUriD            = "\\";
#else
		pathD            = ":";
		iUriD            = "\\";
		fUriD            = "/";
		gingaBinary = "ginga.exe";
#endif

		path = getenv("PATH");
		if (path.find(";") != std::string::npos) {
			pathD = ";";
			iUriD = "\\";
			fUriD = "/";

		} else if (path.find(":") != std::string::npos) {
			pathD = ":";
			iUriD = "/";
			fUriD = "\\";
		}

#if defined(_WIN32) && !defined(__MINGW32__)
		// Get the current executable path
		HMODULE hModule = GetModuleHandleW(NULL);
		WCHAR wexepath[300];
		GetModuleFileNameW(hModule, wexepath, 300);

		char DefChar = ' ';
		char exepath[300];
		WideCharToMultiByte(CP_ACP, 0, wexepath,-1, exepath, 260, &DefChar, NULL);

		currentPath = exepath;

		gingaCurrentPath = currentPath.substr( 0,
						   currentPath.find_last_of(iUriD)+1);

#else
		vector<string>* params;
		vector<string>::iterator i;

		params = split(path, pathD);
		if (params != NULL) {
			i = params->begin();
			while (i != params->end()) {
				currentPath = (*i) + iUriD + gingaBinary;

				if (access(currentPath.c_str(), (int)X_OK) == 0) {
					gingaCurrentPath = (*i);

					clog << "SystemCompat::initializeGingaPath found ";
					clog << "ginga binary file inside '" << gingaCurrentPath;
					clog << "'";
					clog << endl;
					if (gingaCurrentPath.find_last_of(iUriD) !=
							gingaCurrentPath.length() - 1) {

						gingaCurrentPath = gingaCurrentPath + iUriD;
					}

					break;
				}
				++i;
			}
			delete params;
		}
#endif //WIN32
	}

	void SystemCompat::initializeUserCurrentPath() {
		char path[PATH_MAX] = "";
		getcwd(path, PATH_MAX);

		userCurrentPath.assign(path, strlen(path));

		if (userCurrentPath.find_last_of(iUriD) !=
				userCurrentPath.length() - 1) {

			userCurrentPath = userCurrentPath + iUriD;
		}
	}

	void* SystemCompat::getComponentManagerInstance() {
		return cmInstance;
	}

	void SystemCompat::setComponentManagerInstance(void* cmInstance) {
		SystemCompat::cmInstance = cmInstance;
	}

	string SystemCompat::appendLibExt(string libName) {
		string newLibName = libName;

#ifdef WIN32
		if (libName.find(".dll") == std::string::npos) {
			newLibName = libName + ".dll";
		}
#else //!WIN32
		if (libName.find(".so") == std::string::npos) {
			newLibName = libName + ".so";
		}
#endif //WIN32

		return newLibName;
	}

	void* SystemCompat::loadComponent(
			string libName, void** llib, string symName) {
		abort ();
	}

	bool SystemCompat::releaseComponent(void* component) {
		abort ();
	}

	void SystemCompat::sigpipeHandler(int x) throw(const char*) {
#ifndef WIN32
		signal(SIGPIPE, sigpipeHandler);  //reset the signal handler
		clog << "SystemCompat::sigpipeHandler ";
		clog << "throw: " << strsignal(x) << endl;
		throw strsignal(x);  //throw the exception
#endif //!WIN32
	}

	string SystemCompat::getGingaPrefix() {
		return SystemCompat::gingaPrefix;
	}

	string SystemCompat::updatePath(string dir) {
		return updatePath(dir, iUriD);
	}

	int SystemCompat::zip_directory(
			const string &zipfile_path,
			const string &directory_path,
			const string &iUriD) {

#if HAVE_MULTIDEVICE
		struct zip* zipFile;
		int error_open;
		string dir_name;
		string partial_path;
		int pos;
		string strError;
		string functionStr;
		size_t strPos;

		clog << "functions::zip_directory " << endl;
		clog << "zipfile_path = " << zipfile_path << endl;
		clog << "directory_path = " << directory_path << endl;

		error_open = 0;

		if ((zipFile=zip_open(zipfile_path.c_str(), ZIP_CREATE, &error_open)) == NULL) {
			getZipError(zipFile, &strError);
			printZipError("zip_directory - zip_walker", strError);
			return 1;
		}

		if (zipFile != NULL && error_open == 0) {
			strPos = directory_path.find_last_of(iUriD);
			if (strPos == std::string::npos) {
				dir_name = directory_path;

			} else {
				dir_name = directory_path.substr(strPos + 1);
			}

			partial_path = directory_path;

			// This make no sense. I am always adding an empty directory!
			/* if (zip_dir_add(zipFile, dir_name.c_str(), ZIP_FL_ENC_GUESS) < 0) {
				getZipError(zipFile, &strError);
				printZipError("zip_directory - zip_dir_add ", strError);
				zip_discard(zipFile);
				return - 1;
			} */

			// \fixme This should not be recursive! So, there would not
			// be a possibility of stack overflow.
			if (zipwalker(zipFile, directory_path, partial_path, iUriD) < 0) {
				getZipError(zipFile, &strError);
				printZipError("zip_directory - zip_walker", strError);
				zip_discard(zipFile);
				return - 1;
			}

			/* if (getZipError(zipFile, &strError)) {
				printZipError("zip_directory - can't close zip file: ", strError);
				return -1;
			} */

			if (zip_close(zipFile) == -1) {
				clog << "functions::zip_directory Warning! can't close zip archive '";
				clog << zipfile_path << endl;
				return -1;
			}

		} else {
			clog << "functions::zip_directory Warning! Can't open '";
			clog << zipfile_path << "': error code = " << error_open;
			clog << endl;
			return -1;
		}

#endif
		clog << "functions::zip_directory all done" << endl;
		return 0;
	}


	int SystemCompat::unzip_file(const char *zipname, const char *filedir) {
#if HAVE_MULTIDEVICE
		struct zip *zipf;
		struct zip_file *inf;
		char cur_dir[2000];
		char buf[1024];
		int len = 0;
		int k;
		int errorp;
		FILE *ofp;
		int i;
		const char *cur_file_name;
		int name_len;

		//open our zip file
		zipf = zip_open(zipname, 0, &errorp);

		//skado if doesnt exist
		if (!zipf) {
			return 0;
		}

//		chdir(filedir);
		//save our current dir so we can return to it
		getcwd(cur_dir, 2000);

		//printf (":: cur dir: %s\n",cur_dir); //cout

		clog << ":: current dir: " << cur_dir << endl;

		//change to the dir we want to extract to
		chdir(filedir);
		clog << ":: extract to dir: " << filedir << endl;

		for(k = 0; (inf = zip_fopen_index(zipf, k, 0)); k++) {

			cur_file_name = zip_get_name(zipf, k, 0);

			if ((k == 0)&&(cur_file_name == NULL)) {
				continue;
			}

			name_len = strlen(cur_file_name);

			//open the file for writting
			char *filename = (char*)malloc((name_len+3)*sizeof(char));
			filename[0] = '.';
			//filename[1] = '/';
#ifdef WIN32
			filename[1] = '\\';

#else
			filename[1] = '/';
#endif
			filename[2] = '\0';

			strcat(filename, cur_file_name);
			if (cur_file_name[name_len-1] == '/') {
				//printf(":: creating dir: %s\n",filename);
				clog << ":: creating dir: " << filename << endl;

#ifdef WIN32
				_mkdir(filename);
#else
				mkdir(filename, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

			} else {
				//printf(":: inflating %s",filename);//cout
				clog << ":: inflating file " << filename << endl;

				ofp = fopen(filename, "wb");
				if (!ofp) {
					continue;
				}

				while((len = zip_fread(inf,buf,1024))) {
					fwrite(buf,sizeof(char),len,ofp);

					/* for (i=0; i < len; i++) {
						fprintf(ofp, "%c", buf[i]);
					} */

				}
//				printf(" [done] \n");
				//close the files
				zip_fclose(inf);
				fclose(ofp);
			}
			//printf(" [done]\n");//cout
			clog << " [done]" << endl;
			free(filename);
		}
		//go back to our original dir
		chdir(cur_dir);
#endif
		return 1;
	}

	string SystemCompat::updatePath(string dir, string separator) {
		bool found = false;
		string temp, newDir;
		vector<string>* params;
		vector<string>::iterator it;
		string::size_type pos;

		checkValues();

		if (dir.find("<") != std::string::npos ||
				checkUriPrefix(dir)) {

			return dir;
		}

		while (true) {
			pos = dir.find_first_of(fUriD);
			if (pos == std::string::npos) {
				break;
			}
			dir.replace(pos, 1, iUriD);
		}

		if (dir.find("..") == std::string::npos) {
			return dir;
		}

		params = split(dir, iUriD);
		newDir = "";
		it = params->begin();
		while (it != params->end()) {
			if ((it + 1) != params->end()) {
				temp = *(it + 1);
				if (temp != ".." || found) {
#ifdef WIN32
					if (newDir == "") {
						newDir = (*it); //Drive letter:

					} else {
						newDir = newDir + separator + (*it);
					}
#else
					newDir = newDir + separator + (*it);
#endif //WIN32

				} else {
					++it;
					found = true;
				}

			} else if ((*it) != ".") {
				newDir = newDir + separator + (*it);

			}
			++it;
		}
		delete params;

		if (found) {
			return SystemCompat::updatePath(newDir);

		} else {
			return newDir;
		}
	}

	bool SystemCompat::isXmlStr(string location) {
		if (location.find("<") != std::string::npos ||
				location.find("?xml") != std::string::npos ||
				location.find("|") != std::string::npos) {

			return true;
		}

		return false;
	}

	bool SystemCompat::checkUriPrefix(string uri) {
		string::size_type len;

		len = uri.length();
		if ((len >= 10 && uri.substr(0,10) == "x-sbtvdts:")        ||
				(len >= 9 && uri.substr(0,9) == "sbtvd-ts:")       ||
				(len >= 7 && uri.substr(0,7) == "http://")         ||
				(len >= 6 && uri.substr(0,6) == "ftp://")          ||
				(len >= 7 && uri.substr(0,7) == "file://")         ||
				(len >= 6 && uri.substr(0,6) == "tcp://")          ||
				(len >= 6 && uri.substr(0,6) == "udp://")          ||
				(len >= 6 && uri.substr(0,6) == "rtp://")          ||
				(len >= 13 && uri.substr(0,13) == "ncl-mirror://") ||
				(len >= 7 && uri.substr(0,7) == "rtsp://")) {

			return true;
		}

		return false;
	}

	bool SystemCompat::isAbsolutePath(string path) {
		string::size_type i, len;

		checkValues();

		if (isXmlStr(path)) {
			return true;
		}

		len = path.length();
		if (checkUriPrefix(path)) {
			return true;
		}

		i = path.find_first_of(fUriD);
		while (i != string::npos) {
			path.replace(i, 1, iUriD);
			i = path.find_first_of(fUriD);
		}

		if ((len >= 1 && path.substr(0,1) == iUriD) ||
				(len >= 2 && path.substr(1,2) == ":" + iUriD)) {

			return true;
		}

		return false;
	}

	string SystemCompat::getIUriD() {
		checkValues();
		return iUriD;
	}

	string SystemCompat::getFUriD() {
		checkValues();
		return fUriD;
	}

	string SystemCompat::getPath(string filename) {
		string path;
		string::size_type i;

		i = filename.find_last_of(iUriD);
		if (i != string::npos) {
			path = filename.substr(0, i);

		} else {
			path = "";
		}

		return path;
	}

	string SystemCompat::convertRelativePath(string relPath) {

		string _str;
		_str = relPath;

#ifdef WIN32
		_str.replace( relPath.begin(), relPath.end(), '/', '\\');
#endif
		return _str;
	}

	string SystemCompat::getGingaBinPath() {
		checkValues();

		return gingaCurrentPath;
	}

	string SystemCompat::getUserCurrentPath() {
		checkValues();

		return userCurrentPath;
	}

	void SystemCompat::setGingaContextPrefix(string newBaseDir) {
		ctxFilesPref = newBaseDir;
	}

	string SystemCompat::getGingaContextPrefix() {
		if (ctxFilesPref == "") {
			checkValues();
			ctxFilesPref = filesPref + iUriD + "contextmanager" + iUriD;
		}

		return updatePath(ctxFilesPref);
	}

	string SystemCompat::appendGingaFilesPrefix(string relUrl) {
		string absuri;

		checkValues();

		absuri = updatePath(filesPref + iUriD + relUrl);
		/*cout << "SystemCompat::appendGingaFilesPrefix to relUrl = '";
		cout << relUrl << "' filesPref = '" << filesPref << "' ";
		cout << " updated path = '" << absuri << "' ";
		cout << endl;*/

		return absuri;
	}

	string SystemCompat::appendGingaInstallPrefix(string relUrl) {
		string absuri;

		checkValues();

		absuri = updatePath(installPref + iUriD + relUrl);
		/*cout << "SystemCompat::appendGingaInstallPrefix to relUrl = '";
		cout << relUrl << "' installPref = '" << installPref << "' ";
		cout << " updated path = '" << absuri << "' ";
		cout << endl;*/

		return absuri;
	}

	void SystemCompat::initializeSigpipeHandler() {
#ifndef WIN32
		signal(SIGPIPE, sigpipeHandler);
#endif
	}

	string SystemCompat::getOperatingSystem() {

		string _ops;

#ifdef WIN32
		_ops = "Windows";
#else //!WIN32
		_ops = "Linux";
#endif //WIN32

		return _ops;
	}

	float SystemCompat::getClockSpeed() {
		float clockSpeed = 1000.0;

#ifdef HAVE_SYS_SYSINFO_H
		ifstream fis;
		string line = "";

		fis.open("/proc/cpuinfo", ifstream::in);

		if (!fis.is_open()) {
			clog << "SystemInfo::initializeClockSpeed Warning: can't open ";
			clog << "file '/proc/cpuinfo'" << endl;
			return clockSpeed;
		}

		while (fis.good()) {
			fis >> line;
			if (line == "cpu") {
				fis >> line;
				if (line == "MHz") {
					fis >> line;
					if (line == ":") {
						fis >> line;
						clockSpeed = util::stof(line);
						break;
					}
				}
			}
		}
#endif
		return clockSpeed;
	}

	float SystemCompat::getMemorySize() {

		float memSize = 0.0;

#ifdef WIN32
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	memSize = (float)ms.dwAvailPhys;
#elif HAVE_SYS_SYSINFO_H
		struct sysinfo info;
		sysinfo(&info);
		memSize = info.totalram;
#endif

		return (memSize);
	}

     void SystemCompat::strError (int err, char *buf, size_t size) {
		int saved_errno = errno;
#ifdef WIN32
		strerror_s (buf, size, err);
#else
		strerror_r (err, buf, size);
#endif
		errno = saved_errno;
	}


	int SystemCompat::changeDir (const char *path) {
#ifdef WIN32
			return _chdir (path);
#else
			return chdir (path);
#endif
	}

	void SystemCompat::makeDir(const char* dirName, unsigned int mode) {
#ifdef WIN32
			_mkdir(dirName);
#else
			mkdir(dirName, mode);
#endif
	}

	void SystemCompat::uSleep(unsigned int microseconds) {
#ifndef WIN32
		::usleep(microseconds);
#else
		Sleep(microseconds/1000);
#endif
	}

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
 #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
 #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

	int SystemCompat::gettimeofday(struct timeval *tv, struct timezone *tz) {
#if defined(_WIN32) && !defined(__MINGW32__)
		FILETIME ft;
		unsigned __int64 tmpres = 0;
		static int tzflag;
		if (NULL != tv) {
			GetSystemTimeAsFileTime(&ft);
			tmpres |= ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;
			/*converting file time to unix epoch*/
			tmpres /= 10;  /*convert into microseconds*/
			tmpres -= DELTA_EPOCH_IN_MICROSECS;
			tv->tv_sec = (long)(tmpres / 1000000UL);
			tv->tv_usec = (long)(tmpres % 1000000UL);
		}

		if (NULL != tz) {
			if (!tzflag) {
				_tzset();
				tzflag++;
			}

			tz->tz_minuteswest = _timezone / 60;
			tz->tz_dsttime = _daylight;
		}
		return 0;
#else
		return ::gettimeofday(tv, tz);
#endif
	}

	int SystemCompat::getUserClock(struct timeval* usrClk) {
		int rval = 0;

#if defined(_WIN32) && !defined(__MINGW32__)
		double temp;

		temp            = clock() / CLOCKS_PER_SEC;
		usrClk->tv_sec  = (long) temp;
		usrClk->tv_usec = 1000000*(temp - usrClk->tv_sec);
#else
		struct rusage usage;

		if (getrusage(RUSAGE_SELF, &usage) != 0) {
			clog << "SystemCompat::getUserClock getrusage error." << endl;
			rval = -1;

		} else {
			usrClk->tv_sec  = usage.ru_utime.tv_sec;
			usrClk->tv_usec = usage.ru_utime.tv_usec;
		}
#endif
		return rval;
	}

#if defined(_WIN32) && !defined(__MINGW32__)
	LARGE_INTEGER win_getFILETIMEoffset() {
		SYSTEMTIME s;
		FILETIME f;
		LARGE_INTEGER t;

		s.wYear = 1970;
		s.wMonth = 1;
		s.wDay = 1;
		s.wHour = 0;
		s.wMinute = 0;
		s.wSecond = 0;
		s.wMilliseconds = 0;
		SystemTimeToFileTime(&s, &f);
		t.QuadPart = f.dwHighDateTime;
		t.QuadPart <<= 32;
		t.QuadPart |= f.dwLowDateTime;
		return t;
	}

	int win_clock_gettime(int clockType, struct timespec* tv) {
		unsigned __int64 t;
		LARGE_INTEGER pf, pc;
		union {
			unsigned __int64 u64;
			FILETIME ft;
		} ct, et, kt, ut;

		switch (clockType) {
			case CLOCK_REALTIME:
				GetSystemTimeAsFileTime(&ct.ft);
				t = ct.u64 - DELTA_EPOCH_IN_100NS;
				tv->tv_sec = t / POW10_7;
				tv->tv_nsec = ((int) (t % POW10_7)) * 100;

				return 0;

			case CLOCK_MONOTONIC:
				if (QueryPerformanceFrequency(&pf) == 0) {
					return EINVAL;
				}

				if (QueryPerformanceCounter(&pc) == 0) {
					return EINVAL;
				}

				tv->tv_sec = pc.QuadPart / pf.QuadPart;
				tv->tv_nsec = (int) (((pc.QuadPart % pf.QuadPart) * POW10_9 + (pf.QuadPart >> 1)) / pf.QuadPart);
				if (tv->tv_nsec >= POW10_9) {
					tv->tv_sec ++;
					tv->tv_nsec -= POW10_9;
				}

				return 0;

			case CLOCK_PROCESS_CPUTIME_ID:
				if (GetProcessTimes(GetCurrentProcess(), &ct.ft, &et.ft, &kt.ft, &ut.ft) == 0) {
					return EINVAL;
				}

				t = kt.u64 + ut.u64;
				tv->tv_sec = t / POW10_7;
				tv->tv_nsec = ((int) (t % POW10_7)) * 100;

				return 0;

			case CLOCK_THREAD_CPUTIME_ID:
				if (GetThreadTimes(GetCurrentThread(), &ct.ft, &et.ft, &kt.ft, &ut.ft) == 0) {
					return EINVAL;
				}
				t = kt.u64 + ut.u64;
				tv->tv_sec = t / POW10_7;
				tv->tv_nsec = ((int) (t % POW10_7)) * 100;

				return 0;

			default:
				break;
		}

		return EINVAL;

		/*LARGE_INTEGER t;
		FILETIME f;
		double microseconds;
		static LARGE_INTEGER offset;
		static double frequencyToMicroseconds;
		static int initialized = 0;
		static BOOL usePerformanceCounter = 0;

		if (!initialized) {
			LARGE_INTEGER performanceFrequency;
			initialized = 1;
			usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
			if (usePerformanceCounter) {
				QueryPerformanceCounter(&offset);
				frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;

			} else {
				offset = win_getFILETIMEoffset();
				frequencyToMicroseconds = 10.;
			}
		}
		if (usePerformanceCounter) {
			QueryPerformanceCounter(&t);

		} else {
			GetSystemTimeAsFileTime(&f);
			t.QuadPart = f.dwHighDateTime;
			t.QuadPart <<= 32;
			t.QuadPart |= f.dwLowDateTime;
		}

		t.QuadPart  -= offset.QuadPart;
		microseconds = (double)t.QuadPart / frequencyToMicroseconds;
		t.QuadPart   = microseconds;
		tv->tv_sec   = t.QuadPart / 1000;
		tv->tv_nsec  = t.QuadPart % 1000;
		return 0;*/
	}
#endif

	int SystemCompat::clockGetTime(int clockType, struct timespec* tv) {
		int res;

#if defined(_WIN32) && !defined(__MINGW32__)
		res = win_clock_gettime(clockType, tv);
#else
		res = clock_gettime(clockType, tv);
#endif
		return res;
	}

	/* replacement of Unix rint() for Windows */
	int SystemCompat::rint (double x) {
#if defined(_WIN32) && !defined(__MINGW32__)
		char *buf;
		int i,dec,sig;

		buf = _fcvt(x, 0, &dec, &sig);
		i = atoi(buf);
		if(sig == 1) {
			i = i * -1;
		}

		return(i);
#else
		return ::rint(x);
#endif
	}

	string SystemCompat::getTemporaryDir() {
#if defined(_WIN32) && !defined(__MINGW32__)
		//TODO: Use the WIN32 API to return the temporary directory
		TCHAR lpTempPathBuffer[MAX_PATH];
		int dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
					   lpTempPathBuffer); // buffer for path
		if (dwRetVal > MAX_PATH || (dwRetVal == 0))
		{
			return gingaCurrentPath + "Temp\\";
		}
		return lpTempPathBuffer;
#else
		return "/tmp/";
#endif
	
	}

	static std::ofstream logOutput;

	void SystemCompat::setLogTo(short logType, string sufix) {
		string logUri = "";

		switch (logType) {
			case LOG_NULL:
				if (logOutput) {
					logOutput.close();
				}

				logOutput.open("/dev/null");
				if (logOutput) {
					clog.rdbuf(logOutput.rdbuf());
				}
				break;

			case LOG_STDO:
				if (logOutput) {
					logOutput.close();
				}

				logOutput.open("/dev/stdout");
				if (logOutput) {
					clog.rdbuf(logOutput.rdbuf());
				}
				break;

			case LOG_FILE:
				logUri = getTemporaryDir() + iUriD + "ginga";
				makeDir(logUri.c_str(), 0755);
				logUri = logUri + iUriD + "logFile" + sufix + ".txt";
				if (logOutput) {
					logOutput.close();
				}

				logOutput.open(logUri.c_str());
				if (logOutput) {
					clog.rdbuf(logOutput.rdbuf());
				}
				break;

			default:
				break;
		}
	}

	string SystemCompat::checkPipeName(string pipeName) {
		string newPipeName = pipeName;

		assert(pipeName != "");

#if defined(_WIN32) && !defined(__MINGW32__)
		if (pipeName.find("\\\\.\\pipe\\") == std::string::npos) {
			newPipeName = "\\\\.\\pipe\\" + pipeName;
		}
#else
		string tempDir = SystemCompat::getTemporaryDir();

		if (pipeName.length() < tempDir.length() ||
				pipeName.substr(0, tempDir.length()) != tempDir) {

			newPipeName = tempDir + pipeName;
		}
#endif

		return newPipeName;
	}

	void SystemCompat::checkPipeDescriptor(PipeDescriptor pd) {
#if defined(_WIN32) && !defined(__MINGW32__)
		assert(pd > 0);
#else
		assert(pd >= 0);
#endif
	}

	bool SystemCompat::createPipe(string pipeName, PipeDescriptor* pd) {
		pipeName = checkPipeName(pipeName);

#if defined(_WIN32) && !defined(__MINGW32__)
		*pd = CreateNamedPipe(
				pipeName.c_str(),
				PIPE_ACCESS_OUTBOUND, // 1-way pipe
				PIPE_TYPE_BYTE, // send data as a byte stream
				1, // only allow 1 instance of this pipe
				0, // no outbound buffer
				0, // no inbound buffer
				0, // use default wait time
				NULL); // use default security attributes);

		if (*pd == NULL || *pd == INVALID_HANDLE_VALUE) {
			clog << "SystemCompat::createPipe Warning! Failed to create '";
			clog << pipeName << "' pipe instance.";
			clog << endl;
			// TODO: look up error code: GetLastError()
			return false;
		}

		// This call blocks until a client process connects to the pipe
		BOOL result = ConnectNamedPipe(*pd, NULL);
		if (!result) {
			clog << "SystemCompat::createPipe Warning! Failed to make ";
			clog << "connection on " << pipeName << endl;
			// TODO: look up error code: GetLastError()
			CloseHandle(*pd); // close the pipe
			return false;
		}
#else
		mkfifo(pipeName.c_str(), 0666);

		*pd = open(pipeName.c_str(), O_WRONLY);
		if (*pd == -1) {
			clog << "SystemCompat::createPipe Warning! Failed to make ";
			clog << "connection on " << pipeName << endl;

			return false;
		}
#endif
		return true;
	}

	bool SystemCompat::openPipe(string pipeName, PipeDescriptor* pd) {
		pipeName = checkPipeName(pipeName);

#if defined(_WIN32) && !defined(__MINGW32__)
		*pd = CreateFile(
				pipeName.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

		if (*pd == INVALID_HANDLE_VALUE) {
			clog << "SystemCompat::openPipe Failed to open '";
			clog << pipeName << "'" << endl;
			// TODO: look up error code: GetLastError()
			return false;
		}
#else
		*pd = open(pipeName.c_str(), O_RDONLY);
		if (*pd < 0) {
			clog << "SystemCompat::openPipe Warning! ";
			clog << "Can't open '" << pipeName;
			clog << "'" << endl;
			perror("SystemCompat::openPipe can't open pipe");
			return false;
		}
#endif
		return true;
	}

	void SystemCompat::closePipe(PipeDescriptor pd) {
		checkPipeDescriptor(pd);

#if defined(_WIN32) && !defined(__MINGW32__)
		CloseHandle(pd);
#else
		close(pd);
#endif

		clog << "SystemCompat::closePipe '";
		clog << pd << "'" << endl;
	}

	int SystemCompat::readPipe(PipeDescriptor pd, char* buffer, int buffSize) {
		int bytesRead = 0;

		checkPipeDescriptor(pd);

#if defined(_WIN32) && !defined(__MINGW32__)
		DWORD bRead = 0;
		BOOL result = ReadFile(
				pd,
				buffer,
				buffSize,
				&bRead,
				NULL);

		bytesRead = (int)bRead;
#else
		bytesRead = read(pd, buffer, buffSize);
#endif

		return bytesRead;
	}

	int SystemCompat::writePipe(PipeDescriptor pd, char* data, int dataSize) {
		int bytesWritten = 0;

		assert(pd > 0);

#if defined(_WIN32) && !defined(__MINGW32__)
		// This call blocks until a client process reads all the data
		DWORD bWritten = 0;
		BOOL result = WriteFile(
				pd,
				data,
				dataSize,
				&bWritten,
				NULL); // not using overlapped IO

		if (!result) {
			clog << "SystemCompat::writePipe error: '";
			clog << GetLastError() << "' pd = " << pd << endl;
		}
		bytesWritten = (int)bWritten;

		assert(bytesWritten == dataSize);
#else
		bytesWritten = write(pd, (void*)data, dataSize);
#endif

		return bytesWritten;
	}

}
}
}
}
}
}
}
