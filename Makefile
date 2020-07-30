

REVISION_HASH := $(shell git rev-parse --quiet --short HEAD)




release:
	mkdir -p _build
	$(info Creating new firmware release: openfang_${REVISION_HASH}.bin)
	buildscripts/buildopenfang.sh

rootfs:
	
