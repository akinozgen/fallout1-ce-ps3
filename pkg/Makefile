# Fallout 1 CE — PS3 kurulum paketi (.pkg)
#
# DIKKAT: pkgfiles/ dizinine oyun verisi konursa paket onu da icerir; boyle
# uretilen paket TELIFLI icerik tasidigi icin dagitilamaz, yalnizca yerel
# testte kullanilir. Dagitimlik surumde pkgfiles/ bos birakilir.
#
# Kullanim (proje kokunden):
#   docker run --rm -v "$PWD":/src -w /src/pkg ps3dev make
# Ciktilar: ../build/fallout-ce.pkg (RPCS3) ve ../build/fallout-ce.gnpdrm.pkg (HEN)
#
# APPID, motordaki PS3_GAME_DATA_PATH (src/platform/ps3/paths.h) ile AYNI
# olmali: pkg /dev_hdd0/game/FALL1CE00/ altina kurulur, motor da veriyi
# oradan okur — EBOOT ile asset'ler ayni USRDIR'de bulusur.

ifeq ($(strip $(PSL1GHT)),)
$(error PSL1GHT tanimsiz — ps3dev Docker imaji icinde calistir)
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
	@echo "Paketler hazir: $(OUTDIR)"

# Motorun ELF'i fallout1-ce build'inden gelir (once onu derlemis ol).
$(TARGET).elf: ../fallout1-ce/build-ps3/fallout-ce
	cp $< $@

clean:
	rm -rf $(TARGET).elf $(TARGET).self $(TARGET).fake.self \
	       $(TARGET).pkg $(TARGET).gnpdrm.pkg build

.PHONY: all clean
