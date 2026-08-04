#ifndef FALLOUT_PLATFORM_PS3_OSK_H_
#define FALLOUT_PLATFORM_PS3_OSK_H_

namespace fallout {

// Opens the PS3 system on-screen keyboard (OSK) modally.
// initial: current contents of the field (OSK opens prefilled; NULL/"" = empty).
// If the user confirms, ASCII text is written to out and true is returned;
// false on cancel/error. maxLength is the character limit excluding the NUL.
bool ps3_osk_get_text(const char* title, const char* initial, char* out, int maxLength);

} // namespace fallout

#endif /* FALLOUT_PLATFORM_PS3_OSK_H_ */
