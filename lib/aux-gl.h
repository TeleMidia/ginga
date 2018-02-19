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

#ifndef AUX_GL_H
#define AUX_GL_H

#if defined WITH_OPENGL && WITH_OPENGL
#define GL_GLEXT_PROTOTYPES 1
#if defined WITH_OPENGLES2 && WITH_OPENGLES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#define WITH_OPENGLES2 0
#else
typedef unsigned int GLuint;
typedef float GLfloat;
#endif

#include "ginga.h"

#define CHECK_GL_ERROR()                                                   \
  G_STMT_START                                                             \
  {                                                                        \
    GLenum glerror;                                                        \
    if ((glerror = glGetError ()) != GL_NO_ERROR)                          \
      {                                                                    \
        ERROR ("OpenGL error (%d) in %s:%d.", (int) glerror, __FILE__,     \
               __LINE__);                                                  \
      }                                                                    \
  }                                                                        \
  G_STMT_END

class GL
{

public:
  static void init ();
  static void beginDraw ();

  static void clear_scene (int w, int h);

  static void create_texture (GLuint *);
  static void create_texture (GLuint *, int, int, unsigned char *);
  static void delete_texture (GLuint *);

  static void update_texture (GLuint, int, int, unsigned char *);
  static void update_subtexture (GLuint, int, int, int, int,
                                 unsigned char *);

  static void draw_quad (int, int, int, int, GLuint, GLfloat a = 1.0f);
  static void draw_quad (int, int, int, int, GLfloat, GLfloat, GLfloat,
                         GLfloat);
};

#endif // AUX_GINGA_H
