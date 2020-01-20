CMAKE_MINIMUM_REQUIRED(VERSION 3.9)


include(scripts/toolchain.cmake)

MACRO(GET_SOURCES OUTPUT SRC_DIR)
	FILE(GLOB_RECURSE ${OUTPUT} ${SRC_DIR}/*.c ${SRC_DIR}/*.cpp ${SRC_DIR}/*.h ${SRC_DIR}/*.hpp)
ENDMACRO(GET_SOURCES)

project("blink")

FILE(GLOB_RECURSE ASM_SX_FILES *.sx)
SET_SOURCE_FILES_PROPERTIES(${ASM_SX_FILES} PROPERTIES LANGUAGE CXX)

SET(RESET_HANDLER_SRC "modm/src/modm/platform/core/reset_handler.sx")

add_library(modm OBJECT)
get_sources(MODM_SRC modm)
target_sources(modm PRIVATE ${MODM_SRC})
target_include_directories(modm PUBLIC modm/src modm/ext/cmsis/core modm/ext/cmsis/device modm/ext)

get_sources(MAIN_SRC src)
add_executable(blink "${MAIN_SRC}" ${RESET_HANDLER_SRC})
target_include_directories(blink PRIVATE "src")
target_link_libraries(blink PRIVATE modm)
set_target_properties(blink PROPERTIES SUFFIX ".elf")

ADD_CUSTOM_COMMAND(TARGET blink POST_BUILD COMMAND ${CMAKE_SIZE} ARGS blink.elf)

ADD_CUSTOM_TARGET(upload-blink ${OPENOCD_COMMAND} -s ${OPENOCD_SCRIPTS} -f "${PROJECT_SOURCE_DIR}/scripts/openocd.cfg" -c "program_file ${PROJECT_BINARY_DIR}/blink.elf" DEPENDS blink openocd)