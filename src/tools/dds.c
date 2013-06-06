/*
 * dds.c -- dds texture loader
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut dds.c -o dds
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include "glut_wrap.h"


/* OpenGL texture info */
struct gl_texture_t
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint internalFormat;
  GLuint id;

  GLubyte *texels;

  GLint numMipmaps;
};

/* DirectDraw's structures */
struct DDPixelFormat
{
  GLuint size;
  GLuint flags;
  GLuint fourCC;
  GLuint bpp;
  GLuint redMask;
  GLuint greenMask;
  GLuint blueMask;
  GLuint alphaMask;
};

struct DDSCaps
{
  GLuint caps;
  GLuint caps2;
  GLuint caps3;
  GLuint caps4;
};

struct DDColorKey
{
  GLuint lowVal;
  GLuint highVal;
};

struct DDSurfaceDesc
{
  GLuint size;
  GLuint flags;
  GLuint height;
  GLuint width;
  GLuint pitch;
  GLuint depth;
  GLuint mipMapLevels;
  GLuint alphaBitDepth;
  GLuint reserved;
  GLuint surface;

  struct DDColorKey ckDestOverlay;
  struct DDColorKey ckDestBlt;
  struct DDColorKey ckSrcOverlay;
  struct DDColorKey ckSrcBlt;

  struct DDPixelFormat format;
  struct DDSCaps caps;

  GLuint textureStage;
};

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
  (GLuint)( \
    (((GLuint)(GLubyte)(ch3) << 24) & 0xFF000000) | \
    (((GLuint)(GLubyte)(ch2) << 16) & 0x00FF0000) | \
    (((GLuint)(GLubyte)(ch1) <<  8) & 0x0000FF00) | \
     ((GLuint)(GLubyte)(ch0)        & 0x000000FF) )
#endif

#define FOURCC_DXT1 MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT3 MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT5 MAKEFOURCC('D', 'X', 'T', '5')

/* texture Id */
GLuint texId;


#ifndef max
static int
max (int a, int b)
{
  return ((a > b) ? a : b);
}
#endif

static struct gl_texture_t *
ReadDDSFile (const char *filename)
{
  struct DDSurfaceDesc ddsd;
  struct gl_texture_t *texinfo;
  FILE *fp;
  char magic[4];
  long bufferSize, curr, end;

  /* Open the file */
  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return NULL;
    }

  /* Read magic number and check if valid .dds file */
  fread (&magic, sizeof (char), 4, fp);

  if (strncmp (magic, "DDS ", 4) != 0)
    {
      fprintf (stderr, "the file \"%s\" doesn't appear to be"
	       "a valid .dds file!\n", filename);
      fclose (fp);
      return NULL;
    }

  /* Get the surface descriptor */
  fread (&ddsd, sizeof (ddsd), 1, fp);

  texinfo = (struct gl_texture_t *)
    calloc (sizeof (struct gl_texture_t), 1);
  texinfo->width = ddsd.width;
  texinfo->height = ddsd.height;
  texinfo->numMipmaps = ddsd.mipMapLevels;

  switch (ddsd.format.fourCC)
    {
    case FOURCC_DXT1:
      /* DXT1's compression ratio is 8:1 */
      texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      texinfo->internalFormat = 3;
      break;

    case FOURCC_DXT3:
      /* DXT3's compression ratio is 4:1 */
      texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      texinfo->internalFormat = 4;
      break;

    case FOURCC_DXT5:
      /* DXT5's compression ratio is 4:1 */
      texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      texinfo->internalFormat = 4;
      break;

    default:
      /* Bad fourCC, unsupported or bad format */
      fprintf (stderr, "the file \"%s\" doesn't appear to be"
	       "compressed using DXT1, DXT3, or DXT5! [%i]\n",
	       filename, ddsd.format.fourCC);
      free (texinfo);
      fclose (fp);
      return NULL;
    }

  /* Calculate pixel data size */
  curr = ftell (fp);
  fseek (fp, 0, SEEK_END);
  end = ftell (fp);
  fseek (fp, curr, SEEK_SET);
  bufferSize = end - curr;

  /* Read pixel data with mipmaps */
  texinfo->texels = (GLubyte *)malloc (bufferSize * sizeof (GLubyte));
  fread (texinfo->texels, sizeof (GLubyte), bufferSize, fp);

  /* Close the file */
  fclose (fp);
  return texinfo;
}

static GLuint
loadDDSTexture (const char *filename)
{
  struct gl_texture_t *compressed_texture = NULL;
  GLsizei mipWidth, mipHeight, mipSize;
  int blockSize, offset;
  GLuint tex_id = 0;
  GLint mip;

  /* Read texture from file */
  compressed_texture = ReadDDSFile (filename);

  if (compressed_texture && compressed_texture->texels)
    {
      /* Generate new texture */
      glGenTextures (1, &compressed_texture->id);
      glBindTexture (GL_TEXTURE_2D, compressed_texture->id);

      /* Setup some parameters for texture filters and mipmapping */
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      mipWidth = compressed_texture->width;
      mipHeight = compressed_texture->height;
      blockSize = (compressed_texture->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
      offset = 0;

      /* Upload mipmaps to video memory */
      for (mip = 0; mip < compressed_texture->numMipmaps; ++mip)
	{
	  mipSize = ((mipWidth + 3) / 4) * ((mipHeight + 3) / 4) * blockSize;

	  glCompressedTexImage2D (GL_TEXTURE_2D, mip, compressed_texture->format,
				  mipWidth, mipHeight, 0, mipSize,
				  compressed_texture->texels + offset);

	  mipWidth = max (mipWidth >> 1, 1);
	  mipHeight = max (mipHeight >> 1, 1);

	  offset += mipSize;
	}

      tex_id = compressed_texture->id;

      /* Opengl has its own copy of pixels */
      free (compressed_texture->texels);
      free (compressed_texture);
    }

  return tex_id;
}

static void
cleanup (void)
{
  glDeleteTextures (1, &texId);
}

static void
init (const char *filename)
{
  const char *glexts = (const char *)glGetString (GL_EXTENSIONS);

  /* Initialize OpenGL */
  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Check for S3TC support */
  if (!strstr (glexts, "GL_EXT_texture_compression_s3tc"))
    {
      fprintf (stderr, "error: GL_EXT_texture_compression_s3tc "
	       "extension is required for DDS textures!\n");
      exit(-1);
    }


  /* Load DDS texture from file */
  texId = loadDDSTexture (filename);
  if (!texId)
    exit (EXIT_FAILURE);
}

static void
reshape (int w, int h)
{
  if (h == 0)
    h = 1;

  glViewport (0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  if (w <= h)
    gluOrtho2D (-1.0, 1.0, -1.0*(GLfloat)h/(GLfloat)w, 1.0*(GLfloat)h/(GLfloat)w);
  else
    gluOrtho2D (-1.0*(GLfloat)w/(GLfloat)h, 1.0*(GLfloat)w/(GLfloat)h, -1.0, 1.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glutPostRedisplay ();
}

static void
display (void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, texId);

  /* Draw textured quad */
  glTranslatef (0.0, 0.0, 0.5);
  glBegin (GL_QUADS);
    glTexCoord2f (0.0f, 0.0f);
    glVertex3f (-1.0f, -1.0f, 0.0f);

    glTexCoord2f (1.0f, 0.0f);
    glVertex3f (1.0f, -1.0f, 0.0f);

    glTexCoord2f (1.0f, 1.0f);
    glVertex3f (1.0f, 1.0f, 0.0f);

    glTexCoord2f (0.0f, 1.0f);
    glVertex3f (-1.0f, 1.0f, 0.0f);
  glEnd  ();

  glDisable (GL_TEXTURE_2D);

  glutSwapBuffers ();
}

static void
keyboard (unsigned char key, int x, int y)
{
  /* Escape */
  if (key == 27)
    exit (0);
}

int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
      fprintf (stderr, "usage: %s <filename.dds>\n", argv[0]);
      return -1;
    }

  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize (512, 512);
  glutCreateWindow ("DDS Texture Demo");

  /* Initialize OpenGL extensions */
  glewInit();

  atexit (cleanup);
  init (argv[1]);

  glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutKeyboardFunc (keyboard);

  glutMainLoop ();

  return 0;
}
