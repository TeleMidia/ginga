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

#include "ginga.h"
#include "SystemCompat.h"
#if WITH_MULTIDEVICE
#include <zip.h>
#endif

GINGA_SYSTEM_BEGIN

#if WITH_MULTIDEVICE
static bool
getZipError (zip *file, string *strError)
{
  bool hasError = false;
  int zipErr, sysErr;
  char buff[2048];

  zip_error_get (file, &zipErr, &sysErr);
  if (zipErr != 0)
    {
      zip_error_to_str (buff, 2048, zipErr, sysErr);
      strError->assign (buff, strlen (buff));
      hasError = true;
    }

  return hasError;
}

static void
printZipError (const string &function, const string &strError)
{
  clog << function << " Warning! libzip error: '";
  clog << strError << "'" << endl;
}

static int
zipwalker (void *zipfile,
           const string &initdir,
           const string &dirpath,
           const string &iUriD)
{
  DIR *d;
  struct dirent *dir;
  FILE *fp;
  string relpath;
  string fullpath;
  struct zip *file;
  int len_dirpath;
  int ret = 0;
  string strDirName;
  string strError;

  d = opendir (initdir.c_str ());
  if (d == NULL)
    {
      return -1;
    }

  file = (struct zip *)zipfile;
  len_dirpath = (int) dirpath.length ();

  while ((dir = readdir (d)))
    {
      if (strcmp (dir->d_name, ".") == 0 || strcmp (dir->d_name, "..") == 0)
        {
          continue;
        }

      strDirName.assign (dir->d_name, strlen (dir->d_name));
      fullpath = initdir + iUriD + strDirName;
      if ((int) fullpath.length () > len_dirpath)
        {
          // Uses "/" as separator because is the default zip funcion
          // separator.
          relpath = SystemCompat::updatePath (
              fullpath.substr (len_dirpath + 1), "/");
        }
      else
        {
          continue;
        }

      if (isDirectory (fullpath.c_str ()))
        {
          // \fixme We should not use that!
          chdir (fullpath.c_str ());

          clog << "Directory ( " << relpath << " ) " << endl;
          if (zip_dir_add (file, relpath.c_str (), ZIP_FL_ENC_GUESS) < 0)
            {
              getZipError (file, &strError);
              printZipError ("zipwalker", strError);
              ret = -1;
              break;
            }

          if (zipwalker (file, fullpath, dirpath, iUriD) < 0)
            {
              getZipError (file, &strError);
              printZipError ("zipwalker", strError);
              ret = -1;
              break;
            }

          // \fixme We should not use that!
          clog << "Returning to dir '" << initdir << "'" << endl;
          chdir (initdir.c_str ());
        }
      else
        {
          clog << ":: full uri: " << fullpath << endl;
          clog << ":: init dir: " << initdir << endl;
          clog << ":: file name: " << string (dir->d_name) << endl;
          clog << ":: relpath ( " << relpath << " ) " << endl;

          fp = fopen (fullpath.c_str (), "rb");
          if (fp == NULL)
            {
              clog << ":: can't open " << string (relpath) << endl;
            }
          else
            {
              struct zip_source *s;

              s = zip_source_filep (file, fp, 0, -1);
              if (s == NULL)
                {
                  clog << ":: error [" << string (relpath)
                       << "]: " << string (zip_strerror (file)) << endl;
                  ret = -1;
                  break;
                }

              if (zip_add (file, relpath.c_str (), s) == -1)
                {
                  zip_source_free (s);
                  clog << ":: error [" << string (relpath)
                       << "]: " << string (zip_strerror (file)) << endl;
                  ret = -1;
                  break;
                }
            }
        }
    }

  clog << "zipwalker closing dir" << endl;
  closedir (d);
  clog << "zipwalker all done!" << endl;
  return ret;
}
#endif

string SystemCompat::filesPref = "";
string SystemCompat::ctxFilesPref = "";
string SystemCompat::installPref = "";
string SystemCompat::userCurrentPath = "";
string SystemCompat::gingaCurrentPath = "";
string SystemCompat::pathD = "";
string SystemCompat::iUriD = "";
string SystemCompat::fUriD = "";
string SystemCompat::gingaPrefix = "";
bool SystemCompat::initialized = false;
void *SystemCompat::cmInstance = NULL;

void
SystemCompat::checkValues ()
{
  if (!initialized)
    {
      initialized = true;
      initializeGingaPath ();
      initializeGingaPrefix ();
      initializeUserCurrentPath ();
      initializeGingaConfigFile ();
    }
}

void
SystemCompat::initializeGingaPrefix ()
{
  SystemCompat::gingaPrefix = "";
}

void
SystemCompat::initializeGingaConfigFile ()
{
  ifstream fis;
  string line, key, partial, value;

#if defined(_MSC_VER)
  filesPref = gingaCurrentPath + "files\\";
  installPref = gingaCurrentPath;

  pathD = ";";
  iUriD = "\\";
  fUriD = "/";

  return;
#else
  string gingaini = "";

  fis.open (gingaini.c_str (), ifstream::in);

  if (!fis.is_open ())
    {
      clog << "SystemCompat::initializeGingaConfigFile ";
      clog << "Warning! Can't open input file: '" << gingaini;
      clog << "' loading default configuration!" << endl;

      gingaCurrentPath = gingaPrefix + iUriD + "bin" + iUriD;
      installPref = gingaPrefix;
      filesPref = gingaPrefix + iUriD + "etc" + iUriD + "ginga" + iUriD
                  + "files" + iUriD;

      return;
    }

  value = "";

  while (fis.good ())
    {
      getline (fis, line);
      if (line.find ("#") == std::string::npos
          && (line.find ("=") != std::string::npos || value != ""))
        {
          if (value == "")
            {
              key = line.substr (0, line.find_last_of ("="));
              partial = line.substr ((line.find_first_of ("=") + 1),
                                     line.length ()
                                         - (line.find_first_of ("=") + 1));
            }
          else
            {
              partial = line;
            }

          value = value + partial;

          if (value.substr (value.length () - 1, 1) == "\"")
            {
              if (key == "system.internal.delimiter")
                {
                  iUriD = value.substr (1, value.length () - 2);
                }
              else if (key == "system.foreign.delimiter")
                {
                  fUriD = value.substr (1, value.length () - 2);
                }
              else if (key == "system.files.prefix")
                {
                  filesPref = value.substr (1, value.length () - 2);
                }
              else if (key == "system.install.prefix")
                {
                  installPref = value.substr (1, value.length () - 2);
                }

              value = "";
            }
          else
            {
              value = value + " ";
            }
        }

      clog << "SystemCompat::initializeGingaConfigFile " << endl;
      clog << "line    = '" << line << "'" << endl;
      clog << "key     = '" << key << "'" << endl;
      clog << "partial = '" << partial << "'" << endl;
      clog << "value   = '" << value << "'" << endl;
    }

  fis.close ();
#endif
}

void
SystemCompat::initializeGingaPath ()
{
  string path, currentPath;
  string gingaBinary = "ginga";

#ifndef _MSC_VER
  pathD = ";";
  iUriD = "/";
  fUriD = "\\";
#else
  pathD = ":";
  iUriD = "\\";
  fUriD = "/";
  gingaBinary = "ginga.exe";
#endif

  path = getenv ("PATH");
  if (path.find (";") != std::string::npos)
    {
      pathD = ";";
      iUriD = "\\";
      fUriD = "/";
    }
  else if (path.find (":") != std::string::npos)
    {
      pathD = ":";
      iUriD = "/";
      fUriD = "\\";
    }

#if defined(_MSC_VER)
  // Get the current executable path
  HMODULE hModule = GetModuleHandleW (NULL);
  WCHAR wexepath[300];
  GetModuleFileNameW (hModule, wexepath, 300);

  char DefChar = ' ';
  char exepath[300];
  WideCharToMultiByte (CP_ACP, 0, wexepath, -1, exepath, 260, &DefChar,
                       NULL);

  currentPath = exepath;

  gingaCurrentPath
      = currentPath.substr (0, currentPath.find_last_of (iUriD) + 1);

#else
  vector<string> *params;
  vector<string>::iterator i;

  params = split (path, pathD);
  if (params != NULL)
    {
      i = params->begin ();
      while (i != params->end ())
        {
          currentPath = (*i) + iUriD + gingaBinary;

          if (access (currentPath.c_str (), (int)X_OK) == 0)
            {
              gingaCurrentPath = (*i);

              clog << "SystemCompat::initializeGingaPath found ";
              clog << "ginga binary file inside '" << gingaCurrentPath;
              clog << "'";
              clog << endl;
              if (gingaCurrentPath.find_last_of (iUriD)
                  != gingaCurrentPath.length () - 1)
                {
                  gingaCurrentPath = gingaCurrentPath + iUriD;
                }

              break;
            }
          ++i;
        }
      delete params;
    }
#endif //_MSC_VER
}

void
SystemCompat::initializeUserCurrentPath ()
{
  char path[PATH_MAX] = "";
  getcwd (path, PATH_MAX);

  userCurrentPath.assign (path, strlen (path));

  if (userCurrentPath.find_last_of (iUriD) != userCurrentPath.length () - 1)
    {
      userCurrentPath = userCurrentPath + iUriD;
    }
}

string
SystemCompat::getGingaPrefix ()
{
  return SystemCompat::gingaPrefix;
}

string
SystemCompat::updatePath (const string &dir)
{
  return updatePath (dir, iUriD);
}

int
SystemCompat::zip_directory (const string &zipfile_path,
                             const string &directory_path,
                             const string &iUriD)
{
#if WITH_MULTIDEVICE
  struct zip *zipFile;
  int error_open;
  string dir_name;
  string partial_path;
  string strError;
  string functionStr;
  size_t strPos;

  clog << "functions::zip_directory " << endl;
  clog << "zipfile_path = " << zipfile_path << endl;
  clog << "directory_path = " << directory_path << endl;

  error_open = 0;

  if ((zipFile = zip_open (zipfile_path.c_str (), ZIP_CREATE, &error_open))
      == NULL)
    {
      getZipError (zipFile, &strError);
      printZipError ("zip_directory - zip_walker", strError);
      return 1;
    }

  if (zipFile != NULL && error_open == 0)
    {
      strPos = directory_path.find_last_of (iUriD);
      if (strPos == std::string::npos)
        {
          dir_name = directory_path;
        }
      else
        {
          dir_name = directory_path.substr (strPos + 1);
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
      if (zipwalker (zipFile, directory_path, partial_path, iUriD) < 0)
        {
          getZipError (zipFile, &strError);
          printZipError ("zip_directory - zip_walker", strError);
          zip_discard (zipFile);
          return -1;
        }

      /* if (getZipError(zipFile, &strError)) {
              printZipError("zip_directory - can't close zip file: ",
      strError);
              return -1;
      } */

      if (zip_close (zipFile) == -1)
        {
          clog << "functions::zip_directory Warning! can't close zip "
                  "archive '";
          clog << zipfile_path << endl;
          return -1;
        }
    }
  else
    {
      clog << "functions::zip_directory Warning! Can't open '";
      clog << zipfile_path << "': error code = " << error_open;
      clog << endl;
      return -1;
    }

#endif
  clog << "functions::zip_directory all done" << endl;
  return 0;
}

int
SystemCompat::unzip_file (const char *zipname, const char *filedir)
{
#if WITH_MULTIDEVICE
  struct zip *zipf;
  struct zip_file *inf;
  char cur_dir[2000];
  char buf[1024];
  int len = 0;
  int k;
  int errorp;
  FILE *ofp;
  const char *cur_file_name;
  int name_len;

  // open our zip file
  zipf = zip_open (zipname, 0, &errorp);

  // skado if doesnt exist
  if (!zipf)
    {
      return 0;
    }

  //		chdir(filedir);
  // save our current dir so we can return to it
  getcwd (cur_dir, 2000);

  // printf (":: cur dir: %s\n",cur_dir); //cout

  clog << ":: current dir: " << cur_dir << endl;

  // change to the dir we want to extract to
  chdir (filedir);
  clog << ":: extract to dir: " << filedir << endl;

  for (k = 0; (inf = zip_fopen_index (zipf, k, 0)); k++)
    {
      cur_file_name = zip_get_name (zipf, k, 0);

      if ((k == 0) && (cur_file_name == NULL))
        {
          continue;
        }

      name_len = (int) strlen (cur_file_name);

      // open the file for writting
      char *filename = (char *)malloc ((name_len + 3) * sizeof (char));
      filename[0] = '.';
// filename[1] = '/';
#ifdef _MSC_VER
      filename[1] = '\\';

#else
      filename[1] = '/';
#endif
      filename[2] = '\0';

      strcat (filename, cur_file_name);
      if (cur_file_name[name_len - 1] == '/')
        {
          // printf(":: creating dir: %s\n",filename);
          clog << ":: creating dir: " << filename << endl;

#ifdef _MSC_VER
          _mkdir (filename);
#else
          mkdir (filename, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
        }
      else
        {
          // printf(":: inflating %s",filename);//cout
          clog << ":: inflating file " << filename << endl;

          ofp = fopen (filename, "wb");
          if (!ofp)
            {
              continue;
            }

          while ((len = (int) zip_fread (inf, buf, 1024)))
            {
              fwrite (buf, sizeof (char), len, ofp);

              /* for (i=0; i < len; i++) {
                      fprintf(ofp, "%c", buf[i]);
              } */
            }
          //				printf(" [done] \n");
          // close the files
          zip_fclose (inf);
          fclose (ofp);
        }
      // printf(" [done]\n");//cout
      clog << " [done]" << endl;
      free (filename);
    }
  // go back to our original dir
  chdir (cur_dir);
#endif
  return 1;
}

string
SystemCompat::updatePath (const string &d, const string &separator)
{
  bool found = false;
  string dir = d;
  string temp, newDir;
  vector<string> *params;
  vector<string>::iterator it;
  string::size_type pos;

  checkValues ();

  if (dir.find ("<") != std::string::npos || checkUriPrefix (dir))
    {
      return dir;
    }

  while (true)
    {
      pos = dir.find_first_of (fUriD);
      if (pos == std::string::npos)
        {
          break;
        }
      dir.replace (pos, 1, iUriD);
    }

  if (dir.find ("..") == std::string::npos)
    {
      return dir;
    }

  params = split (dir, iUriD);
  newDir = "";
  it = params->begin ();
  while (it != params->end ())
    {
      if ((it + 1) != params->end ())
        {
          temp = *(it + 1);
          if (temp != ".." || found)
            {
#ifdef _MSC_VER
              if (newDir == "")
                {
                  newDir = (*it); // Drive letter:
                }
              else
                {
                  newDir = newDir + separator + (*it);
                }
#else
              newDir = newDir + separator + (*it);
#endif //_MSC_VER
            }
          else
            {
              ++it;
              found = true;
            }
        }
      else if ((*it) != ".")
        {
          newDir = newDir + separator + (*it);
        }
      ++it;
    }
  delete params;

  if (found)
    {
      return SystemCompat::updatePath (newDir);
    }
  else
    {
      return newDir;
    }
}

bool
SystemCompat::isXmlStr (const string &location)
{
  if (location.find ("<") != std::string::npos
      || location.find ("?xml") != std::string::npos
      || location.find ("|") != std::string::npos)
    {
      return true;
    }

  return false;
}

bool
SystemCompat::checkUriPrefix (const string &uri)
{
  string::size_type len;

  len = uri.length ();
  if ((len >= 10 && uri.substr (0, 10) == "x-sbtvdts:")
      || (len >= 9 && uri.substr (0, 9) == "sbtvd-ts:")
      || (len >= 7 && uri.substr (0, 7) == "http://")
      || (len >= 6 && uri.substr (0, 6) == "ftp://")
      || (len >= 7 && uri.substr (0, 7) == "file://")
      || (len >= 6 && uri.substr (0, 6) == "tcp://")
      || (len >= 6 && uri.substr (0, 6) == "udp://")
      || (len >= 6 && uri.substr (0, 6) == "rtp://")
      || (len >= 13 && uri.substr (0, 13) == "ncl-mirror://")
      || (len >= 7 && uri.substr (0, 7) == "rtsp://"))
    {
      return true;
    }

  return false;
}

bool
SystemCompat::isAbsolutePath (const string &p)
{
  string::size_type i, len;
  string path = p;

  checkValues ();

  if (isXmlStr (path))
    {
      return true;
    }

  len = path.length ();
  if (checkUriPrefix (path))
    {
      return true;
    }

  i = path.find_first_of (fUriD);
  while (i != string::npos)
    {
      path.replace (i, 1, iUriD);
      i = path.find_first_of (fUriD);
    }

  if ((len >= 1 && path.substr (0, 1) == iUriD)
      || (len >= 2 && path.substr (1, 2) == ":" + iUriD))
    {
      return true;
    }

  return false;
}

string
SystemCompat::getIUriD ()
{
  checkValues ();
  return iUriD;
}

string
SystemCompat::getFUriD ()
{
  checkValues ();
  return fUriD;
}

string
SystemCompat::getPath (const string &filename)
{
  string path;
  string::size_type i;

  i = filename.find_last_of (iUriD);
  if (i != string::npos)
    {
      path = filename.substr (0, i);
    }
  else
    {
      path = "";
    }

  return path;
}

string
SystemCompat::convertRelativePath (const string &relPath)
{
  string _str;
  _str = relPath;

#ifdef _MSC_VER
  _str.replace (relPath.begin (), relPath.end (), '/', '\\');
#endif
  return _str;
}

string
SystemCompat::getGingaBinPath ()
{
  checkValues ();

  return gingaCurrentPath;
}

string
SystemCompat::getUserCurrentPath ()
{
  checkValues ();

  return userCurrentPath;
}

void
SystemCompat::setGingaContextPrefix (const string &newBaseDir)
{
  ctxFilesPref = newBaseDir;
}

string
SystemCompat::getGingaContextPrefix ()
{
  if (ctxFilesPref == "")
    {
      checkValues ();
      ctxFilesPref = filesPref + iUriD + "contextmanager" + iUriD;
    }

  return updatePath (ctxFilesPref);
}

string
SystemCompat::appendGingaFilesPrefix (const string &relUrl)
{
  string absuri;

  checkValues ();
  absuri = updatePath (filesPref + iUriD + relUrl);
  return absuri;
}

string
SystemCompat::appendGingaInstallPrefix (const string &relUrl)
{
  string absuri;
  checkValues ();
  absuri = updatePath (installPref + iUriD + relUrl);
  return absuri;
}

int
SystemCompat::getUserClock (struct timeval *usrClk)
{
  int rval = 0;

#if defined(_MSC_VER)
  double temp;

  temp = clock () / CLOCKS_PER_SEC;
  usrClk->tv_sec = (long)temp;
  usrClk->tv_usec = 1000000 * (temp - usrClk->tv_sec);
#else
  struct rusage usage;

  if (getrusage (RUSAGE_SELF, &usage) != 0)
    {
      clog << "SystemCompat::getUserClock getrusage error." << endl;
      rval = -1;
    }
  else
    {
      usrClk->tv_sec = usage.ru_utime.tv_sec;
      usrClk->tv_usec = usage.ru_utime.tv_usec;
    }
#endif
  return rval;
}

static std::ofstream logOutput;

void
SystemCompat::setLogTo (short logType, const string &sufix)
{
  string logUri = "";

  switch (logType)
    {
    case LOG_NULL:
      if (logOutput)
        {
          logOutput.close ();
        }

      logOutput.open ("/dev/null");
      if (logOutput)
        {
          clog.rdbuf (logOutput.rdbuf ());
        }
      break;

    case LOG_STDO:
      if (logOutput)
        {
          logOutput.close ();
        }

      logOutput.open ("/dev/stdout");
      if (logOutput)
        {
          clog.rdbuf (logOutput.rdbuf ());
        }
      break;

    case LOG_FILE:
      logUri = string (g_get_tmp_dir ()) + "/" + iUriD + "ginga";
      g_mkdir (logUri.c_str (), 0755);
      logUri = logUri + iUriD + "logFile" + sufix + ".txt";
      if (logOutput)
        {
          logOutput.close ();
        }

      logOutput.open (logUri.c_str ());
      if (logOutput)
        {
          clog.rdbuf (logOutput.rdbuf ());
        }
      break;

    default:
      break;
    }
}

string
SystemCompat::checkPipeName (const string &pipeName)
{
  string newPipeName = pipeName;

  assert (pipeName != "");

#if defined(_MSC_VER)
  if (pipeName.find ("\\\\.\\pipe\\") == std::string::npos)
    {
      newPipeName = "\\\\.\\pipe\\" + pipeName;
    }
#else
  string tempDir = string (g_get_tmp_dir ()) + "/";

  if (pipeName.length () < tempDir.length ()
      || pipeName.substr (0, tempDir.length ()) != tempDir)
    {
      newPipeName = tempDir + pipeName;
    }
#endif

  return newPipeName;
}

void
SystemCompat::checkPipeDescriptor (PipeDescriptor pd)
{
#if defined(_MSC_VER)
  assert (pd > 0);
#else
  assert (pd >= 0);
#endif
}

bool
SystemCompat::createPipe (const string &pName, PipeDescriptor *pd)
{
  string pipeName = checkPipeName (pName);

#if defined(_MSC_VER)
  *pd = CreateNamedPipe (pipeName.c_str (),
                         PIPE_ACCESS_OUTBOUND, // 1-way pipe
                         PIPE_TYPE_BYTE,       // send data as a byte stream
                         1,     // only allow 1 instance of this pipe
                         0,     // no outbound buffer
                         0,     // no inbound buffer
                         0,     // use default wait time
                         NULL); // use default security attributes);

  if (*pd == NULL || *pd == INVALID_HANDLE_VALUE)
    {
      clog << "SystemCompat::createPipe Warning! Failed to create '";
      clog << pipeName << "' pipe instance.";
      clog << endl;
      // TODO: look up error code: GetLastError()
      return false;
    }

  // This call blocks until a client process connects to the pipe
  BOOL result = ConnectNamedPipe (*pd, NULL);
  if (!result)
    {
      clog << "SystemCompat::createPipe Warning! Failed to make ";
      clog << "connection on " << pipeName << endl;
      // TODO: look up error code: GetLastError()
      CloseHandle (*pd); // close the pipe
      return false;
    }
#else
  mkfifo (pipeName.c_str (), 0666);

  *pd = open (pipeName.c_str (), O_WRONLY);
  if (*pd == -1)
    {
      clog << "SystemCompat::createPipe Warning! Failed to make ";
      clog << "connection on " << pipeName << endl;

      return false;
    }
#endif
  return true;
}

bool
SystemCompat::openPipe (const string &pName, PipeDescriptor *pd)
{
  string pipeName = checkPipeName (pName);

#if defined(_MSC_VER)
  *pd = CreateFile (pipeName.c_str (), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, NULL);

  if (*pd == INVALID_HANDLE_VALUE)
    {
      clog << "SystemCompat::openPipe Failed to open '";
      clog << pipeName << "'" << endl;
      // TODO: look up error code: GetLastError()
      return false;
    }
#else
  *pd = open (pipeName.c_str (), O_RDONLY);
  if (*pd < 0)
    {
      clog << "SystemCompat::openPipe Warning! ";
      clog << "Can't open '" << pipeName;
      clog << "'" << endl;
      perror ("SystemCompat::openPipe can't open pipe");
      return false;
    }
#endif
  return true;
}

void
SystemCompat::closePipe (PipeDescriptor pd)
{
  checkPipeDescriptor (pd);

#if defined(_MSC_VER)
  CloseHandle (pd);
#else
  close (pd);
#endif

  clog << "SystemCompat::closePipe '";
  clog << pd << "'" << endl;
}

int
SystemCompat::readPipe (PipeDescriptor pd, char *buffer, int buffSize)
{
  int bytesRead = 0;

  checkPipeDescriptor (pd);

#if defined(_MSC_VER)
  DWORD bRead = 0;
  BOOL result = ReadFile (pd, buffer, buffSize, &bRead, NULL);

  bytesRead = (int)bRead;
#else
  bytesRead = (int) read (pd, buffer, buffSize);
#endif

  return bytesRead;
}

int
SystemCompat::writePipe (PipeDescriptor pd, char *data, int dataSize)
{
  int bytesWritten = 0;

  assert (pd > 0);

#if defined(_MSC_VER)
  // This call blocks until a client process reads all the data
  DWORD bWritten = 0;
  BOOL result = WriteFile (pd, data, dataSize, &bWritten,
                           NULL); // not using overlapped IO

  if (!result)
    {
      clog << "SystemCompat::writePipe error: '";
      clog << GetLastError () << "' pd = " << pd << endl;
    }
  bytesWritten = (int)bWritten;

  assert (bytesWritten == dataSize);
#else
  bytesWritten = (int) write (pd, (void *)data, dataSize);
#endif

  return bytesWritten;
}

GINGA_SYSTEM_END
