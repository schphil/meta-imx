
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " file://inputselector-sticky-events-haven-t-send-out-when-ac-1-4-1.patch \
                   file://0001-baseparse-handle-base-parse-error.patch \
"

