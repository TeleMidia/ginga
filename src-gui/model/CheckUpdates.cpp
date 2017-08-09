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

#include "ginga_gtk.h"
#include <curl/curl.h>

static char errorBuffer[CURL_ERROR_SIZE];
static std::string buffer;


int
writer (char *data, size_t size, size_t nmemb, std::string *writerData)
{
  if (writerData == NULL)
    return 0;

  writerData->append (data, size * nmemb);

  return size * nmemb;
}

gboolean
init (CURL *&conn, gchar *url)
{
    /*
  CURLcode code;

  conn = curl_easy_init ();

  if (conn == NULL)
    {
      fprintf (stderr, "Failed to create CURL connection\n");
      return false;
    }

  code = curl_easy_setopt (conn, CURLOPT_ERRORBUFFER, errorBuffer);
  if (code != CURLE_OK)
    {
      fprintf (stderr, "Failed to set error buffer [%d]\n", code);
      return false;
    }

  code = curl_easy_setopt (conn, CURLOPT_URL, url);
  if (code != CURLE_OK)
    {
      fprintf (stderr, "Failed to set URL [%s]\n", errorBuffer);
      return false;
    }

  code = curl_easy_setopt (conn, CURLOPT_FOLLOWLOCATION, 1L);
  if (code != CURLE_OK)
    {
      fprintf (stderr, "Failed to set redirect option [%s]\n", errorBuffer);
      return false;
    }

  code = curl_easy_setopt (conn, CURLOPT_WRITEFUNCTION, writer);
  if (code != CURLE_OK)
    {
      fprintf (stderr, "Failed to set writer [%s]\n", errorBuffer);
      return false;
    }

  code = curl_easy_setopt (conn, CURLOPT_WRITEDATA, &buffer);
  if (code != CURLE_OK)
    {
      fprintf (stderr, "Failed to set write data [%s]\n", errorBuffer);
      return false;
    }
 */
  return true;
}

void
check_updates (void)
{

  CURL *conn = NULL;
  CURLcode code;

 // curl_global_init (CURL_GLOBAL_DEFAULT);
/*
  if (!init (conn, "https://www.google.com.br/"))
    {
      fprintf (stderr, "Connection initializion failed\n");
      return;
    }

  code = curl_easy_perform (conn);
  curl_easy_cleanup (conn);

  if (code != CURLE_OK)
    {
      fprintf (stderr, "Failed to get '%s' [%s]\n",
               "https://www.google.com.br/", errorBuffer);
      exit (EXIT_FAILURE);
    }
 */

}