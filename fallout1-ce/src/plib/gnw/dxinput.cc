#include "plib/gnw/dxinput.h"

namespace fallout {

static bool dxinput_mouse_init();
static void dxinput_mouse_exit();
static bool dxinput_keyboard_init();
static void dxinput_keyboard_exit();

static int gMouseWheelDeltaX = 0;
static int gMouseWheelDeltaY = 0;

#ifdef __PS3__
// DS3/DualSense -> mouse emulation:
//   LEFT stick = cursor (so the right thumb can stay on X while dragging),
//   X = left click, O = right click, R2 = speed up.
// PSL1GHT joystick driver: axes L=(0,1) R=(2,3), buttons in SDL_sysjoystick.c
#define PS3_AXIS_CURSOR_H 0
#define PS3_AXIS_CURSOR_V 1
#define PS3_BTN_CROSS 9
#define PS3_BTN_CIRCLE 10
#define PS3_BTN_R2 14
#define PS3_AXIS_DEADZONE 8000
// The axis value is divided by this to get the pixel delta (smaller = faster).
#define PS3_AXIS_DIVISOR 4096

static SDL_Joystick* gPs3Joystick = NULL;

static void ps3JoystickEnsureOpen()
{
    if (gPs3Joystick == NULL && SDL_NumJoysticks() > 0) {
        gPs3Joystick = SDL_JoystickOpen(0);
    }
}
#endif

// 0x4E0400
bool dxinput_init()
{
    if (SDL_InitSubSystem(SDL_INIT_EVENTS) != 0) {
        return false;
    }

#ifdef __PS3__
    // The engine never opens this subsystem because it does not use joysticks
    // on PC; without it SDL_NumJoysticks() always returns 0 and the
    // gamepad->mouse emulation (dxinput_get_mouse_state) never kicks in.
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
        printf("[fce] SDL joystick subsystem failed: %s\n", SDL_GetError());
    }
#endif

    if (!dxinput_mouse_init()) {
        goto err;
    }

    if (!dxinput_keyboard_init()) {
        goto err;
    }

    return true;

err:

    dxinput_mouse_exit();

    return false;
}

// 0x4E0478
void dxinput_exit()
{
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

// 0x4E04E8
bool dxinput_acquire_mouse()
{
    return true;
}

// 0x4E0514
bool dxinput_unacquire_mouse()
{
    return true;
}

// 0x4E053C
bool dxinput_get_mouse_state(MouseData* mouseState)
{
    // CE: This function is sometimes called outside loops calling `get_input`
    // and subsequently `GNW95_process_message`, so mouse events might not be
    // handled by SDL yet.
    //
    // TODO: Move mouse events processing into `GNW95_process_message` and
    // update mouse position manually.
    SDL_PumpEvents();

    Uint32 buttons = SDL_GetRelativeMouseState(&(mouseState->x), &(mouseState->y));
    mouseState->buttons[0] = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    mouseState->buttons[1] = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;

#ifdef __PS3__
    // Feed the gamepad into the engine as if it were a mouse. If a real USB
    // mouse is present, the SDL values above keep flowing too (free bonus).
    ps3JoystickEnsureOpen();
    if (gPs3Joystick != NULL) {
        Sint16 ax = SDL_JoystickGetAxis(gPs3Joystick, PS3_AXIS_CURSOR_H);
        Sint16 ay = SDL_JoystickGetAxis(gPs3Joystick, PS3_AXIS_CURSOR_V);
        if (ax > -PS3_AXIS_DEADZONE && ax < PS3_AXIS_DEADZONE) ax = 0;
        if (ay > -PS3_AXIS_DEADZONE && ay < PS3_AXIS_DEADZONE) ay = 0;

        int divisor = PS3_AXIS_DIVISOR;
        if (SDL_JoystickGetButton(gPs3Joystick, PS3_BTN_R2)) {
            divisor /= 3;
        }

        mouseState->x += ax / divisor;
        mouseState->y += ay / divisor;

        if (SDL_JoystickGetButton(gPs3Joystick, PS3_BTN_CROSS)) {
            mouseState->buttons[0] = true;
        }
        if (SDL_JoystickGetButton(gPs3Joystick, PS3_BTN_CIRCLE)) {
            mouseState->buttons[1] = true;
        }
    }
#endif

    mouseState->wheelX = gMouseWheelDeltaX;
    mouseState->wheelY = gMouseWheelDeltaY;

    gMouseWheelDeltaX = 0;
    gMouseWheelDeltaY = 0;

    return true;
}

// 0x4E05A8
bool dxinput_acquire_keyboard()
{
    return true;
}

// 0x4E05D4
bool dxinput_unacquire_keyboard()
{
    return true;
}

// 0x4E05FC
bool dxinput_flush_keyboard_buffer()
{
    SDL_FlushEvents(SDL_KEYDOWN, SDL_TEXTINPUT);
    return true;
}

// 0x4E0650
bool dxinput_read_keyboard_buffer(KeyboardData* keyboardData)
{
    return true;
}

// 0x4E070C
bool dxinput_mouse_init()
{
#ifdef __PS3__
    // Relative mouse mode is unsupported on PSL1GHT; not treated as fatal.
    // The cursor is driven by the analog stick (dxinput_get_mouse_state).
    SDL_SetRelativeMouseMode(SDL_TRUE);
    return true;
#else
    return SDL_SetRelativeMouseMode(SDL_TRUE) == 0;
#endif
}

// 0x4E078C
void dxinput_mouse_exit()
{
}

// 0x4E07B8
bool dxinput_keyboard_init()
{
    return true;
}

// 0x4E0874
void dxinput_keyboard_exit()
{
}

#ifdef __PS3__
void dxinput_ps3_right_stick(Sint16* rx, Sint16* ry)
{
    ps3JoystickEnsureOpen();
    if (gPs3Joystick != NULL) {
        *rx = SDL_JoystickGetAxis(gPs3Joystick, 2);
        *ry = SDL_JoystickGetAxis(gPs3Joystick, 3);
    } else {
        *rx = 0;
        *ry = 0;
    }
}
#endif

void handleMouseEvent(SDL_Event* event)
{
    // Mouse movement and buttons are accumulated in SDL itself and will be
    // processed later in `mouseDeviceGetData` via `SDL_GetRelativeMouseState`.

    if (event->type == SDL_MOUSEWHEEL) {
        gMouseWheelDeltaX += event->wheel.x;
        gMouseWheelDeltaY += event->wheel.y;
    }
}

} // namespace fallout
