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

#include "aux-ginga.h"
#include "aux-gl.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wimplicit-fallthrough)

// OpenGL ------------------------------------------------------------------
# if defined WITH_OPENGLES2 && WITH_OPENGLES2
auto vertexSource =
    "uniform vec2 winSize;\n"
    "\n"
    "in vec2 pos;\n"
    "in vec4 color;\n"
    "in vec2 texcoord;\n"
    "\n"
    "out vec4 f_color;\n"
    "out vec2 f_texcoord;\n"
    "void main() {\n"
    "   pos.x = ((pos.x)/winSize.x) * 2.0f - 1.0;\n"
    "   pos.y = ((pos.y)/winSize.y) * (-2.0f) + 1.0f;\n"
    "   gl_Position = vec4 ( pos.x, pos.y, 0.0, 1.0);\n"
    "   f_texcoord = texcoord;\n"
    "   f_color = color;\n"
    "}\n";

static auto fragmentSource =
    "uniform int use_tex;\n"
    "uniform sampler2D tex;\n"
    "\n"
    "in vec4 f_color;\n"
    "in vec2 f_texcoord;\n"
    "\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "   vec4 t0 = texture2D(tex, f_texcoord);\n"
    "   outColor = use_tex * t0 * f_color + "
    "              (1.0-use_tex) * f_color;\n"
    "}\n";

struct GLES2Ctx
{
  GLuint vertexShader, fragmentShader, shaderProgram;

  // Buffers
  GLuint vbo;
  GLuint ebo;

  // Attributes
  GLint posAttr;
  GLint colorAttr;
  GLint texAttr;

  // Log
  GLchar log [255];
  GLint log_len = 0;
};

static struct GLES2Ctx gles2ctx;

struct sprite {
  GLfloat pos [2];
  GLfloat v_color [4];
  GLfloat tex_coords [2];
};

static struct sprite vertices[] = {
  {{0.0f, 0.0f}, {1.0, 1.0, 1.0, 1.0}, {0.0f, 0.0f}},
  {{400.0f, 0.0f}, {1.0, 1.0, 1.0, 1.0}, {1.0f, 0.0f}},
  {{400.0f, 400.0f}, {1.0, 1.0, 1.0, 1.0}, {1.0f, 1.0f}},
  {{0.0f, 400.0f}, {1.0, 1.0, 1.0, 1.0}, {0.0f, 1.0f}} };

static GLuint elements[] = {
  0, 1, 2,
  2, 3, 0
};
# endif

/**
 * @brief GL::init Initiliazes the OpenGL context.
 */
void
GL::init ()
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else

# if defined WITH_OPENGLES2 && WITH_OPENGLES2
  glGenBuffers (1, &gles2ctx.vbo);
  glBindBuffer (GL_ARRAY_BUFFER, gles2ctx.vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers (1, &gles2ctx.ebo);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, gles2ctx.ebo);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (elements), elements,
                GL_STATIC_DRAW);

  gles2ctx.vertexShader = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (gles2ctx.vertexShader, 1, &vertexSource, nullptr);
  glCompileShader (gles2ctx.vertexShader);

  gles2ctx.fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (gles2ctx.fragmentShader, 1, &fragmentSource, nullptr);
  glCompileShader (gles2ctx.fragmentShader);
  gles2ctx.shaderProgram = glCreateProgram ();
  glAttachShader (gles2ctx.shaderProgram, gles2ctx.vertexShader);
  glAttachShader (gles2ctx.shaderProgram, gles2ctx.fragmentShader);
  glLinkProgram (gles2ctx.shaderProgram);
  glUseProgram (gles2ctx.shaderProgram);

  glGetShaderInfoLog (gles2ctx.vertexShader, 255, &gles2ctx.log_len, gles2ctx.log);

  TRACE ("%s.", gles2ctx.log);

  CHECK_GL_ERROR ();

  gles2ctx.posAttr = glGetAttribLocation (gles2ctx.shaderProgram, "pos");
  if (gles2ctx.posAttr < 0)
    WARNING ("Shader pos attribute not found.");

  gles2ctx.colorAttr = glGetAttribLocation (gles2ctx.shaderProgram, "color");
  if (gles2ctx.colorAttr < 0)
    WARNING ("Shader color attribute not found.");

  gles2ctx.texAttr = glGetAttribLocation (gles2ctx.shaderProgram, "texcoord");
  if (gles2ctx.texAttr < 0)
    WARNING ("Shader texcoord attribute not found.");

# endif
  CHECK_GL_ERROR ();
#endif
}

/**
 * @brief GL::clear_scene Clear and configure OpenGL context
 */
void
GL::clear_scene (int w, int h)
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  glViewport (0.0, 0.0, w, h);

# if WITH_OPENGLES2
  GLint loc = glGetUniformLocation (gles2ctx.shaderProgram, "winSize");
  g_assert (loc);
  glUniform2f (loc, (GLfloat) w, (GLfloat) h);
  loc = glGetUniformLocation (gles2ctx.shaderProgram, "use_tex");
  glUniform1i (loc, 0);
# else
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0.0, w, h, 0.0, 0.0, 1.0);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
# endif

  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation (GL_FUNC_ADD);

  glEnable (GL_TEXTURE_2D);
#endif
}

/**
 * @brief GL::create_texture Creates a new uninitialized OpenGL texture.
 */
void
GL::create_texture (GLuint *gltex)
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  glGenTextures (1, gltex);
  glBindTexture (GL_TEXTURE_2D, *gltex);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  CHECK_GL_ERROR ();
#endif
}

/**
 * @brief GL::create_texture Creates a new OpenGL texture and initializes it
 *  with the data content.
 */
void
GL::create_texture (GLuint *gltex, int tex_w, int tex_h, unsigned char *data)
{
#if !(defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  create_texture (gltex);
  glTexImage2D (GL_TEXTURE_2D, 0, 4,
                tex_w, tex_h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

  CHECK_GL_ERROR ();
#endif
}

/**
 * @brief GL::delete_texture Deletes the texture.
 */
void
GL::delete_texture (GLuint *gltex)
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  if (*gltex)
    {
      glDeleteTextures (1, gltex);
    }

  CHECK_GL_ERROR ();
#endif
}

/**
 * @brief GL::update_texture Updates texture with the data content.
 */
void
GL::update_texture (GLuint gltex, int tex_w, int tex_h, unsigned char *data)
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  g_assert (gltex > 0);
  glBindTexture (GL_TEXTURE_2D, gltex);
  glTexImage2D (GL_TEXTURE_2D, 0, 4,
                tex_w, tex_h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
  glBindTexture (GL_TEXTURE_2D, 0);
  CHECK_GL_ERROR ();
#endif
}

/**
 * @brief GL::update_subtexture Updates subtexture with the data content.
 */
void
GL::update_subtexture (GLuint gltex,
                      int xoffset, int yoffset, int width, int height,
                      unsigned char *data)
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  glBindTexture (GL_TEXTURE_2D, gltex);
  glTexSubImage2D (GL_TEXTURE_2D,
                   0,
                   xoffset,
                   yoffset,
                   width,
                   height,
                   GL_BGRA_EXT,
                   GL_UNSIGNED_BYTE,
                   data);
  glBindTexture(GL_TEXTURE_2D, 0);
  CHECK_GL_ERROR ();
#endif
}

/**
 * @brief GL::draw_quad Draws a textured rectangle
 */
void
GL::draw_quad (int x, int y, int w, int h, GLuint gltex, GLfloat alpha)
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  g_assert (gltex > 0);
  glBindTexture (GL_TEXTURE_2D, gltex);

# if WITH_OPENGLES2
  GLint loc = glGetUniformLocation (gles2ctx.shaderProgram, "use_tex");
  glUniform1i (loc, 1);

  vertices[0].pos[0] = (GLfloat) x;
  vertices[0].pos[1] = (GLfloat) y;
  vertices[0].v_color[0] = 1.0;
  vertices[0].v_color[1] = 1.0;
  vertices[0].v_color[2] = 1.0;
  vertices[0].v_color[3] = alpha;

  vertices[1].pos[0] = (GLfloat)(x + w);
  vertices[1].pos[1] = (GLfloat) y;
  vertices[1].v_color[0] = 1.0;
  vertices[1].v_color[1] = 1.0;
  vertices[1].v_color[2] = 1.0;
  vertices[1].v_color[3] = alpha;

  vertices[2].pos[0] = (GLfloat)(x + w);
  vertices[2].pos[1] = (GLfloat)(y + h);
  vertices[2].v_color[0] = 1.0;
  vertices[2].v_color[1] = 1.0;
  vertices[2].v_color[2] = 1.0;
  vertices[2].v_color[3] = alpha;

  vertices[3].pos[0] = (GLfloat) x;
  vertices[3].pos[1] = (GLfloat)(y + h);
  vertices[3].v_color[0] = 1.0;
  vertices[3].v_color[1] = 1.0;
  vertices[3].v_color[2] = 1.0;
  vertices[3].v_color[3] = alpha;

  glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray ((GLuint) gles2ctx.posAttr);
  glVertexAttribPointer ((GLuint) gles2ctx.posAttr, 2, GL_FLOAT, GL_FALSE,
                         sizeof (struct sprite),
                         0);

  glEnableVertexAttribArray ((GLuint) gles2ctx.colorAttr);
  glVertexAttribPointer ((GLuint) gles2ctx.colorAttr, 4, GL_FLOAT, GL_FALSE,
                         sizeof (struct sprite),
                         (GLvoid*) (2 * sizeof (GLfloat)));

  glEnableVertexAttribArray ((GLuint) gles2ctx.texAttr);
  glVertexAttribPointer ((GLuint) gles2ctx.texAttr, 2, GL_FLOAT, GL_FALSE,
                         sizeof (struct sprite),
                         (GLvoid*) (6 * sizeof (GLfloat)) );

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation (GL_FUNC_ADD);

  // glDrawArrays (GL_TRIANGLES, 0, 3);
  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
# else
  glColor4f (1.0f, 1.0f, 1.0f, alpha);
  glBegin( GL_QUADS );
    glTexCoord2d (0.0,0.0); glVertex2d (x, y);
    glTexCoord2d (1.0,0.0); glVertex2d (x + w, y);
    glTexCoord2d (1.0,1.0); glVertex2d (x + w, y + h);
    glTexCoord2d (0.0,1.0); glVertex2d (x, y + h);
  glEnd();
# endif
  glBindTexture (GL_TEXTURE_2D, 0);

  CHECK_GL_ERROR ();
#endif
}

/**
 * @brief GL::draw_quad Draws a colored rectangle
 */
void
GL::draw_quad (int x, int y, int w, int h,
               GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
#if ! (defined WITH_OPENGL && WITH_OPENGL)
  ERROR_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
# if WITH_OPENGLES2
  GLint loc = glGetUniformLocation (gles2ctx.shaderProgram, "use_tex");
  glUniform1i (loc, 0);

  vertices[0].pos[0] = (GLfloat) x;
  vertices[0].pos[1] = (GLfloat) y;
  vertices[0].v_color[0] = r;
  vertices[0].v_color[1] = g;
  vertices[0].v_color[2] = b;
  vertices[0].v_color[3] = a;

  vertices[1].pos[0] = (GLfloat)(x + w);
  vertices[1].pos[1] = (GLfloat) y;
  vertices[1].v_color[0] = r;
  vertices[1].v_color[1] = g;
  vertices[1].v_color[2] = b;
  vertices[1].v_color[3] = a;

  vertices[2].pos[0] = (GLfloat)(x + w);
  vertices[2].pos[1] = (GLfloat)(y + h);
  vertices[2].v_color[0] = r;
  vertices[2].v_color[1] = g;
  vertices[2].v_color[2] = b;
  vertices[2].v_color[3] = a;

  vertices[3].pos[0] = (GLfloat) x;
  vertices[3].pos[1] = (GLfloat)(y + h);
  vertices[3].v_color[0] = r;
  vertices[3].v_color[1] = g;
  vertices[3].v_color[2] = b;
  vertices[3].v_color[3] = a;

  glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
# else
  glColor4f (r, g, b, a);
  glBegin (GL_QUADS);
    glVertex2d (x, y);
    glVertex2d (x + w, y);
    glVertex2d (x + w, y + h);
    glVertex2d (x, y + h);
  glEnd();
# endif
  CHECK_GL_ERROR ();
#endif
}
