aux_source_directory(. DIR_AES_SRCS)
add_library(aes ${DIR_AES_SRCS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/aes)