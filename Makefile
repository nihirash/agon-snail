ASM:=ez80asm
ASM_FLAGS:=-l -i

SRC_PREFIX=src

RASTERFORMATS := [Pp][Nn][Gg]
IMG_CONVERTER := tools/img2agonimg.py
IMG_PATH := $(SRC_PREFIX)/raw-images
IMG_DEST := $(SRC_PREFIX)/binaries
IMG_TO_CONVERT := $(foreach format,$(RASTERFORMATS), $(wildcard $(IMG_PATH)/*.$(format)))
IMGS_CONVERTED := $(sort $(addprefix $(IMG_DEST)/,$(addsuffix .agi,$(notdir $(basename $(IMG_TO_CONVERT))))))

DOCUMENTS := $(wildcard $(SRC_PREFIX)/documents/*.*)

ASM_SRC := $(wildcard $(SRC_PREFIX)/**/*.inc) $(wildcard $(SRC_PREFIX)/*.inc) $(wildcard $(SRC_PREFIX)/*.asm)
MAIN_SRC := snail.asm
BINARY_PATH := $(abspath .)
BINARY := $(BINARY_PATH)/snail.bin

.SECONDEXPANSION:
.DELETE_ON_ERROR:

$(IMG_DEST)/%.agi: $(IMG_PATH)/%.png
		$(IMG_CONVERTER) $< $@

$(BINARY): $(ASM_SRC) $(IMGS_CONVERTED) $(DOCUMENTS)
		(cd $(SRC_PREFIX) && $(ASM) $(MAIN_SRC) -l $(BINARY))

all: $(BINARY)

clean:
		rm -f $(IMGS_CONVERTED) $(BINARY)