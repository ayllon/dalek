cmake_minimum_required (VERSION 2.8)

set (FLOPPY_IMAGE "${CMAKE_CURRENT_BINARY_DIR}/floppy.img")

add_custom_command (OUTPUT ${FLOPPY_IMAGE}
    COMMAND dd if="/dev/zero" of="${FLOPPY_IMAGE}" bs=1024 count=1440
)

add_custom_target (formated_floppy
    COMMAND mkfs.vfat "${FLOPPY_IMAGE}" -n "DALEK"
    DEPENDS "${FLOPPY_IMAGE}"
)

add_custom_target (install_stage1
    COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/bootloader/stage1/stage1.bin of="${FLOPPY_IMAGE}" conv=notrunc# bs=1 count=450 seek=62 skip=62
    DEPENDS stage1.bin formated_floppy
)

add_custom_target (install_stage2
    COMMAND mcopy -i "${FLOPPY_IMAGE}" -o "${CMAKE_CURRENT_BINARY_DIR}/bootloader/stage2/stage2.bin" ::/
    DEPENDS stage2.bin formated_floppy
)

add_custom_target (floppy DEPENDS formated_floppy install_stage1 install_stage2)

