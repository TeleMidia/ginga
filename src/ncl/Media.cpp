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
#include "Media.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new media.
 * @param id Media id.
 * @param settings True if media is settings node.
 */
Media::Media (const string &id, bool settings)
  : Node (id)
{
  _content = nullptr;
  _descriptor = nullptr;
  _isSettings = settings;
}

/**
 * @brief Destroys media.
 */
Media::~Media ()
{
  if (_content != nullptr)
    delete _content;
  if (_descriptor != nullptr)
    delete _descriptor;
  _instances.clear ();
}

/**
 * @brief Tests if media is settings node.
 */
bool
Media::isSettings ()
{
  return _isSettings;
}

/**
 * @brief Gets media content.
 */
Content *
Media::getContent ()
{
  return _content;
}

/**
 * @brief Sets media content.  (Can only be called once.)
 */
void
Media::setContent (Content *content)
{
  g_assert_null (_content);
  g_assert_nonnull (content);
  _content = content;
}

/**
 * @brief Gets media descriptor.
 */
Descriptor *
Media::getDescriptor ()
{
  return _descriptor;
}

/**
 * @brief Sets media descriptor.  (Can only be called once.)
 */
void
Media::setDescriptor (Descriptor *descriptor)
{
  g_assert_null (_descriptor);
  g_assert_nonnull (descriptor);
  _descriptor = descriptor;
}

/**
 * @brief Add media reference.
 * @param node Refer node.
 */
void
Media::addSameInstance (Refer *node)
{
  g_assert_nonnull (node);
  _instances.insert (node);
}

/**
 * @brief Get media references.
 */
const set<Refer *> *
Media::getInstSameInstances ()
{
  return &_instances;
}

GINGA_NCL_END
