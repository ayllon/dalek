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

add_custom_target (install_stage1
    COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/stage1.bin of="${FLOPPY_IMAGE}" conv=notrunc# bs=1 count=450 seek=62 skip=62
    DEPENDS stage1.bin "${FLOPPY_IMAGE}"
)

add_custom_target (install_stage2
    COMMAND mcopy -i "${FLOPPY_IMAGE}" -o "${CMAKE_CURRENT_BINARY_DIR}/root/*" ::/
    DEPENDS stage2.bin install_stage1
)

add_custom_target (floppy DEPENDS "${FLOPPY_IMAGE}" install_stage1 install_stage2)

