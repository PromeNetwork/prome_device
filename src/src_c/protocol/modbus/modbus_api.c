#include <errno.h>

#include "basic_types.h"
#include "log.h"
#include "modbus.h"

UINT32 ModbusConnect(UINT8 *ipaddr, UINT32 port, UINT32 slave_id, modbus_t **fd)
{
	modbus_t *ctx = NULL;

	if(ipaddr == NULL || fd == NULL)
	{
		return 1;
	}

	if(*fd != NULL)
	{
		return 0;
	}
	
	/* 创建并初始化 modbus_t 指针 */
    ctx = modbus_new_tcp(ipaddr, port);
    if (ctx == NULL) 
	{
        LOG_PRINT(WX_LOG_ERROR, "Modbus connection %s port %d id %d failed: %s", 
							  ipaddr, port, slave_id, 
							  "unable to allocate libmodbus context");
        return 1;
    }

	/* 设置错误恢复模式 */
    modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_LINK |
                              	   MODBUS_ERROR_RECOVERY_PROTOCOL);
    //modbus_set_debug(ctx, TRUE);

    if (modbus_connect(ctx) == -1) 
	{
        LOG_PRINT(WX_LOG_ERROR, "Modbus connection %s port %d id %d failed: %s", 
							  ipaddr, port, slave_id, modbus_strerror(errno));
        modbus_free(ctx);
        return 1;
    }
	
	modbus_set_slave(ctx, slave_id);
	*fd = ctx;

	return 0;
}

void ModbusDisConnect(modbus_t **fd)
{
	modbus_t *ctx = NULL;

	if(fd == NULL || *fd == NULL)
	{
		return;
	}
	ctx = *fd;
	modbus_close(ctx);
	modbus_free(ctx);
	*fd = NULL;
}

UINT32 ModbusReConnect(UINT8 *ipaddr, UINT32 port, UINT32 slave_id, modbus_t **fd)
{
	UINT32 ret = 0;

	if(ipaddr == NULL || fd == NULL)
	{
		return 1;
	}
	
	ModbusDisConnect(fd);

	return ModbusConnect(ipaddr, port, slave_id, fd);
	
}



