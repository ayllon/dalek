cmake_minimum_required (VERSION 2.8)

set (FLOPPY_IMAGE "floppy.img")

if (APPLE)
    add_custom_command(OUTPUT "${FLOPPY_IMAGE}"
        COMMAND hdiutil create -layout NONE -type UDTO -size 1440k -fs MS-DOS -volname "DALEK" -o "${FLOPPY_IMAGE}"
        COMMAND mv "${FLOPPY_IMAGE}.cdr" "${FLOPPY_IMAGE}"
    )
else (APPLE)
    add_custom_command (OUTPUT "${FLOPPY_IMAGE}"
        COMMAND dd if="/dev/zero" of="${FLOPPY_IMAGE}" bs=1024 count=1440
        COMMAND mkfs.vfat "${FLOPPY_IMAGE}" -n "DALEK"
    )
endif (APPLE)

add_custom_target (install_boot
    COMMAND "${CMAKE_SOURCE_DIR}/bootloader/install.py" -L "${CMAKE_CURRENT_BINARY_DIR}" -t fat1x "${FLOPPY_IMAGE}"
    DEPENDS stage1-fat1x.bin stage2.bin "${FLOPPY_IMAGE}"
)

add_custom_target (floppy DEPENDS "${FLOPPY_IMAGE}" install_boot)
