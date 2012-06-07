#! /bin/sh -
# run.sh -- Run NCL wrapper.
#
# Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

export LD_LIBRARY_PATH=/usr/local/lib/ginga:/usr/local/lib/ginga/adapters:/usr/local/lib/ginga/cm:/usr/local/lib/ginga/epgfactory:/usr/local/lib/ginga/epgfactory/src:/usr/local/lib/ginga/mb:/usr/local/lib/ginga/mb/dec:/usr/local/lib/ginga/ic:/usr/local/lib/ginga/converters:/usr/local/lib/ginga/dp:/usr/local/lib/ginga/players:/usr/lib/lua/5.1/socket:/usr/local/lib/lua/5.1/socket
exec /usr/local/sbin/ginga --disable-gfx --ncl "$@" 1>/dev/null
