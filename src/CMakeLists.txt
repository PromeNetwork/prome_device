#cmake file for project association                     #表示注释
#author:chenxiaoquan@wxblockchain.com
#created:>2021/4/9

cmake_minimum_required(VERSION 3.5)

PROJECT(gateway_sdk)

if(JAVA_SUPPORT)
	ADD_SUBDIRECTORY(src_java)
endif()
if(BULLETPROOF_LIB)
	ADD_SUBDIRECTORY(bplib)
endif()
ADD_SUBDIRECTORY(src_c)


