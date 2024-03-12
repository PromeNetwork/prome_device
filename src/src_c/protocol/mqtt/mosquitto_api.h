

#ifndef __MOSQUITTO_API_H__
#define __MOSQUITTO_API_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <semaphore.h>

#include "basic_types.h"
#include "mosquitto.h"
#include "mosquitto_api.h"


#define AUTH_ONE_WAY	"one_way"
#define AUTH_TWO_WAY	"two_way"


typedef int (*MQTT_MSG_CB)(void *payload, UINT32 len, char *topic, void *para);

typedef struct _MQTT_CONNECT_PARAM
{
	char username[16];
	char password[16];
	char host[64];
	int qos;
	int port;
	char *sub_topics;
	MQTT_MSG_CB msg_cb;
	void *userdata;
	sem_t disconnect_sem;
	struct mosquitto *mosq;
}MQTT_CONNECT_PARAM;

void MqttDeInit();
void MqttInit();
int MqttPublish(char *topic, void *payload, int len, struct mosquitto *mosq);
int MqttServerConnect(MQTT_CONNECT_PARAM *para);
void MqttServerDisConnect(MQTT_CONNECT_PARAM *para);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MOSQUITTO_API_H__ */
