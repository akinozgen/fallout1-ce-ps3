#ifndef FALLOUT_PLATFORM_PS3_PATHS_H_
#define FALLOUT_PLATFORM_PS3_PATHS_H_

// Fixed location of the game data on PS3 - path resolution is driven ONLY here.
//
// The player copies these files here from their own legal copy:
//   MASTER.DAT, CRITTER.DAT, DATA/, fallout.cfg (optional)
//
// Real PS3 : /dev_hdd0/game/FALL1CE00/USRDIR/
// RPCS3    : <rpcs3>/dev_hdd0/game/FALL1CE00/USRDIR/
//
// FALL1CE00 must match the .pkg TITLE_ID, so that on pkg install EBOOT.BIN and
// the data end up in the same folder. Savegames are written here as well.
#define PS3_GAME_DATA_PATH "/dev_hdd0/game/FALL1CE00/USRDIR"

#endif /* FALLOUT_PLATFORM_PS3_PATHS_H_ */
