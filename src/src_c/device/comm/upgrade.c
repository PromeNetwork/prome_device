#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>
#include <semaphore.h>

#include "basic_types.h"
#include "dev_cfg.h"
#include "upgrade.h"
#include "http_client.h"
#include "log.h"
#include "version.h"
#include "hexutils.h"

#define UPGRADE_SCRIPT	ROOT_DIR"/scripts/upgrade.sh"
#define UPGRADE_DIR		ROOT_DIR"/upgrade"


sem_t  upgrade_sem;
UINT32 upgrade_status = 0;


static void *_device_upgrade(void *ptr)
{
	UINT32 ret = 0;
	UPGRADE_CFG *cfg = NULL;
	UINT8 cmd[256];
	char localpath[128];

	if(ptr == NULL)
	{
		upgrade_status = 1;
		sem_post(&upgrade_sem);
		return NULL;
	}

	cfg = (UPGRADE_CFG *)ptr;

	memset(localpath, 0, sizeof(localpath));
	sprintf(localpath, "%s/%s", cfg->curr_path, cfg->file);
	
	printf("url:%s\r\n", cfg->url);
	LOG_PRINT(WX_LOG_INFO, "Start download upgrade file.");
	ret = http_download_file(cfg->url, localpath);
	if(ret)
	{
		LOG_PRINT(WX_LOG_ERROR, "Download upgrade file fail.");
		upgrade_status = 1;
		sem_post(&upgrade_sem);
		return NULL;
	}
	LOG_PRINT(WX_LOG_INFO, "Download upgrade file ok.");
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "%s %s %s", UPGRADE_SCRIPT, localpath, cfg->ver);
	ret = SystemNew(cmd);
	if(ret)
	{
		//LOG_PRINT(WX_LOG_ERROR, "Copy upgrade file fail.");
		upgrade_status = 1;
		sem_post(&upgrade_sem);
		return NULL;
	}
	upgrade_status = 0;
	sem_post(&upgrade_sem);
	return NULL;
}


UINT32 DoUpgrade(UPGRADE_CFG *cfg)
{
	pthread_t id; 
	UPGRADE_CFG *para = NULL;
	VERSION ver_now;
	VERSION ver_new;
	INT32 ret = 0;

	if(cfg == NULL)
	{
		return 1;
	}

	LOG_PRINT(WX_LOG_INFO, "System start upgrade ...");
	#if 0
	memset(&ver_new, 0, sizeof(ver_new));
	memset(&ver_now, 0, sizeof(ver_now));
	SoftVerGet(&ver_now);
	SoftVerParse(&ver_new, cfg->ver);
	ret = SoftVerCompare(&ver_now, &ver_new);
	//if((ret == 1) || (ret == 0))
	if(ret == 0)
	{/*当前版本高于或等于待升级版本，不升级*/
		LOG_PRINT(WX_LOG_ERROR, "%s %d.%d.%d.%s %s %s %s",
				  "The current version", ver_now.major_ver, 
				  ver_now.minor_ver, ver_now.stage_ver, ver_now.build_time,
				  "is higher than or equal to the version",
				  cfg->ver, "to be upgraded.");
		return 1;
	}
	#endif

	sem_init(&upgrade_sem, 0, 0);

	para = malloc(sizeof(UPGRADE_CFG));
	if(para == NULL)
	{
		LOG_PRINT(WX_LOG_ERROR, "Gateway upgrade fail!");
		return 1;
	}
	memcpy(para, cfg, sizeof(UPGRADE_CFG));

	if (!pthread_create(&id, NULL, _device_upgrade, (void *)para))
    {
        LOG_PRINT(WX_LOG_INFO, "Create upgrade thread OK!");
		sem_wait(&upgrade_sem);
		
    }
    else
    {
        LOG_PRINT(WX_LOG_ERROR, "Create upgrade thread fail!");
		sem_destroy(&upgrade_sem);
		return 1;
    }

	sem_destroy(&upgrade_sem);
	return upgrade_status;
}




