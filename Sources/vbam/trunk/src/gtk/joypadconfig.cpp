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

#include "joypadconfig.h"

#include <gtkmm/stock.h>

#include "intl.h"

namespace VBA
{

const JoypadConfigDialog::SJoypadKey JoypadConfigDialog::m_astKeys[] =
{
    { KEY_UP,             "Up :"         },
    { KEY_DOWN,           "Down :"       },
    { KEY_LEFT,           "Left :"       },
    { KEY_RIGHT,          "Right :"      },
    { KEY_BUTTON_A,       "Button A :"   },
    { KEY_BUTTON_B,       "Button B :"   },
    { KEY_BUTTON_L,       "Button L :"   },
    { KEY_BUTTON_R,       "Button R :"   },
    { KEY_BUTTON_SELECT,  "Select :"     },
    { KEY_BUTTON_START,   "Start :"      },
    { KEY_BUTTON_SPEED,   "Speed :"      },
    { KEY_BUTTON_CAPTURE, "Capture :"    },
    { KEY_BUTTON_AUTO_A,  "Autofire A :" },
    { KEY_BUTTON_AUTO_B,  "Autofire B :" }
};

JoypadConfigDialog::JoypadConfigDialog(Config::Section * _poConfig) :
  Gtk::Dialog("Joypad config", true, true),
  m_oTitleHBox(false, 5),
  m_oTitleLabel("Joypad :", Gtk::ALIGN_RIGHT),
  m_oDefaultJoypad("Default joypad"),
  m_oTable(G_N_ELEMENTS(m_astKeys), 2, false),
  m_bUpdating(false),
  m_ePad(PAD_MAIN),
  m_poConfig(_poConfig)
{
  // Joypad selection
  m_oTitleCombo.append_text("1");
  m_oTitleCombo.append_text("2");
  m_oTitleCombo.append_text("3");
  m_oTitleCombo.append_text("4");

  m_oTitleHBox.pack_start(m_oTitleLabel, Gtk::PACK_SHRINK);
  m_oTitleHBox.pack_start(m_oTitleCombo);

  // Joypad buttons
  for (guint i = 0; i < G_N_ELEMENTS(m_astKeys); i++)
  {
    Gtk::Label * poLabel = Gtk::manage( new Gtk::Label(m_astKeys[i].m_csKeyName, Gtk::ALIGN_RIGHT) );
    Gtk::Entry * poEntry = Gtk::manage( new Gtk::Entry() );
    m_oTable.attach(* poLabel, 0, 1, i, i + 1);
    m_oTable.attach(* poEntry, 1, 2, i, i + 1);
    m_oEntries.push_back(poEntry);

    poEntry->signal_focus_in_event().connect(sigc::bind(
                                               sigc::mem_fun(*this, &JoypadConfigDialog::bOnEntryFocusIn), i));
    poEntry->signal_focus_out_event().connect(sigc::mem_fun(*this, &JoypadConfigDialog::bOnEntryFocusOut));
  }

  // Dialog validation button
  m_poOkButton = add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);

  // Layout
  m_oTitleHBox.set_border_width(5);
  m_oTable.set_border_width(5);
  m_oTable.set_spacings(5);
  get_vbox()->set_spacing(5);
  get_vbox()->pack_start(m_oTitleHBox);
  get_vbox()->pack_start(m_oSeparator);
  get_vbox()->pack_start(m_oDefaultJoypad);
  get_vbox()->pack_start(m_oTable);

  // Signals and default values
  m_oConfigSig = Glib::signal_idle().connect(sigc::mem_fun(*this, &JoypadConfigDialog::bOnConfigIdle),
          Glib::PRIORITY_DEFAULT_IDLE);
  m_oDefaultJoypad.signal_toggled().connect(sigc::mem_fun(*this,
              &JoypadConfigDialog::vOnDefaultJoypadSelect) );
  m_oTitleCombo.signal_changed().connect(sigc::mem_fun(*this,
              &JoypadConfigDialog::vOnJoypadSelect) );

  m_oTitleCombo.set_active_text("1");

  show_all_children();
}

JoypadConfigDialog::~JoypadConfigDialog()
{
  m_oConfigSig.disconnect();
}

void JoypadConfigDialog::vUpdateEntries()
{
  m_bUpdating = true;
  m_oDefaultJoypad.set_active(inputGetDefaultJoypad() == m_ePad);

  for (guint i = 0; i < m_oEntries.size(); i++)
  {
    const char * csName = 0;

    guint uiKeyval = inputGetKeymap(m_ePad, m_astKeys[i].m_eKeyFlag);
    int dev = uiKeyval >> 16;
    if (dev == 0)
    {
      csName = gdk_keyval_name(uiKeyval);
    }
    else
    {
      int what = uiKeyval & 0xffff;
      std::stringstream os;
      os << "Joy " << dev;

      if(what >= 128)
      {
        // joystick button
        int button = what - 128;
        os << " Button " << button;
      }
      else if (what < 0x20)
      {
        // joystick axis
        int dir = what & 1;
		what >>= 1;
	    os << " Axis " << what << (dir?'-':'+');
      }
      else if (what < 0x30)
      {
        // joystick hat
        what = (what & 15);
        what >>= 2;
        os << " Hat " << what;
      }

      csName = os.str().c_str();
    }

    if (csName == 0)
    {
      m_oEntries[i]->set_text(_("<Undefined>"));
    }
    else
    {
      m_oEntries[i]->set_text(csName);
    }
  }

  m_bUpdating = false;
}

bool JoypadConfigDialog::bOnEntryFocusIn(GdkEventFocus * _pstEvent,
                                         guint           _uiEntry)
{
  m_iCurrentEntry = _uiEntry;

  return false;
}

bool JoypadConfigDialog::bOnEntryFocusOut(GdkEventFocus * _pstEvent)
{
  m_iCurrentEntry = -1;

  return false;
}

bool JoypadConfigDialog::on_key_press_event(GdkEventKey * _pstEvent)
{
  if (m_iCurrentEntry < 0)
  {
    return Gtk::Window::on_key_press_event(_pstEvent);
  }

  // Forward the keyboard event by faking a SDL event
  SDL_Event event;
  event.type = SDL_KEYDOWN;
  event.key.keysym.sym = (SDLKey)_pstEvent->keyval;
  vOnInputEvent(event);

  return true;
}

void JoypadConfigDialog::on_response(int response_id)
{
  m_poConfig->vSetKey("active_joypad", inputGetDefaultJoypad());
}

void JoypadConfigDialog::vOnInputEvent(const SDL_Event &event)
{
  if (m_iCurrentEntry < 0)
  {
	return;
  }

  int code = inputGetEventCode(event);
  inputSetKeymap(m_ePad, m_astKeys[m_iCurrentEntry].m_eKeyFlag, code);
  vUpdateEntries();

  if (m_iCurrentEntry + 1 < (gint)m_oEntries.size())
  {
    m_oEntries[m_iCurrentEntry + 1]->grab_focus();
  }
  else
  {
    m_poOkButton->grab_focus();
  }
}

bool JoypadConfigDialog::bOnConfigIdle()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
    case SDL_JOYAXISMOTION:
      if (abs(event.jaxis.value) < 16384) continue;
      if (event.jaxis.which != m_oPreviousEvent.jaxis.which
              || event.jaxis.axis != m_oPreviousEvent.jaxis.axis
              || (event.jaxis.value > 0 && m_oPreviousEvent.jaxis.value < 0)
              || (event.jaxis.value < 0 && m_oPreviousEvent.jaxis.value > 0))
      {
        vOnInputEvent(event);
        m_oPreviousEvent = event;
      }
      vEmptyEventQueue();
      break;
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONUP:
      vOnInputEvent(event);
      vEmptyEventQueue();
      break;
    }
  }

  return true;
}

void JoypadConfigDialog::vEmptyEventQueue()
{
  // Empty the SDL event queue
  SDL_Event event;
  while(SDL_PollEvent(&event));
}

void JoypadConfigDialog::vOnJoypadSelect()
{
  std::string oText = m_oTitleCombo.get_active_text();

  if (oText == "1")
  {
    m_ePad = PAD_1;
  }
  else if (oText == "2")
  {
    m_ePad = PAD_2;
  }
  else if (oText == "3")
  {
    m_ePad = PAD_3;
  }
  else if (oText == "4")
  {
    m_ePad = PAD_4;
  }

  vEmptyEventQueue();
  memset(&m_oPreviousEvent, 0, sizeof(m_oPreviousEvent));

  vUpdateEntries();
}

void JoypadConfigDialog::vOnDefaultJoypadSelect()
{
  if (m_bUpdating) return;

  if (m_oDefaultJoypad.get_active())
  {
    inputSetDefaultJoypad(m_ePad);
  }
  else
  {
    inputSetDefaultJoypad(PAD_MAIN);
  }
}

} // namespace VBA
