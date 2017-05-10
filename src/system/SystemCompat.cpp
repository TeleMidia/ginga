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

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_SYSTEM_BEGIN

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
    }
}

void
SystemCompat::initializeGingaPrefix ()
{
  SystemCompat::gingaPrefix = "";
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
  static const vector <string> uri_protocols = {
    "x-sbtvdts:",
    "sbtvd-ts:",
    "http://",
    "ftp://",
    "file://",
    "tcp://",
    "udp://",
    "rtp://",
    "ncl-mirror://",
    "rtsp://"
  };

  for (const string &s: uri_protocols)
  {
    if ( uri.compare (0, s.size(), s) == 0 )
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
SystemCompat::setLogTo (SystemCompat::LogType logType, const string &sufix)
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
