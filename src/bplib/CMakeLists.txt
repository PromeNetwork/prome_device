#cmake file for project association                     #表示注释

#created:>2021/4/9


set(BP_BUILD_DIR ${TOP_BUILD_PATH}/src/bplib)
set(BP_SRC_DIR ${SRC_PATH}/bplib)
set(BP_INC_DIR ${INC_PATH}/bulletproof)
set(BP_LIB_NAME libbulletproof)

file(MAKE_DIRECTORY ${BP_INC_DIR})

execute_process(COMMAND sh ${BP_SRC_DIR}/compile.sh ${TOP_BUILD_PATH} ${BP_SRC_DIR} ${BP_LIB_NAME} ${SW_VERSION} ${BP_INC_DIR} 
				# COMMAND pwd
				# COMMAND ls ${BP_BUILD_DIR}
				# COMMAND cp ${BP_BUILD_DIR}/${BP_LIB_NAME}.h ${INC_PATH}/bulletproof
				)
# file(COPY ${BP_BUILD_DIR}/${BP_LIB_NAME}.h DESTINATION ${INC_PATH}/bulletproof)
# INSTALL(FILES ${BP_BUILD_DIR}/${BP_LIB_NAME}.h DESTINATION ${INC_PATH}/bulletproof)
INSTALL(FILES ${BP_BUILD_DIR}/${BP_LIB_NAME}.so DESTINATION lib)
INSTALL(FILES ${BP_BUILD_DIR}/${BP_LIB_NAME}.so.1 DESTINATION lib)
INSTALL(FILES ${BP_BUILD_DIR}/${BP_LIB_NAME}.so.${SW_VERSION} DESTINATION lib)
