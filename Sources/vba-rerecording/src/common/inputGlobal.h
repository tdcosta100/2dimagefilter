#ifndef VBA_INPUT_GLOBAL_H
#define VBA_INPUT_GLOBAL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum
{
	KEY_LEFT, KEY_RIGHT,
	KEY_UP, KEY_DOWN,
	KEY_BUTTON_A, KEY_BUTTON_B,
	KEY_BUTTON_START, KEY_BUTTON_SELECT,
	KEY_BUTTON_L, KEY_BUTTON_R,
	KEY_BUTTON_SPEED, KEY_BUTTON_CAPTURE,
	KEY_BUTTON_GS
};

#define BUTTON_MASK_A                    (0x0001)
#define BUTTON_MASK_B                    (0x0002)
#define BUTTON_MASK_SELECT               (0x0004)
#define BUTTON_MASK_START                (0x0008)
#define BUTTON_MASK_RIGHT                (0x0010)
#define BUTTON_MASK_LEFT                 (0x0020)
#define BUTTON_MASK_UP                   (0x0040)
#define BUTTON_MASK_DOWN                 (0x0080)
#define BUTTON_MASK_R                    (0x0100)
#define BUTTON_MASK_L                    (0x0200)
#define BUTTON_REGULAR_MASK              (BUTTON_MASK_A|BUTTON_MASK_B|BUTTON_MASK_SELECT|BUTTON_MASK_START| \
                                          BUTTON_MASK_RIGHT|BUTTON_MASK_LEFT|BUTTON_MASK_UP|BUTTON_MASK_DOWN|BUTTON_MASK_R| \
                                          BUTTON_MASK_L)
#define BUTTON_MASK_RESET                (0x0400)
#define BUTTON_MASK_UNUSED               (0x0800)
#define BUTTON_MASK_LEFT_MOTION          (0x1000)
#define BUTTON_MASK_RIGHT_MOTION         (0x2000)
#define BUTTON_MASK_DOWN_MOTION          (0x4000)
#define BUTTON_MASK_UP_MOTION            (0x8000)
#define BUTTON_MOTION_MASK               (BUTTON_MASK_LEFT_MOTION|BUTTON_MASK_RIGHT_MOTION|BUTTON_MASK_DOWN_MOTION| \
                                          BUTTON_MASK_UP_MOTION)
#define BUTTON_RECORDINGONLY_MASK        (BUTTON_MASK_RESET|BUTTON_MASK_UNUSED|BUTTON_MASK_LEFT_MOTION| \
                                          BUTTON_MASK_RIGHT_MOTION|BUTTON_MASK_RIGHT_MOTION|BUTTON_MASK_DOWN_MOTION| \
                                          BUTTON_MASK_UP_MOTION)
#define BUTTON_MASK_SPEED                (0x0400)
#define BUTTON_MASK_CAPTURE              (0x0800)
#define BUTTON_MASK_GAMESHARK            (0x1000)
#define BUTTON_NONRECORDINGONLY_MASK     (BUTTON_MASK_SPEED|BUTTON_MASK_CAPTURE|BUTTON_MASK_GAMESHARK)

#endif // VBA_INPUT_GLOBAL_H
