/***********************************************************************************
 * @file   			: modbus_api.h
 * @author   		: xqchen
 * @date   			: 2021-2-1
 * @brief   		: modbus_api.c header file
 * @Copyright   	: Copyright (c) 2008-2021   Shanghai Wanxiang Blockchain Inc.
 * @other   		: 
 * @note History 	: 
***********************************************************************************/

#ifndef __MODBUS_API_H__
#define __MODBUS_API_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "modbus.h"


UINT32 ModbusConnect(UINT8 *ipaddr, UINT32 port, UINT32 slave_id, modbus_t **fd);
void ModbusDisConnect(modbus_t **fd);
UINT32 ModbusReConnect(UINT8 *ipaddr, UINT32 port, UINT32 slave_id, modbus_t **fd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MODBUS_API_H__ */
