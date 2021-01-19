# Copyright 2020 NXP

DESCRIPTION = "This is the basic core image for V2X with internal HSM"

inherit core-image

IMAGE_FEATURES += 	" ssh-server-openssh \
			  debug-tweaks \
			"

IMAGE_INSTALL_append =	"  imx-seco-libs \
			   ecdsa-lib-imx \
			   v2xsehsm \
			   vtest \
			   util-linux-fdisk \
			   util-linux-rtcwake \
			   e2fsprogs-resize2fs \
			   lttng-tools \
			"

# Add 128 MB in root file system
IMAGE_ROOTFS_EXTRA_SPACE = "131072"

export IMAGE_BASENAME = "core-image-v2x-imx"

COMPATIBLE_MACHINE = "(mx8dxl)"
