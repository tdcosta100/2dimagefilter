// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include "screenarea-opengl.h"

#include <cstring>

namespace VBA
{

template<typename T> T min( T x, T y ) { return x < y ? x : y; }
template<typename T> T max( T x, T y ) { return x > y ? x : y; }

ScreenAreaGl::ScreenAreaGl(int _iWidth, int _iHeight, int _iScale) :
  ScreenArea(_iWidth, _iHeight, _iScale),
  m_uiScreenTexture(0),
  m_iTextureSize(256)
{
  Glib::RefPtr<Gdk::GL::Config> glconfig;

  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |
                                     Gdk::GL::MODE_DOUBLE);
  if (!glconfig)
    {
      fprintf(stderr, "*** OpenGL : Cannot open display.\n");
      throw std::exception();
    }

  set_gl_capability(glconfig);

  vUpdateSize();
}

void ScreenAreaGl::on_realize()
{
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return;

    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    if (glIsTexture(m_uiScreenTexture))
      glDeleteTextures(1, &m_uiScreenTexture);



    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, 1.0, 1.0, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glGenTextures(1, &m_uiScreenTexture);
    glBindTexture(GL_TEXTURE_2D, m_uiScreenTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Calculate texture size as a the smallest working power of two
    float n1 = log10((float)m_iScaledWidth ) / log10( 2.0f);
    float n2 = log10((float)m_iScaledHeight ) / log10( 2.0f);
    float n = (n1 > n2)? n1 : n2;

      // round up
    if (((float)((int)n)) != n)
      n = ((float)((int)n)) + 1.0f;

    m_iTextureSize = (int)pow(2.0f, n);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iTextureSize, m_iTextureSize, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, NULL);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

  glwindow->gl_end();
}

void ScreenAreaGl::vDrawPixels(u8 * _puiData)
{
  ScreenArea::vDrawPixels(_puiData);

  queue_draw_area(0, 0, get_width(), get_height());
}

void ScreenAreaGl::vDrawBlackScreen()
{
  if (m_puiPixels && is_realized())
  {
    memset(m_puiPixels, 0, m_iHeight * (m_iWidth + 1) * sizeof(u32));
    queue_draw_area(0, 0, get_width(), get_height());
  }
}

void ScreenAreaGl::vOnWidgetResize()
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  int iWidth = get_width();
  int iHeight = get_height();
  
  float fScreenAspect = (float) m_iScaledWidth / m_iScaledHeight,
        fWindowAspect = (float) iWidth / iHeight;

  if (!glwindow->gl_begin(get_gl_context()))
    return;

    if (fWindowAspect == fScreenAspect)
      glViewport(0, 0, iWidth, iHeight);
    else if (fWindowAspect < fScreenAspect) {
      int iAspectHeight = (int)(iWidth / fScreenAspect);
      glViewport(0, (iHeight - iAspectHeight) / 2, iWidth, iAspectHeight);
    } else {
      int iAspectWidth = (int)(iHeight * fScreenAspect);
      glViewport((iWidth - iAspectWidth) / 2, 0, iAspectWidth, iHeight);
    }
  
  glwindow->gl_end();
}

bool ScreenAreaGl::on_expose_event(GdkEventExpose * _pstEvent)
{
  if (!m_bEnableRender)
    return true;

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

    glClear( GL_COLOR_BUFFER_BIT );
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_iScaledWidth + 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iScaledWidth + 1, m_iScaledHeight,
                      GL_RGBA, GL_UNSIGNED_BYTE, m_puiPixels);

    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3i(0, 0, 0);
      glTexCoord2f(m_iScaledWidth / (GLfloat) m_iTextureSize, 0.0f);
      glVertex3i(1, 0, 0);
      glTexCoord2f(0.0f, m_iScaledHeight / (GLfloat) m_iTextureSize);
      glVertex3i(0, 1, 0);
      glTexCoord2f(m_iScaledWidth / (GLfloat) m_iTextureSize,
                  m_iScaledHeight / (GLfloat) m_iTextureSize);
      glVertex3i(1, 1, 0);
    glEnd();

    glwindow->swap_buffers();

  glwindow->gl_end();

  return true;
}

} // namespace VBA
