

#ifndef __UPGRADE_H__
#define __UPGRADE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"

typedef struct _UPGRADE_CFG
 {
	 UINT8 protocol[DOMAIN_LEN];
	 UINT8 *url;
	 UINT8 host[DOMAIN_LEN];
	 UINT32 port;
	 UINT8 user[DOMAIN_LEN];
	 UINT8 pwd[DOMAIN_LEN];
	 UINT8 file[DOMAIN_LEN];
	 UINT8 ver[32];
	 UINT8 curr_path[DOMAIN_LEN];
 }UPGRADE_CFG, *P_UPGRADE_CFG;


UINT32 DoUpgrade(UPGRADE_CFG *cfg);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __UPGRADE_H__ */
