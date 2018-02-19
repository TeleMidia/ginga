/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-glib.h"
#include "ginga_gtk.h"
#ifdef G_OS_WIN32
#include <windows.h>
#else
#include <sys/utsname.h>
#endif
#include <libsoup/soup.h>

TrackerFlags trackerFlags;
gchar *baseURI = (gchar *) "http://telemidia.puc-rio.br/~busson/notify.php";

void
http_message_callback (unused (SoupSession *session), SoupMessage *msg,
                       gpointer user_data)
{
  int log_type = GPOINTER_TO_INT (user_data);
  if (log_type < 0) // log_type < 0  = check for update
    {
      SoupBuffer *buffer = soup_message_body_flatten (msg->response_body);
      if (strlen (buffer->data) == 0) // if msg is empty, finalize
        return;
      if (strcmp ((buffer->data), GINGA->version ().substr (0, 6).c_str ()))
        show_ginga_update_alertbox ();
    }
}

void
send_http_log_message (gint log_type, const gchar *log_message)
{
  if (trackerFlags.trackerAccept == 0)
    return;

  SoupSession *session = soup_session_new_with_options (
      SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_CONTENT_SNIFFER, NULL);

#ifdef G_OS_WIN32
  SYSTEM_INFO siSysInfo;
  GetSystemInfo (&siSysInfo);
  OSVERSIONINFO vi;
  memset (&vi, 0, sizeof vi);
  vi.dwOSVersionInfoSize = sizeof vi;
  GetVersionEx (&vi);

  gchar *url = g_markup_printf_escaped (
      "%s?id=%s&os=%s&os_v=%d&ginga_v=%s&arch=%u&type=%d&msg=%s", baseURI,
      gingaID, "Windows", vi.dwMajorVersion, GINGA->version ().c_str (),
      siSysInfo.dwProcessorType, log_type, log_message);
#else
  struct utsname uname_pointer;
  uname (&uname_pointer);
  gchar *url = g_markup_printf_escaped (
      "%s?id=%s&os=%s&os_v=%s&ginga_v=%s&arch=%s&type=%d&msg=%s", baseURI,
      gingaID, uname_pointer.sysname, uname_pointer.release,
      GINGA->version ().c_str (), uname_pointer.machine, log_type,
      log_message);
#endif
  // printf ("url: %s \n", url);
  SoupMessage *msg = soup_message_new ("GET", url);
  soup_session_queue_message (session, msg, http_message_callback,
                              GINT_TO_POINTER (log_type));
}
