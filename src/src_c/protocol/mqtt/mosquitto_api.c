
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include "basic_types.h"

#include "mosquitto.h"
#include "mosquitto_api.h"
#include "log.h"
#include "cJSON.h"
#include "hexutils.h"

typedef int (*MQTT_DISC_CB)(struct mosquitto *mosq, void *obj, int result);
typedef int (*MQTT_CONN_CB)(struct mosquitto *mosq, void *obj, int result);

typedef struct _MQTT_USER_PARAM {
	MQTT_MSG_CB msg_cb;
	MQTT_DISC_CB disc_cb;
	MQTT_CONN_CB conn_cb;
	void *userdata;
	char *topic;
	int qos;
	sem_t sem_connect;
	int connect_status;
	struct mosquitto *mosq;
}MQTT_USER_PARAM, *P_MQTT_USER_PARAM;


void MqttInit()
{
	mosquitto_lib_init();
}

void MqttDeInit()
{
	mosquitto_lib_cleanup();
}

static void on_connect(struct mosquitto *mosq, void *obj, int result)
{
	MQTT_USER_PARAM *userdata = (MQTT_USER_PARAM *)obj;

	if(obj == NULL)
	{
		return;
	}
	
	if(result)
	{
		userdata->connect_status = 1;
	}
	else
	{
		userdata->connect_status = 0;
		if(userdata->conn_cb)
		{
			userdata->conn_cb(mosq, obj, result);
		}
	}
	sem_post(&userdata->sem_connect);
}

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	MQTT_USER_PARAM *user = (MQTT_USER_PARAM *)obj;

	if(user && user->msg_cb)
	{
		user->mosq = mosq;
		user->msg_cb(msg->payload, msg->payloadlen, msg->topic, user->userdata);
	}
	return ;
}

static void on_disconnect(struct mosquitto *mosq, void *userdata, int rc)
{
	//LOG_PRINT(  Prome_LOG_INFO, "Mqtt disconnect from server.");
}

static int sub_on_connect(struct mosquitto *mosq, void *obj, int result)
{
	MQTT_USER_PARAM *userdata = (MQTT_USER_PARAM *)obj;
	cJSON *root = NULL;
	cJSON *tmp = NULL;
	int i = 0;

	if(userdata == NULL || userdata->topic == NULL)
	{
		return 0;
	}

	root = cJSON_Parse((char *)userdata->topic);
	if(root == NULL)
	{
		return 0;
	}

	for(i = 0; i < cJSON_GetArraySize(root); ++i)
	{
		tmp = cJSON_GetArrayItem(root, i);
		
		mosquitto_subscribe(mosq, NULL, tmp->valuestring, userdata->qos);
		
	}

	cJSON_Delete(root);
	return 0;
}

static int sub_on_disconnect(struct mosquitto *mosq, void *obj, int result)
{
	MQTT_USER_PARAM *userdata = (MQTT_USER_PARAM *)obj;
	sem_destroy(&userdata->sem_connect);
	SAFE_FREE(userdata);
}


static void clientid_create(char *clientid)
{
	TIMESTAMP stamp = 0;
	
	stamp = GetTimestampInt();
	srand(stamp);
	//srand(rand());
	sprintf(clientid, "%6d", rand());
}

static int _mqtt_connect_(MQTT_CONNECT_PARAM *para, void *obj)
{
	int rc;
	struct mosquitto *mosq = NULL;
	MQTT_USER_PARAM *userdata = (MQTT_USER_PARAM *)obj;
	char clientid[32];

	memset(clientid, 0, sizeof(clientid));
	clientid_create(clientid);
	mosq = mosquitto_new(clientid, true, obj);
	if(mosq == NULL)
	{
		return 1;
	}


	para->mosq = mosq;

	mosquitto_int_option(mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V31);
	#ifdef MQTT_TLS_SUPPORT
	if((access(ROOT_DIR"/data/ca/ca.crt", F_OK)) == 0)   
    {   
        if(strcmp(MQTT_TLS_AUTH_DIR, AUTH_ONE_WAY) == 0)
		{
			mosquitto_tls_set(mosq, ROOT_DIR"/data/ca/ca.crt", NULL, NULL, NULL, NULL);
		}
		else if(strcmp(MQTT_TLS_AUTH_DIR, AUTH_TWO_WAY) == 0)
		{
			mosquitto_tls_set(mosq, ROOT_DIR"/data/ca/ca.crt", ROOT_DIR"/data/ca", ROOT_DIR"/data/ca/client.crt", ROOT_DIR"/data/ca/client.key", NULL);
		}
    }   
	
	#endif
	mosquitto_username_pw_set(mosq, para->username, para->password);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_disconnect_callback_set(mosq, on_disconnect);
	mosquitto_message_callback_set(mosq, on_message);

	rc = mosquitto_loop_start(mosq);
    if(rc != MOSQ_ERR_SUCCESS)
    {
        LOG_PRINT(  Prome_LOG_ERROR, "mosquitto loop error, ret:%d", rc);
		mosquitto_destroy(mosq);
		para->mosq = NULL;
		return rc;
    }
	
	rc = mosquitto_connect(mosq, para->host, para->port, 60);
	if(rc)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Mqtt Connect to server %s port %d fail.",
				  			para->host, para->port);
		mosquitto_destroy(mosq);
		para->mosq = NULL;
		return rc;
	}

	sem_wait(&userdata->sem_connect);

	if(userdata->connect_status)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Mqtt Connect to server %s port %d fail.",
				  			para->host, para->port);
		mosquitto_destroy(mosq);
		para->mosq = NULL;
		return 1;
	}
	else
	{
		LOG_PRINT(  Prome_LOG_INFO, "Mqtt Connect to server %s port %d ok.",
				  			para->host, para->port);
		return 0;
	}
	
}

int MqttPublish(char *topic, void *payload, int len, struct mosquitto *mosq)
{
	int rc;

	rc = mosquitto_publish(mosq, NULL, topic, len, payload, 0, false);
	if(rc)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Mqtt publish topic %s fail, ret:%d.",
				  topic, rc);
		return rc;
	}

	return 0;
}

#if 0
int MqttServerConnect(	char *topic_list, 
						MQTT_PARAM *para, 
				  		MQTT_MSG_CB msg_cb, 
				  		void *userdata,
				  		struct mosquitto **fd,
				  		sem_t *disconnect_sem
						)
{
	MQTT_SUB_PARAM *cb_userdata = NULL;
	struct timespec tv = { 0, 50e6 };
	int rc;

	if(*fd != NULL)
	{
		return 0;
	}

	cb_userdata = malloc(sizeof(MQTT_SUB_PARAM));
	if(cb_userdata == NULL)
	{
		return -1;
	}
	memset(cb_userdata, 0, sizeof(MQTT_SUB_PARAM));

	sem_init(&cb_userdata->sem_connect, 0, 0);

	cb_userdata->topic = topic_list;
	cb_userdata->qos = para->qos;
	cb_userdata->userdata = userdata;
	cb_userdata->msg_cb = msg_cb;
	cb_userdata->disc_cb = sub_on_disconnect;
	cb_userdata->conn_cb = sub_on_connect;

	rc = _mqtt_connect_(para, fd, (void *)cb_userdata);
	if(rc)
	{
		sem_destroy(&cb_userdata->sem_connect);
		SAFE_FREE(cb_userdata);
		return 1;
	}


	if(topic_list != NULL)
	{
		if(disconnect_sem != NULL)
		{
			sem_wait(disconnect_sem);
		}
		else
		{
			while(1)
			{
				nanosleep(&tv, NULL);
			}
		}
		SAFE_FREE(topic_list);
	}
	sem_destroy(&cb_userdata->sem_connect);
	SAFE_FREE(cb_userdata);
	return rc;
}

#else
int MqttServerConnect(MQTT_CONNECT_PARAM *para)
{
	MQTT_USER_PARAM *cb_userdata = NULL;
	int rc;
	
	if(para == NULL)
	{
		return -1;
	}

	if(para->mosq != NULL)
	{
		return 0;
	}

	cb_userdata = (MQTT_USER_PARAM *)malloc(sizeof(MQTT_USER_PARAM));
	if(cb_userdata == NULL)
	{
		return -1;
	}
	memset(cb_userdata, 0, sizeof(MQTT_USER_PARAM));

	sem_init(&para->disconnect_sem, 0, 0);
	sem_init(&cb_userdata->sem_connect, 0, 0);

	cb_userdata->topic = para->sub_topics;
	cb_userdata->qos = para->qos;
	cb_userdata->userdata = para->userdata;
	cb_userdata->msg_cb = para->msg_cb;
	cb_userdata->disc_cb = sub_on_disconnect;
	cb_userdata->conn_cb = sub_on_connect;

	rc = _mqtt_connect_(para, (void *)cb_userdata);
	if(rc)
	{
		sem_destroy(&cb_userdata->sem_connect);
		SAFE_FREE(cb_userdata);
		return -1;
	}

	if(para->sub_topics != NULL)
	{
		sem_wait(&para->disconnect_sem);
		//SAFE_FREE(para->sub_topics);
	}
	sem_destroy(&cb_userdata->sem_connect);
	SAFE_FREE(cb_userdata);
	return 0;
}

#endif


void MqttServerDisConnect(MQTT_CONNECT_PARAM *para)
{

	if((para == NULL) || (para->mosq == NULL))
	{
		return;
	}

	sem_post(&para->disconnect_sem);
	
	mosquitto_disconnect(para->mosq);
	mosquitto_loop_stop(para->mosq, false);
	mosquitto_destroy(para->mosq);
	
	para->mosq = NULL;
	sem_destroy(&para->disconnect_sem);
}


