cmake_minimum_required(VERSION 3.10)
# Set the project name
project (libamp)

file(GLOB COMPONENT_SRCS
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/usb/*.c
    ${PROJECT_SOURCE_DIR}/littlefs/*.c
    ${PROJECT_SOURCE_DIR}/src/midi/*.c
    ${PROJECT_SOURCE_DIR}/src/midi/bytequeue/*.c
)
# Add a library with the above sources
add_library(${PROJECT_NAME} ${COMPONENT_SRCS})

target_compile_definitions( ${PROJECT_NAME} PRIVATE 
    #LFS_NO_MALLOC
    LFS_NO_ASSERT
    #LFS_YES_TRACE
    LFS_NO_DEBUG
    LFS_NO_ERROR
    LFS_NO_WARN
)

target_include_directories( ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src/
    ${PROJECT_SOURCE_DIR}/usb/
    ${PROJECT_SOURCE_DIR}/littlefs/
    ${PROJECT_SOURCE_DIR}/src/midi/
    ${PROJECT_SOURCE_DIR}/src/midi/bytequeue/
    ${PROJECT_SOURCE_DIR}/usb/lufa/
)