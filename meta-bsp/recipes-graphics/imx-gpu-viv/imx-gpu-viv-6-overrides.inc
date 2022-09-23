require recipes-graphics/imx-gpu-viv/imx-gpu-viv-6.inc

### Section 1: Changes to be upstreamed ###

do_install () {
    install -d ${D}${libdir}
    install -d ${D}${includedir}
    install -d ${D}${bindir}

    cp -P ${S}/gpu-core/usr/lib/*.so* ${D}${libdir}
    cp -r ${S}/gpu-core/usr/include/* ${D}${includedir}
    cp -r ${S}/gpu-demos/opt ${D}
    cp -r ${S}/gpu-tools/gmem-info/usr/bin/* ${D}${bindir}

    # Use vulkan header from vulkan-headers recipe to support vkmark
    rm -rf ${D}${includedir}/vulkan/

    # Install SOC-specific drivers
    if [ -d ${S}/gpu-core/usr/lib/${IMX_SOC} ]; then
        cp -r ${S}/gpu-core/usr/lib/${IMX_SOC}/* ${D}${libdir}
    fi

    install -d ${D}${libdir}/pkgconfig
    if ${HAS_GBM}; then
        install -m 0644 ${S}/gpu-core/usr/lib/pkgconfig/gbm.pc ${D}${libdir}/pkgconfig/gbm.pc
    fi

    install -m 0644 ${S}/gpu-core/usr/lib/pkgconfig/glesv1_cm.pc ${D}${libdir}/pkgconfig/glesv1_cm.pc
    install -m 0644 ${S}/gpu-core/usr/lib/pkgconfig/glesv2.pc    ${D}${libdir}/pkgconfig/glesv2.pc
    install -m 0644 ${S}/gpu-core/usr/lib/pkgconfig/vg.pc        ${D}${libdir}/pkgconfig/vg.pc

    # Install backend-specific drivers
    if [ "${BACKEND}" = "wayland" ]; then
        # Wayland backend
        install -m 0644 ${S}/gpu-core/usr/lib/pkgconfig/egl_wayland.pc ${D}${libdir}/pkgconfig/egl.pc
        cp -r ${S}/gpu-core/usr/lib/wayland/* ${D}${libdir}
    elif [ "${IS_MX8}" != "1" ]; then
        # Framebuffer backend for i.MX 6 and 7
        install -m 0644 ${S}/gpu-core/usr/lib/pkgconfig/egl_linuxfb.pc ${D}${libdir}/pkgconfig/egl.pc
        cp -r ${S}/gpu-core/usr/lib/fb/* ${D}${libdir}
    else
        # Framebuffer backend for i.MX 8 and beyond
        install -m 0644 ${S}/gpu-core/usr/lib/pkgconfig/egl.pc         ${D}${libdir}/pkgconfig/egl.pc
        cp -r ${S}/gpu-core/usr/lib/wayland/* ${D}${libdir}
        rm -rf \
            ${D}${libdir}/libVDK.so* \
            ${D}/opt/viv_samples/es20 \
            ${D}/opt/viv_samples/tiger \
            ${D}/opt/viv_samples/vdk
    fi
    if [ "${BACKEND}" = "wayland" ] && [ "${IS_MX8}" != "1" ]; then
        # Special case for libVDK on Wayland backend, deliver fb library as well.
        cp ${S}/gpu-core/usr/lib/fb/libVDK.so.1.2.0 ${D}${libdir}/libVDK-fb.so.1.2.0
    fi

    # Install Vendor ICDs for OpenCL's installable client driver loader (ICDs Loader)
    install -d ${D}${sysconfdir}/OpenCL/vendors/
    install -m 0644 ${S}/gpu-core/etc/Vivante.icd ${D}${sysconfdir}/OpenCL/vendors/Vivante.icd

    if [ "${IS_MX8}" = "1" ]; then
        if [ -z "${PACKAGES_VULKAN}" ]; then
            rm -f ${D}${libdir}/libvulkan_VSI.so* ${D}${libdir}/libSPIRV_viv${SOLIBS}*
        fi
    fi

    # FIXME: MX6SL does not have 3D support; hack it for now
    if [ "${IS_MX6SL}" = "1" ]; then
        rm -rf ${D}${libdir}/libCLC* ${D}${includedir}/CL \
               \
               ${D}${libdir}/libGL* ${D}${includedir}/GL* ${D}${libdir}/pkgconfig/gl.pc \
               \
               ${D}${libdir}/libGLES* ${D}${libdir}/pkgconfig/gles*.pc \
               \
               ${D}${libdir}/libOpenCL* ${D}${includedir}/CL \
               \
               ${D}${libdir}/libOpenVG.3d.so \
               \
               ${D}${libdir}/libVivanteOpenCL.so \
               \
               ${D}/opt/viv_samples/vdk \
               ${D}/opt/viv_samples/es20 ${D}/opt/viv_samples/cl11

        ln -sf libOpenVG.2d.so ${D}${libdir}/libOpenVG.so
    fi

    find ${D}${libdir} -type f -exec chmod 644 {} \;
    find ${D}${includedir} -type f -exec chmod 644 {} \;

    chown -R root:root "${D}"
}

### Section 2: Changes to be kept in meta-imx ###

PACKAGECONFIG ?= "valgrind"

RDEPENDS:libopenvg-imx += "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd-gpuconfig', '', d)}"

# FIXME: Don't add .so to main package
FILES:libvulkan-imx += "${libdir}/libvulkan_VSI${SOLIBS}"
INSANE_SKIP:libvulkan-imx += "dev-so"
FILES:libopenvx-imx += "${libdir}/libOpenVX${SOLIBS}"
INSANE_SKIP:libopenvx-imx += "dev-so dev-deps"
FILES:libopencl-imx += "${libdir}/libOpenCL${SOLIBS}"
INSANE_SKIP:libopencl-imx += "dev-so dev-deps"
FILES:libopenvg-imx += "${libdir}/libOpenVG*${SOLIBS}"
INSANE_SKIP:libopenvg-imx += "dev-so"
FILES:libvdk-imx += "${libdir}/libVDK*${SOLIBS}"
INSANE_SKIP:libvdk-imx += "dev-so"