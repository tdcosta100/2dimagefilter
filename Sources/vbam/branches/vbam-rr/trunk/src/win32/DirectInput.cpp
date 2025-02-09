#include "stdafx.h"
#include "VBA.h"
#include "Input.h"
#include "Reg.h"
#include "WinResUtil.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment( lib, "dinput8" )
#pragma comment( lib, "dxguid" )


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void directXMessage(const char *);
extern void winlog(const char *msg,...);

#define POV_UP    1
#define POV_DOWN  2
#define POV_RIGHT 4
#define POV_LEFT  8

class DirectInput : public Input {
public:
    virtual void checkDevices();
    DirectInput();
    virtual ~DirectInput();

    virtual bool initialize();
    virtual bool readDevices();
    virtual u32 readDevice(int which);
    virtual CString getKeyName(LONG_PTR key);
    virtual void checkKeys();
    virtual void checkMotionKeys();
    virtual void activate();
    virtual void loadSettings();
    virtual void saveSettings();
};

struct deviceInfo {
    LPDIRECTINPUTDEVICE8 device;
    BOOL isPolled;
    int nButtons;
    int nAxes;
    int nPovs;
    BOOL first;
    struct {
        DWORD offset;
        LONG center;
        LONG negative;
        LONG positive;
    } axis[8];
    int needed;
    union {
        UCHAR data[256];
        DIJOYSTATE state;
    };
};

static deviceInfo *currentDevice = NULL;
static int numDevices = 1;
static deviceInfo *pDevices = NULL;
static LPDIRECTINPUT8 pDirectInput = NULL;
static int axisNumber = 0;




LONG_PTR defvalues[JOYPADS * KEYS_PER_PAD + MOTION_KEYS] =
  {
    DIK_LEFT,  DIK_RIGHT,
    DIK_UP,    DIK_DOWN,
    DIK_X,     DIK_Z,
    DIK_RETURN,DIK_BACK,
    DIK_A,     DIK_S,
    DIK_SPACE, DIK_F12,
    DIK_C,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
    DIK_NUMPAD4, DIK_NUMPAD6, DIK_NUMPAD8, DIK_NUMPAD2
};


void winReadKey(const char *name, KeyList& Keys)
{
  CString TxtKeyList = regQueryStringValue(name, "");
  int curPos= 0;

  CString resToken=TxtKeyList.Tokenize(",",curPos);
  while (resToken != "")
  {
	  Keys.AddTail(atoi(resToken));
	  resToken= TxtKeyList.Tokenize(",",curPos);
  };
}

void winReadKey(const char *name, int num, KeyList& Keys)
{
  char buffer[80];

  sprintf(buffer, "Joy%d_%s", num, name);
  winReadKey(buffer, Keys);
}


void winReadKeys()
{

  for(int i = 0; i < JOYPADS; i++) {
    winReadKey("Left", i, theApp.input->joypaddata[JOYPAD(i,KEY_LEFT)]);
    winReadKey("Right", i, theApp.input->joypaddata[JOYPAD(i, KEY_RIGHT)]);
    winReadKey("Up", i, theApp.input->joypaddata[JOYPAD(i,KEY_UP)]);
    winReadKey("Down", i, theApp.input->joypaddata[JOYPAD(i,KEY_DOWN)]);
    winReadKey("A", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_A)]);
    winReadKey("B", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_B)]);
    winReadKey("L", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_L)]);
    winReadKey("R", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_R)]);
    winReadKey("Start", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_START)]);
    winReadKey("Select", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_SELECT)]);
    winReadKey("Speed", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_SPEED)]);
    winReadKey("Capture", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_CAPTURE)]);
    winReadKey("GS", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_GS)]);
  }
  winReadKey("Motion_Left", theApp.input->joypaddata[MOTION(KEY_LEFT)]);
  winReadKey("Motion_Right", theApp.input->joypaddata[MOTION(KEY_RIGHT)]);
  winReadKey("Motion_Up", theApp.input->joypaddata[MOTION(KEY_UP)]);
  winReadKey("Motion_Down", theApp.input->joypaddata[MOTION(KEY_DOWN)]);
}

void winSaveKey(char *name, KeyList& value)
{
	CString txtKeys;

	POSITION p = value.GetHeadPosition();
	while(p!=NULL)
	{
		CString tmp;
		tmp.Format("%d", value.GetNext(p));
		txtKeys+=tmp;
		if (p!=NULL)
			txtKeys+=",";
	}
	regSetStringValue(name, txtKeys);
}

static void winSaveKey(char *name, int num, KeyList& value)
{
  char buffer[80];

  sprintf(buffer, "Joy%d_%s", num, name);
  winSaveKey(buffer, value);
}

void winSaveKeys()
{
  for(int i = 0; i < JOYPADS; i++) {
    winSaveKey("Left", i, theApp.input->joypaddata[JOYPAD(i,KEY_LEFT)]);
    winSaveKey("Right", i, theApp.input->joypaddata[JOYPAD(i,KEY_RIGHT)]);
    winSaveKey("Up", i, theApp.input->joypaddata[JOYPAD(i,KEY_UP)]);
    winSaveKey("Speed", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_SPEED)]);
    winSaveKey("Capture", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_CAPTURE)]);
    winSaveKey("GS", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_GS)]);
    winSaveKey("Down", i, theApp.input->joypaddata[JOYPAD(i,KEY_DOWN)]);
    winSaveKey("A", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_A)]);
    winSaveKey("B", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_B)]);
    winSaveKey("L", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_L)]);
    winSaveKey("R", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_R)]);
    winSaveKey("Start", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_START)]);
    winSaveKey("Select", i, theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_SELECT)]);
  }
  regSetDwordValue("joyVersion", 1);

  winSaveKey("Motion_Left",
                   theApp.input->joypaddata[MOTION(KEY_LEFT)]);
  winSaveKey("Motion_Right",
                   theApp.input->joypaddata[MOTION(KEY_RIGHT)]);
  winSaveKey("Motion_Up",
                   theApp.input->joypaddata[MOTION(KEY_UP)]);
  winSaveKey("Motion_Down",
                   theApp.input->joypaddata[MOTION(KEY_DOWN)]);
}

static BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                       VOID* pContext )
{
    DIPROPRANGE diprg;
    diprg.diph.dwSize       = sizeof(DIPROPRANGE);
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    diprg.diph.dwHow        = DIPH_BYOFFSET;
    diprg.diph.dwObj        = pdidoi->dwOfs; // Specify the enumerated axis

    diprg.lMin = -32768;
    diprg.lMax = 32767;
    // try to set the range
    if (FAILED(currentDevice->device->SetProperty(DIPROP_RANGE, &diprg.diph))) {
        // Get the range for the axis
        if ( FAILED(currentDevice->device->
                    GetProperty( DIPROP_RANGE, &diprg.diph ) ) ) {
            return DIENUM_STOP;
        }
    }

    DIPROPDWORD didz;

    didz.diph.dwSize = sizeof(didz);
    didz.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    didz.diph.dwHow = DIPH_BYOFFSET;
    didz.diph.dwObj = pdidoi->dwOfs;

    didz.dwData = 5000;

    currentDevice->device->SetProperty(DIPROP_DEADZONE, &didz.diph);

    LONG center = (diprg.lMin + diprg.lMax)/2;
    LONG threshold = (diprg.lMax - center)/2;

    // only 8 axis supported
    if (axisNumber < 8) {
        currentDevice->axis[axisNumber].center = center;
        currentDevice->axis[axisNumber].negative = center - threshold;
        currentDevice->axis[axisNumber].positive = center + threshold;
        currentDevice->axis[axisNumber].offset = pdidoi->dwOfs;
    }
    axisNumber++;
    return DIENUM_CONTINUE;
}

static BOOL CALLBACK EnumPovsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                       VOID* pContext )
{
    return DIENUM_CONTINUE;
}

static BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE pInst,
        LPVOID lpvContext)
{
    ZeroMemory(&pDevices[numDevices],sizeof(deviceInfo));

    HRESULT hRet = pDirectInput->CreateDevice(pInst->guidInstance,
                   &pDevices[numDevices].device,
                   NULL);

    if (hRet != DI_OK)
        return DIENUM_STOP;

    DIDEVCAPS caps;
    caps.dwSize=sizeof(DIDEVCAPS);

    hRet = pDevices[numDevices].device->GetCapabilities(&caps);

    if (hRet == DI_OK) {
        if (caps.dwFlags & DIDC_POLLEDDATAFORMAT ||
                caps.dwFlags & DIDC_POLLEDDEVICE)
            pDevices[numDevices].isPolled = TRUE;

        pDevices[numDevices].nButtons = caps.dwButtons;
        pDevices[numDevices].nAxes = caps.dwAxes;
        pDevices[numDevices].nPovs = caps.dwPOVs;

        for (int i = 0; i < 6; i++) {
            pDevices[numDevices].axis[i].center = 0x8000;
            pDevices[numDevices].axis[i].negative = 0x4000;
            pDevices[numDevices].axis[i].positive = 0xc000;
        }
    }


    numDevices++;


    return DIENUM_CONTINUE;
}

BOOL CALLBACK DIEnumDevicesCallback2(LPCDIDEVICEINSTANCE pInst,
                                     LPVOID lpvContext)
{
    numDevices++;

    return DIENUM_CONTINUE;
}

static int getPovState(DWORD value)
{
    int state = 0;
    if (LOWORD(value) != 0xFFFF) {
        if (value < 9000 || value > 27000)
            state |= POV_UP;
        if (value > 0 && value < 18000)
            state |= POV_RIGHT;
        if (value > 9000 && value < 27000)
            state |= POV_DOWN;
        if (value > 18000)
            state |= POV_LEFT;
    }
    return state;
}

static void checkKeys()
{
  LONG_PTR dev = 0;
  int i;

  for(i = 0; i < (sizeof(theApp.input->joypaddata) / sizeof(theApp.input->joypaddata[0])); i++)
  {
	  if (theApp.input->joypaddata[i].IsEmpty() && defvalues[i])
		  theApp.input->joypaddata[i].AddTail(defvalues[i]);
	  POSITION p = theApp.input->joypaddata[i].GetHeadPosition();
	  while(p!=NULL)
	  {
		  LONG_PTR k = theApp.input->joypaddata[i].GetNext(p);
		  if (k > 0 && DEVICEOF(k) < numDevices)
			  pDevices[DEVICEOF(k)].needed = true;
	  }
  }
}

#define KEYDOWN(buffer,key) (buffer[key] & 0x80)

static bool readKeyboard()
{
    if (pDevices[0].needed) {
        HRESULT hret = pDevices[0].device->
                       GetDeviceState(256,
                                      (LPVOID)pDevices[0].data);

        if (hret == DIERR_INPUTLOST || hret == DIERR_NOTACQUIRED) {
            hret = pDevices[0].device->Acquire();
            if (hret != DI_OK)
                return false;
            hret = pDevices[0].device->GetDeviceState(256,(LPVOID)pDevices[0].data);
        }

        return hret == DI_OK;
    }
    return true;
}

static bool readJoystick(int joy)
{
    if (pDevices[joy].needed) {
        if (pDevices[joy].isPolled)
            ((LPDIRECTINPUTDEVICE2)pDevices[joy].device)->Poll();

        HRESULT hret = pDevices[joy].device->
                       GetDeviceState(sizeof(DIJOYSTATE),
                                      (LPVOID)&pDevices[joy].state);

        if (hret == DIERR_INPUTLOST || hret == DIERR_NOTACQUIRED) {
            hret = pDevices[joy].device->Acquire();

            if (hret == DI_OK) {

                if (pDevices[joy].isPolled)
                    ((LPDIRECTINPUTDEVICE2)pDevices[joy].device)->Poll();

                hret = pDevices[joy].device->
                       GetDeviceState(sizeof(DIJOYSTATE),
                                      (LPVOID)&pDevices[joy].state);
            }
        }

        return hret == DI_OK;
    }

    return true;
}

static void checkKeyboard()
{
    // mham fix. Patch #1378104
    UCHAR keystate[256];
    HRESULT hret = pDevices[0].device->Acquire();

    if (pDevices[0].first) {
        pDevices[0].device->GetDeviceState(256, (LPVOID)pDevices[0].data);
        pDevices[0].first = FALSE;
        return;
    }

    hret = pDevices[0].device->
           GetDeviceState(256, (LPVOID)keystate);

    if (hret == DIERR_INPUTLOST || hret == DIERR_NOTACQUIRED) {
        return;
    }

    if (hret == DI_OK) {
        for (int i = 0; i < 256; i++) {
            if (keystate[i] == pDevices[0].data[i]) continue;
            if (KEYDOWN(keystate, i)) {
                SendMessage(GetFocus(), JOYCONFIG_MESSAGE,0,i);
                break;
            }
        }
    }
    memcpy(pDevices[0].data, keystate, sizeof(UCHAR) * 256);
}

static void checkJoypads()
{
    DIDEVICEOBJECTINSTANCE di;

    ZeroMemory(&di,sizeof(DIDEVICEOBJECTINSTANCE));

    di.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

    int i =0;

    DIJOYSTATE joystick;

    for (i = 1; i < numDevices; i++) {
        HRESULT hret = pDevices[i].device->Acquire();


        if (pDevices[i].isPolled)
            ((LPDIRECTINPUTDEVICE2)pDevices[i].device)->Poll();

        hret = pDevices[i].device->GetDeviceState(sizeof(joystick), &joystick);

        int j;

        if (pDevices[i].first) {
            memcpy(&pDevices[i].state, &joystick, sizeof(joystick));
            pDevices[i].first = FALSE;
            continue;
        }

        for (j = 0; j < pDevices[i].nButtons; j++) {
            if (((pDevices[i].state.rgbButtons[j] ^ joystick.rgbButtons[j])
                    & joystick.rgbButtons[j]) & 0x80) {
                HWND focus = GetFocus();

                SendMessage(focus, JOYCONFIG_MESSAGE, i,j+128);
            }
        }

        for (j = 0; j < pDevices[i].nAxes && j < 8; j++) {
            LONG value = pDevices[i].axis[j].center;
            LONG old = 0;
            switch (pDevices[i].axis[j].offset) {
            case DIJOFS_X:
                value = joystick.lX;
                old = pDevices[i].state.lX;
                break;
            case DIJOFS_Y:
                value = joystick.lY;
                old = pDevices[i].state.lY;
                break;
            case DIJOFS_Z:
                value = joystick.lZ;
                old = pDevices[i].state.lZ;
                break;
            case DIJOFS_RX:
                value = joystick.lRx;
                old = pDevices[i].state.lRx;
                break;
            case DIJOFS_RY:
                value = joystick.lRy;
                old = pDevices[i].state.lRy;
                break;
            case DIJOFS_RZ:
                value = joystick.lRz;
                old = pDevices[i].state.lRz;
                break;
            case DIJOFS_SLIDER(0):
                            value = joystick.rglSlider[0];
                old = pDevices[i].state.rglSlider[0];
                break;
            case DIJOFS_SLIDER(1):
                            value = joystick.rglSlider[1];
                old = pDevices[i].state.rglSlider[1];
                break;
            }
            if (value != old) {
                if (value < pDevices[i].axis[j].negative)
                    SendMessage(GetFocus(), JOYCONFIG_MESSAGE, i, (j<<1));
                else if (value > pDevices[i].axis[j].positive)
                    SendMessage(GetFocus(), JOYCONFIG_MESSAGE, i, (j<<1)+1);
            }
        }

        for (j = 0;j < 4 && j < pDevices[i].nPovs; j++) {
            if (LOWORD(pDevices[i].state.rgdwPOV[j]) != LOWORD(joystick.rgdwPOV[j])) {
                int state = getPovState(joystick.rgdwPOV[j]);

                if (state & POV_UP)
                    SendMessage(GetFocus(), JOYCONFIG_MESSAGE, i, (j<<2)+0x20);
                else if (state & POV_DOWN)
                    SendMessage(GetFocus(), JOYCONFIG_MESSAGE, i, (j<<2)+0x21);
                else if (state & POV_RIGHT)
                    SendMessage(GetFocus(), JOYCONFIG_MESSAGE, i, (j<<2)+0x22);
                else if (state & POV_LEFT)
                    SendMessage(GetFocus(), JOYCONFIG_MESSAGE, i, (j<<2)+0x23);
            }
        }

        memcpy(&pDevices[i].state, &joystick, sizeof(joystick));
    }
}

BOOL checkKey(LONG_PTR key)
{
    LONG_PTR dev = (key >> 8);

    LONG_PTR k = (key & 255);

    if (dev == 0) {
        return KEYDOWN(pDevices[0].data,k);
	} else if (dev >= numDevices) {
		return FALSE;
	} else {
        if (k < 16) {
            LONG_PTR axis = k >> 1;
            LONG value = pDevices[dev].axis[axis].center;
            switch (pDevices[dev].axis[axis].offset) {
            case DIJOFS_X:
                value = pDevices[dev].state.lX;
                break;
            case DIJOFS_Y:
                value = pDevices[dev].state.lY;
                break;
            case DIJOFS_Z:
                value = pDevices[dev].state.lZ;
                break;
            case DIJOFS_RX:
                value = pDevices[dev].state.lRx;
                break;
            case DIJOFS_RY:
                value = pDevices[dev].state.lRy;
                break;
            case DIJOFS_RZ:
                value = pDevices[dev].state.lRz;
                break;
            case DIJOFS_SLIDER(0):
                            value = pDevices[dev].state.rglSlider[0];
                break;
            case DIJOFS_SLIDER(1):
                            value = pDevices[dev].state.rglSlider[1];
                break;
            }

            if (k & 1)
                return value > pDevices[dev].axis[axis].positive;
            return value < pDevices[dev].axis[axis].negative;
        } else if (k < 48) {
            LONG_PTR hat = (k >> 2) & 3;
            int state = getPovState(pDevices[dev].state.rgdwPOV[hat]);
            BOOL res = FALSE;
            switch (k & 3) {
            case 0:
                res = state & POV_UP;
                break;
            case 1:
                res = state & POV_DOWN;
                break;
            case 2:
                res = state & POV_RIGHT;
                break;
            case 3:
                res = state & POV_LEFT;
                break;
            }
            return res;
        } else if (k  >= 128) {
            return pDevices[dev].state.rgbButtons[k-128] & 0x80;
        }
    }

    return FALSE;
}

BOOL checkKey(KeyList &k)
{
	POSITION p = k.GetHeadPosition();
	while(p!=NULL)
	{
		if (checkKey(k.GetNext(p)))
			return TRUE;
	}
	return FALSE;
}

DirectInput::DirectInput()
{
}

DirectInput::~DirectInput()
{
    saveSettings();
    if (pDirectInput != NULL) {
        if (pDevices) {
            for (int i = 0; i < numDevices ; i++) {
                if (pDevices[i].device) {
                    pDevices[i].device->Unacquire();
                    pDevices[i].device->Release();
                    pDevices[i].device = NULL;
                }
            }
            free(pDevices);
            pDevices = NULL;
        }

        pDirectInput->Release();
        pDirectInput = NULL;
    }
}

bool DirectInput::initialize()
{
	HRESULT hr;
	
	hr = DirectInput8Create(
		GetModuleHandle( NULL ),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(LPVOID *)&pDirectInput,
		NULL );
	ASSERT( hr == DI_OK );
	if( hr != DI_OK ) return false;


	hr = pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                   DIEnumDevicesCallback2,
                                   NULL,
                                   DIEDFL_ATTACHEDONLY);



    pDevices = (deviceInfo *)calloc(numDevices, sizeof(deviceInfo));

    hr = pDirectInput->CreateDevice(GUID_SysKeyboard,&pDevices[0].device,NULL);
    pDevices[0].isPolled = false;
    pDevices[0].needed  = true;
    pDevices[0].first  = true;

    if (hr != DI_OK) {
        return false;
    }


    numDevices = 1;

    hr = pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                   DIEnumDevicesCallback,
                                   NULL,
                                   DIEDFL_ATTACHEDONLY);


    if (hr != DI_OK) {
        return false;
    }

    hr = pDevices[0].device->SetDataFormat(&c_dfDIKeyboard);

    if (hr != DI_OK) {
        return false;
    }

    int i;
    for (i = 1; i < numDevices; i++) {
        pDevices[i].device->SetDataFormat(&c_dfDIJoystick);
        pDevices[i].needed = false;
        pDevices[i].first  = true;
        currentDevice = &pDevices[i];
        axisNumber = 0;
        currentDevice->device->EnumObjects(EnumAxesCallback, NULL, DIDFT_AXIS);
        currentDevice->device->EnumObjects(EnumPovsCallback, NULL, DIDFT_POV);


        currentDevice = NULL;
    }

    for (i = 0; i < numDevices; i++)
        pDevices[i].device->Acquire();

    return true;
}

bool DirectInput::readDevices()
{
    bool ok = true;
    for (int i = 0; i < numDevices; i++) {
        if (pDevices[i].needed) {
            if (i) {
                ok = readJoystick(i);
            } else
                ok = readKeyboard();
        }
    }
    return ok;
}

u32 DirectInput::readDevice(int which)
{
  u32 res = 0;
  int i = theApp.joypadDefault;
  if(which >= 0 && which <= 3)
    i = which;

  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_A)]))
    res |= 1;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_B)]))
    res |= 2;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_SELECT)]))
    res |= 4;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_START)]))
    res |= 8;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_RIGHT)]))
    res |= 16;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_LEFT)]))
    res |= 32;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_UP)]))
    res |= 64;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_DOWN)]))
    res |= 128;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_R)]))
    res |= 256;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_L)]))
    res |= 512;

  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_GS)]))
    res |= 4096;

  if(theApp.autoFire) {
    res &= (~theApp.autoFire);
    if(theApp.autoFireToggle)
      res |= theApp.autoFire;
    theApp.autoFireToggle = !theApp.autoFireToggle;
  }

  // disallow L+R or U+D of being pressed at the same time
  if((res & 48) == 48)
    res &= ~16;
  if((res & 192) == 192)
    res &= ~128;

  if(theApp.movieRecording) {
    if(i == theApp.joypadDefault) {
      if(res != theApp.movieLastJoypad) {
        fwrite(&theApp.movieFrame, 1, sizeof(theApp.movieFrame), theApp.movieFile);
        fwrite(&res, 1, sizeof(res), theApp.movieFile);
        theApp.movieLastJoypad = res;
      }
    }
  }
  if(theApp.moviePlaying) {
    if(theApp.movieFrame == theApp.moviePlayFrame) {
      theApp.movieLastJoypad = theApp.movieNextJoypad;
      theApp.movieReadNext();
    }
    res = theApp.movieLastJoypad;
  }
  // we don't record speed up or screen capture buttons
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_SPEED)]) || theApp.speedupToggle)
    res |= 1024;
  if(checkKey(theApp.input->joypaddata[JOYPAD(i,KEY_BUTTON_CAPTURE)]))
    res |= 2048;

  return res;
}

CString DirectInput::getKeyName(LONG_PTR key)
{
    LONG_PTR d = (key >> 8);
    LONG_PTR k = key & 255;

    DIDEVICEOBJECTINSTANCE di;

    ZeroMemory(&di,sizeof(DIDEVICEOBJECTINSTANCE));

    di.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

    CString winBuffer = winResLoadString(IDS_ERROR);

    if (d == 0) {
        pDevices[0].device->GetObjectInfo( &di, (DWORD)key, DIPH_BYOFFSET );
        winBuffer = di.tszName;
	} else if (d < numDevices) {
        if (k < 16) {
            if (k < 4) {
                switch (k) {
                case 0:
                    winBuffer.Format(winResLoadString(IDS_JOY_LEFT), d);
                    break;
                case 1:
                    winBuffer.Format(winResLoadString(IDS_JOY_RIGHT), d);
                    break;
                case 2:
                    winBuffer.Format(winResLoadString(IDS_JOY_UP), d);
                    break;
                case 3:
                    winBuffer.Format(winResLoadString(IDS_JOY_DOWN), d);
                    break;
                }
            } else {
                pDevices[d].device->GetObjectInfo(&di,
                                                  pDevices[d].axis[k>>1].offset,
                                                  DIPH_BYOFFSET);
                if (k & 1)
                    winBuffer.Format("Joy %d %s +", d, di.tszName);
                else
                    winBuffer.Format("Joy %d %s -", d, di.tszName);
            }
        } else if (k < 48) {
            LONG_PTR hat = (k >> 2) & 3;
            pDevices[d].device->GetObjectInfo(&di,
                                              (DWORD)DIJOFS_POV(hat),
                                              DIPH_BYOFFSET);
            char *dir = "up";
            LONG_PTR dd = k & 3;
            if (dd == 1)
                dir = "down";
            else if (dd == 2)
                dir = "right";
            else if (dd == 3)
                dir = "left";
            winBuffer.Format("Joy %d %s %s", d, di.tszName, dir);
        } else {
            pDevices[d].device->GetObjectInfo(&di,
                                              (DWORD)DIJOFS_BUTTON(k-128),
                                              DIPH_BYOFFSET);
            winBuffer.Format(winResLoadString(IDS_JOY_BUTTON),d,di.tszName);
        }
    }
	else
	{
		// Joystick isn't plugged in.  We can't decipher k, so just show its value.
		winBuffer.Format("Joy %d (%d)", d, k);
	}

    return winBuffer;
}

void DirectInput::checkKeys()
{
    ::checkKeys();
}

void DirectInput::checkMotionKeys()
{
  if(checkKey(theApp.input->joypaddata[MOTION(KEY_LEFT)])) {
    theApp.sensorX += 3;
    if(theApp.sensorX > 2197)
      theApp.sensorX = 2197;
    if(theApp.sensorX < 2047)
      theApp.sensorX = 2057;
  } else if(checkKey(theApp.input->joypaddata[MOTION(KEY_RIGHT)])) {
    theApp.sensorX -= 3;
    if(theApp.sensorX < 1897)
      theApp.sensorX = 1897;
    if(theApp.sensorX > 2047)
      theApp.sensorX = 2037;
  } else if(theApp.sensorX > 2047) {
    theApp.sensorX -= 2;
    if(theApp.sensorX < 2047)
      theApp.sensorX = 2047;
  } else {
    theApp.sensorX += 2;
    if(theApp.sensorX > 2047)
      theApp.sensorX = 2047;
  }

  if(checkKey(theApp.input->joypaddata[MOTION(KEY_UP)])) {
    theApp.sensorY += 3;
    if(theApp.sensorY > 2197)
      theApp.sensorY = 2197;
    if(theApp.sensorY < 2047)
      theApp.sensorY = 2057;
  } else if(checkKey(theApp.input->joypaddata[MOTION(KEY_DOWN)])) {
    theApp.sensorY -= 3;
    if(theApp.sensorY < 1897)
      theApp.sensorY = 1897;
    if(theApp.sensorY > 2047)
      theApp.sensorY = 2037;
  } else if(theApp.sensorY > 2047) {
    theApp.sensorY -= 2;
    if(theApp.sensorY < 2047)
      theApp.sensorY = 2047;
  } else {
    theApp.sensorY += 2;
    if(theApp.sensorY > 2047)
      theApp.sensorY = 2047;
  }
}

Input *newDirectInput()
{
    return new DirectInput;
}


void DirectInput::checkDevices()
{
    checkJoypads();
    checkKeyboard();
}

void DirectInput::activate()
{
    for (int i = 0; i < numDevices; i++) {
        if (pDevices != NULL && pDevices[i].device != NULL)
            pDevices[i].device->Acquire();
    }
}

void DirectInput::loadSettings()
{
    winReadKeys();
}

void DirectInput::saveSettings()
{
    winSaveKeys();
}
