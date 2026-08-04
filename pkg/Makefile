# Fallout 1 CE - PS3 installation package (.pkg)
#
# WARNING: if game data is placed into pkgfiles/, the package will include it;
# a package produced this way carries COPYRIGHTED content and must not be
# distributed, only used for local testing. For a distributable build leave
# pkgfiles/ empty.
#
# Usage (from the project root):
#   docker run --rm -v "$PWD":/src -w /src/pkg ps3dev make
# Outputs: ../build/fallout-ce.pkg (RPCS3) and ../build/fallout-ce.gnpdrm.pkg (HEN)
#
# APPID must be the SAME as PS3_GAME_DATA_PATH in the engine
# (src/platform/ps3/paths.h): the pkg installs under /dev_hdd0/game/FALL1CE00/
# and the engine reads its data from there - EBOOT and assets meet in the same USRDIR.

ifeq ($(strip $(PSL1GHT)),)
$(error PSL1GHT is not defined - run inside the ps3dev Docker image)
endif

include $(PSL1GHT)/ppu_rules

LD := $(CC)

TARGET     := fallout-ce
TITLE      := Fallout 1 (CE Port)
APPID      := FALL1CE00
CONTENTID  := UP0001-$(APPID)_00-0000000000000000
ICON0      := $(CURDIR)/ICON0.PNG
PKGFILES   := $(CURDIR)/pkgfiles

OUTDIR := $(CURDIR)/../build

all: $(TARGET).pkg
	$(VERB) mkdir -p $(OUTDIR)
	$(VERB) cp $(TARGET).pkg $(TARGET).gnpdrm.pkg $(OUTDIR)/
	@echo "Packages ready: $(OUTDIR)"

# The engine ELF comes from the fallout1-ce build (build that first).
$(TARGET).elf: ../fallout1-ce/build-ps3/fallout-ce
	cp $< $@

clean:
	rm -rf $(TARGET).elf $(TARGET).self $(TARGET).fake.self \
	       $(TARGET).pkg $(TARGET).gnpdrm.pkg build

.PHONY: all clean
