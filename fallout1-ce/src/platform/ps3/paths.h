#ifndef FALLOUT_PLATFORM_PS3_PATHS_H_
#define FALLOUT_PLATFORM_PS3_PATHS_H_

// Oyun verisinin PS3'teki sabit konumu — yol cozumlemesi TEK buradan yonetilir.
//
// Oyuncu kendi yasal kopyasindan su dosyalari buraya koyar:
//   MASTER.DAT, CRITTER.DAT, DATA/, fallout.cfg (opsiyonel)
//
// Gercek PS3 : /dev_hdd0/game/FALL1CE00/USRDIR/
// RPCS3      : <rpcs3>/dev_hdd0/game/FALL1CE00/USRDIR/
//
// FALL1CE00, .pkg TITLE_ID'si ile ayni tutulmalidir; boylece pkg kurulumunda
// EBOOT.BIN ile veri ayni klasorde bulusur. Savegame'ler de buraya yazilir.
#define PS3_GAME_DATA_PATH "/dev_hdd0/game/FALL1CE00/USRDIR"

#endif /* FALLOUT_PLATFORM_PS3_PATHS_H_ */
