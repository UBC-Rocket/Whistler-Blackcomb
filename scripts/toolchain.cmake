SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# Configure toolchain
SET(GCC_ARM_NONE_EABI_VERSION 9.2.1)

if (WIN32)
	set(TOOL_EXECUTABLE_SUFFIX ".exe")
	if(NOT EXISTS "${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}.zip")
		message("Downloading compiler for Windows")
		file(DOWNLOAD
			"https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/RC2.1/gcc-arm-none-eabi-9-2019-q4-major-win32.zip.bz2?revision=0dc1c4c9-8bba-4577-b57d-dc57d6f80c26&la=en&hash=F0B7C0475BA3213D5CC5DB576C75EC7D9BA3614A"
			"${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}.zip"
		)
	endif()
	if(NOT EXISTS "${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}")
		message("Extracting compiler for Windows")
		file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}")
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf "${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}.zip"
						WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}")
	endif()

	include(ExternalProject)
	ExternalProject_Add(openocd
		URL    "http://sysprogs.com/getfile/798/openocd-20200114.7z"
		PREFIX "${CMAKE_SOURCE_DIR}/tools/openocd"
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
	)
	set(OPENOCD_COMMAND "${CMAKE_SOURCE_DIR}/tools/openocd/src/openocd/bin/openocd.exe")
	set(OPENOCD_SCRIPTS "${CMAKE_SOURCE_DIR}/tools/openocd/src/openocd/share/scripts")
else()
	message(FATAL_ERROR "Linux and Mac support coming")
endif()

IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}/bin/")
	message(FATAL_ERROR "toolchain not installed")
ENDIF()



IF(NOT TARGET_TRIPLET)
	SET(TARGET_TRIPLET "${CMAKE_SOURCE_DIR}/tools/gcc-arm-none-eabi-${GCC_ARM_NONE_EABI_VERSION}/bin/arm-none-eabi")
ENDIF()

IF (WIN32)
	SET(TOOL_EXECUTABLE_SUFFIX ".exe")
ELSE()
	SET(TOOL_EXECUTABLE_SUFFIX "")
ENDIF()

	SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
	SET(CMAKE_C_COMPILER "${TARGET_TRIPLET}-gcc${TOOL_EXECUTABLE_SUFFIX}" CACHE STRING "Cross-compiler")
	SET(CMAKE_CXX_COMPILER "${TARGET_TRIPLET}-g++${TOOL_EXECUTABLE_SUFFIX}" CACHE STRING "Cross-compiler")

SET(CMAKE_LINKER       "${TARGET_TRIPLET}-g++${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "linker tool")
SET(CMAKE_ASM_COMPILER "${TARGET_TRIPLET}-gcc${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "ASM compiler")
SET(CMAKE_OBJCOPY      "${TARGET_TRIPLET}-objcopy${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "objcopy tool")
SET(CMAKE_OBJDUMP      "${TARGET_TRIPLET}-objdump${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "objdump tool")
SET(CMAKE_SIZE         "${TARGET_TRIPLET}-size${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "size tool")
SET(CMAKE_DEBUGER      "${TARGET_TRIPLET}-gdb${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "debuger")
SET(CMAKE_CPPFILT      "${TARGET_TRIPLET}-c++filt${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "C++filt")
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Prepare linker script
CONFIGURE_FILE(modm/link/linkerscript.ld ${CMAKE_CURRENT_BINARY_DIR}/${LINKER_SCRIPT_NAME})

# Toolchain configuration
SET(CCFLAGS "\
	-fdata-sections \
	-ffunction-sections \
	-finline-limit=10000 \
	-fshort-wchar \
	-fsingle-precision-constant \
	-funsigned-bitfields \
	-funsigned-char \
	-fwrapv \
	-g3 \
	-gdwarf \
	-W \
	-Wall \
	-Wdouble-promotion \
	-Wduplicated-cond \
	-Werror=format \
	-Werror=maybe-uninitialized \
	-Werror=overflow \
	-Werror=sign-compare \
	-Wextra \
	-Wlogical-op \
	-Wpointer-arith \
	-Wundef \
")

SET(CCFLAGS_RELEASE "\
	-O3 \
")

SET(CCFLAGS_DEBUG "\
	-fno-move-loop-invariants \
	-fno-split-wide-types \
	-fno-tree-loop-optimize \
	-Og \
")


SET(CFLAGS "\
	-std=gnu11 \
	-Wbad-function-cast \
	-Wimplicit \
	-Wnested-externs \
	-Wredundant-decls \
	-Wstrict-prototypes \
")


SET(CXXFLAGS "\
	-fno-exceptions \
	-fno-rtti \
	-fno-threadsafe-statics \
	-fno-unwind-tables \
	-fstrict-enums \
	-fuse-cxa-atexit \
	-std=c++17 \
	-Woverloaded-virtual \
")


SET(ASFLAGS "\
	-g3 \
	-gdwarf \
")


SET(ARCHFLAGS "\
	-mcpu=cortex-m7 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-mthumb \
")


SET(LINKFLAGS "\
	--specs=nano.specs \
	--specs=nosys.specs \
	-Llink \
	-nostartfiles \
	-Tlinkerscript.ld \
	-Wl,--fatal-warnings \
	-Wl,--gc-sections \
	-Wl,--no-wchar-size-warning \
	-Wl,--relax \
	-Wl,-Map,${CMAKE_PROJECT_NAME}.map,--cref \
	-Wl,-wrap,_calloc_r \
	-Wl,-wrap,_free_r \
	-Wl,-wrap,_malloc_r \
	-Wl,-wrap,_realloc_r \
")


# Set flags for CMake
SET(CMAKE_C_FLAGS "${ARCHFLAGS} ${CCFLAGS} ${CFLAGS}" CACHE INTERNAL "c compiler flags")
SET(CMAKE_C_FLAGS_RELEASE   "${CCFLAGS_RELEASE}"      CACHE INTERNAL "c compiler flags release")
SET(CMAKE_C_FLAGS_DEBUG     "${CCFLAGS_DEBUG}"        CACHE INTERNAL "c compiler flags debug")

SET(CMAKE_CXX_FLAGS "${ARCHFLAGS} ${CCFLAGS} ${CXXFLAGS}" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "${CCFLAGS_RELEASE}"          CACHE INTERNAL "cxx compiler flags release")
SET(CMAKE_CXX_FLAGS_DEBUG   "${CCFLAGS_DEBUG}"            CACHE INTERNAL "cxx compiler flags debug")

SET(CMAKE_ASM_FLAGS "${ARCHFLAGS} ${ASFLAGS}" CACHE INTERNAL "asm compiler flags")

SET(CMAKE_EXE_LINKER_FLAGS  "${ARCHFLAGS} ${LINKFLAGS}" CACHE INTERNAL "linker flags")

IF(APPLE)
	STRING(REPLACE "-Wl,-search_paths_first" "" CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS})
	STRING(REPLACE "-Wl,-search_paths_first" "" CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS})
ENDIF()
