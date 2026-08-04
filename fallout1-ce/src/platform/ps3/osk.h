#ifndef FALLOUT_PLATFORM_PS3_OSK_H_
#define FALLOUT_PLATFORM_PS3_OSK_H_

namespace fallout {

// PS3 sistem ekran klavyesini (OSK) modal acar.
// initial: alanin mevcut icerigi (OSK dolu acilir; NULL/"" = bos).
// Kullanici onaylarsa out'a ASCII metin yazilir ve true doner;
// iptal/hata durumunda false. maxLength NUL haric karakter sinirdir.
bool ps3_osk_get_text(const char* title, const char* initial, char* out, int maxLength);

} // namespace fallout

#endif /* FALLOUT_PLATFORM_PS3_OSK_H_ */
