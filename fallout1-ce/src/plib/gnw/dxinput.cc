#include "plib/gnw/dxinput.h"

namespace fallout {

static bool dxinput_mouse_init();
static void dxinput_mouse_exit();
static bool dxinput_keyboard_init();
static void dxinput_keyboard_exit();

static int gMouseWheelDeltaX = 0;
static int gMouseWheelDeltaY = 0;

#ifdef __PS3__
// DS3/DualSense -> fare emulasyonu:
//   SOL analog = imlec (surukle-birakta sag basparmak X'te kalabilsin diye),
//   X = sol tik, O = sag tik, R2 = hizlandir.
// PSL1GHT joystick surucusu: eksenler L=(0,1) R=(2,3), butonlar SDL_sysjoystick.c
#define PS3_AXIS_CURSOR_H 0
#define PS3_AXIS_CURSOR_V 1
#define PS3_BTN_CROSS 9
#define PS3_BTN_CIRCLE 10
#define PS3_BTN_R2 14
#define PS3_AXIS_DEADZONE 8000
// Eksen degeri bu bolenle piksel deltasina iner (kucuk = hizli imlec).
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
    // Motor PC'de joystick kullanmadigi icin bu alt sistemi hic acmiyor;
    // acilmazsa SDL_NumJoysticks() hep 0 doner ve gamepad->fare emulasyonu
    // (dxinput_get_mouse_state) asla devreye girmez.
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
        printf("[fce] SDL joystick subsystem BASARISIZ: %s\n", SDL_GetError());
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
    // Gamepad'i fare gibi motorun damarina bagla. Gercek USB fare varsa
    // yukaridaki SDL degerleri de akmaya devam eder (bedava bonus).
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
    // PSL1GHT'ta relative mouse mode desteklenmez; fatal sayilmaz.
    // Imlec analog cubuktan surulur (dxinput_get_mouse_state).
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
