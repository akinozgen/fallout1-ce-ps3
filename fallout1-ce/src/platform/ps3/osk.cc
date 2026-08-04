// PS3 sistem ekran klavyesi (sysutil OSK) entegrasyonu.
//
// Akis (PSL1GHT osk ornegiyle ayni iskelet):
//   sysMemContainerCreate -> sysUtilRegisterCallback -> oskLoadAsync
//   -> [her frame: sysUtilCheckCallback + ekrani tazele] -> DONE/CANCELED
//   -> oskUnloadAsync -> temizlik.
// Dialog acikken ekran tazelenmeye devam etmeli (flip zinciri) — yoksa
// OSK gorunmez; bu yuzden dongude win_refresh_all cagrilir.

#include "platform/ps3/osk.h"

#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include <sys/memory.h>
#include <sysutil/osk.h>
#include <sysutil/sysutil.h>

#include "plib/gnw/gnw.h"
#include "plib/gnw/svga.h"

namespace fallout {

#define PS3_OSK_MAX_LEN 64
#define PS3_OSK_MEM_CONTAINER_SIZE (4 * 1024 * 1024)

enum Ps3OskState {
    PS3_OSK_RUNNING,
    PS3_OSK_DONE,
    PS3_OSK_CANCELED
};

static volatile int s_oskState = PS3_OSK_CANCELED;
static u16 s_resultText[PS3_OSK_MAX_LEN + 1];
static oskCallbackReturnParam s_returnParam;

static void ascii_to_utf16(u16* dst, const char* src, int maxLength)
{
    int i;
    for (i = 0; i < maxLength && src[i] != '\0'; i++) {
        dst[i] = (u16)(unsigned char)src[i];
    }
    dst[i] = 0;
}

static void utf16_to_ascii(char* dst, const u16* src, int maxLength)
{
    int i;
    for (i = 0; i < maxLength && src[i] != 0; i++) {
        dst[i] = (src[i] < 128) ? (char)src[i] : '?';
    }
    dst[i] = '\0';
}

static void ps3_osk_event_handler(u64 status, u64 param, void* userdata)
{
    switch (status) {
    case SYSUTIL_OSK_INPUT_ENTERED:
        oskGetInputText(&s_returnParam);
        break;
    case SYSUTIL_OSK_INPUT_CANCELED:
        oskAbort();
        break;
    case SYSUTIL_OSK_DONE:
        oskUnloadAsync(&s_returnParam);
        s_oskState = (s_returnParam.res == OSK_OK) ? PS3_OSK_DONE : PS3_OSK_CANCELED;
        break;
    default:
        break;
    }
}

bool ps3_osk_get_text(const char* title, const char* initial, char* out, int maxLength)
{
    out[0] = '\0';

    if (maxLength > PS3_OSK_MAX_LEN) {
        maxLength = PS3_OSK_MAX_LEN;
    }

    sys_mem_container_t container;
    if (sysMemContainerCreate(&container, PS3_OSK_MEM_CONTAINER_SIZE) != 0) {
        printf("[fce] osk: sysMemContainerCreate basarisiz\n");
        return false;
    }

    static u16 message[64];
    static u16 startText[PS3_OSK_MAX_LEN + 1];
    ascii_to_utf16(message, title, 63);
    ascii_to_utf16(startText, initial != NULL ? initial : "", PS3_OSK_MAX_LEN);

    oskParam param;
    memset(&param, 0, sizeof(param));
    param.allowedPanels = OSK_PANEL_TYPE_LATIN | OSK_PANEL_TYPE_ENGLISH | OSK_PANEL_TYPE_NUMERAL;
    param.firstViewPanel = OSK_PANEL_TYPE_LATIN;
    param.controlPoint.x = 0.0f;
    param.controlPoint.y = 0.0f;
    param.prohibitFlags = OSK_PROHIBIT_RETURN;

    oskInputFieldInfo field;
    memset(&field, 0, sizeof(field));
    field.message = message;
    field.startText = startText;
    field.maxLength = maxLength;

    memset(s_resultText, 0, sizeof(s_resultText));
    s_returnParam.res = OSK_CANCELED;
    s_returnParam.len = maxLength;
    s_returnParam.str = s_resultText;
    s_oskState = PS3_OSK_RUNNING;

    if (sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT1, ps3_osk_event_handler, NULL) != 0) {
        printf("[fce] osk: sysUtilRegisterCallback basarisiz\n");
        sysMemContainerDestroy(container);
        return false;
    }

    oskSetInitialInputDevice(OSK_DEVICE_PAD);

    if (oskLoadAsync(container, &param, &field) != 0) {
        printf("[fce] osk: oskLoadAsync basarisiz\n");
        sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT1);
        sysMemContainerDestroy(container);
        return false;
    }

    while (s_oskState == PS3_OSK_RUNNING) {
        sysUtilCheckCallback();
        // Dialog altinda flip zinciri kopmasin diye ekrani tazele.
        win_refresh_all(&scr_size);
        SDL_Delay(16);
    }

    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT1);
    sysMemContainerDestroy(container);

    if (s_oskState != PS3_OSK_DONE) {
        return false;
    }

    utf16_to_ascii(out, s_resultText, maxLength);
    return out[0] != '\0';
}

} // namespace fallout
