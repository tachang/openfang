

REVISION_HASH := $(shell git rev-parse --quiet --short HEAD)

release:	
	$(info Creating new firmware release: openfang_${REVISION_HASH}.bin)

build:
	docker build -t tachang/openfang-buildenv .

syncrootfs:
	rsync -vr /src/overlayfs_dafang/etc/ /sharedfiles/rootfs/etc/
