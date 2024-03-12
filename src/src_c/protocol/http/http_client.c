#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "basic_types.h" 

#include "curl/curl.h"
#include "curl/easy.h"
#include "http_client.h" 
#include "cJSON.h"
#include "log.h"


static size_t curl_write_data_cb(void *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	http_respone *mem = (http_respone *)userp;

	if((data == NULL) || (mem == NULL))
	{
		return 0;
	}

	char *ptr = realloc(mem->data, mem->data_len + realsize + 1);
	if(ptr == NULL)
		return 0;  /* out of memory! */

	mem->data = ptr;
	memcpy(&(mem->data[mem->data_len]), data, realsize);
	mem->data_len += realsize;
	mem->data[mem->data_len] = 0;

	return realsize;
}

int http_post(char *url, char *data_in, int len, void *data_out, char *token)
{
	http_respone *respone = (http_respone *)data_out;
	CURL *curl_handle = NULL;
	CURLcode curl_res;
	int ret = 0;
	char token_str[128];

	curl_handle = curl_easy_init();
	if (!curl_handle)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "curl easy init failed");
		ret = -1;
		return ret;
	}
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);	
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, len);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data_in);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 120);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 60L);

	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_data_cb);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, respone);
	curl_easy_setopt(curl_handle, CURLOPT_HEADER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL,1L);

	struct curl_slist *pList = NULL;

	pList = curl_slist_append(pList,"Content-Type: application/json");
	if(token && (strlen(token) != 0))
	{
		memset(token_str, 0, sizeof(token_str));
		sprintf(token_str, "Authorization: %s", token);
		pList = curl_slist_append(pList,token_str);
	}
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, pList);
	curl_res = curl_easy_perform(curl_handle);
	if(curl_res != CURLE_OK)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "curl_easy_perform error, err_msg:[%d]", curl_res);
		ret = -1;
	}

	if(respone->data)
	{
		//printf("respone.data:%s\r\n", respone->data);
	}

	if(curl_handle)
	{
		curl_easy_cleanup(curl_handle);
	}

	if(pList)
	{
		curl_slist_free_all(pList);
	}

	return ret;
}

int http_post_file_octetstream(char *url, HTTP_POST_FORM *form, UINT32 form_num, 
				   void *data_out, char *token)
{
	http_respone *respone = (http_respone *)data_out;
	CURL *curl_handle = NULL;
	CURLcode curl_res;
	//CURLFORMcode formCode;
	uint8_t timeout = 120U;
	UINT32 i = 0;
	char token_str[128];
	
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;

	//curl_global_init(CURL_GLOBAL_ALL);
	 /* Fill in the file upload field */ 
	for(i = 0; i < form_num; i++)
	{
		if(form[i].content_type == HTTP_CONTENT_OCTET_STREAM)
		{
			curl_formadd(&formpost,
 		                 &lastptr,
 		                 CURLFORM_COPYNAME, form[i].content_name,
 		                 CURLFORM_FILE, form[i].content,
 		                 CURLFORM_END);
		}
		else
		{
			curl_formadd(&formpost,
   		                 &lastptr,
   		                 CURLFORM_COPYNAME, form[i].content_name,
   		                 CURLFORM_COPYCONTENTS, form[i].content,
   		                 CURLFORM_END);
		}
	}

	curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "submit",
                 CURLFORM_COPYCONTENTS, "send",
                 CURLFORM_END);

	curl_handle = curl_easy_init();
	if (!curl_handle)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "curl easy init failed");
		return 1;
	}

	
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_data_cb);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, respone);
	curl_easy_setopt(curl_handle, CURLOPT_HEADER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 60L); 	
	curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL,1L);
	
	headerlist = curl_slist_append(headerlist,"Content-Type: application/octet-stream");
	if(token)
	{
		memset(token_str, 0, sizeof(token_str));
		sprintf(token_str, "Authorization: %s", token);
		headerlist = curl_slist_append(headerlist,token_str);
	}
	if(curl_handle) 
	{
	    /* what URL that receives this POST */ 
	    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	    //curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerlist);

	    curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);
	 
	    /* Perform the request, res will get the return code */ 
	    curl_res = curl_easy_perform(curl_handle);
	    /* Check for errors */ 
	    if(curl_res != CURLE_OK)
	      fprintf(stderr, "curl_easy_perform() failed: %s\n",
	              curl_easy_strerror(curl_res));
	 
	    /* always cleanup */ 
	    curl_easy_cleanup(curl_handle);
	 
	    /* then cleanup the formpost chain */ 
	    curl_formfree(formpost);
	    /* free slist */ 
	    curl_slist_free_all(headerlist);
 	}


	return 0;
}

int http_post_file_multipart(char *url, UINT8 *filename, void *data_out, char *token)
{
	struct curl_httppost *formpost = 0;
	struct curl_httppost *lastptr  = 0;
	struct curl_slist *headerlist = NULL;
	CURLcode curl_res;
	CURL *curl_handle = NULL;
	http_respone *respone = (http_respone *)data_out;
	uint8_t timeout = 120U;
	char token_str[128];

	curl_handle = curl_easy_init(); 
	if (!curl_handle)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "curl easy init failed");
		return -1;
	}

	curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", 
				 CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);
	curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", 
				 CURLFORM_FILE, filename, CURLFORM_END);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_data_cb);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, respone);
	curl_easy_setopt(curl_handle, CURLOPT_HEADER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 60L); 
	curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL,1L);
	
	headerlist = curl_slist_append(headerlist,"Content-Type: multipart/form-data");
	if(token)
	{
		memset(token_str, 0, sizeof(token_str));
		sprintf(token_str, "Authorization: %s", token);
		headerlist = curl_slist_append(headerlist,token_str);
	}
	if(curl_handle) 
	{
	    /* what URL that receives this POST */ 
	    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	    //curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerlist);

	    curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);
	 
	    /* Perform the request, res will get the return code */ 
	    curl_res = curl_easy_perform(curl_handle);
	    /* Check for errors */ 
	    if(curl_res != CURLE_OK)
	      fprintf(stderr, "curl_easy_perform() failed: %s\n",
	              curl_easy_strerror(curl_res));
	 
	    /* always cleanup */ 
	    curl_easy_cleanup(curl_handle);
	 
	    /* then cleanup the formpost chain */ 
	    curl_formfree(formpost);
	    /* free slist */ 
	    curl_slist_free_all(headerlist);
 	}


	return 0;
}


int http_get(char *url, void *data_out)
{
	//创建curl对象 
	http_respone *respone = (http_respone *)data_out;
	CURL *curl_handle = NULL;
	CURLcode curl_res;
	int ret = 0;
	
	//curl初始�?
	curl_handle = curl_easy_init(); 
	if (!curl_handle)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "curl easy init failed");
		return -1;
	}

	curl_easy_setopt(curl_handle, CURLOPT_HEADER,0);	

	curl_easy_setopt(curl_handle, CURLOPT_URL,url);	


	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION,1);	

	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_data_cb); 	

	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, respone);	

	curl_easy_setopt(curl_handle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);	

	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 60L); 	

	curl_easy_setopt(curl_handle,CURLOPT_TIMEOUT, 120);

	curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);	

	
	curl_res = curl_easy_perform(curl_handle); 
	if(curl_res != CURLE_OK)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "curl_easy_perform() failed: %s", curl_easy_strerror(curl_res));
		ret  = -1;
	}


	curl_easy_cleanup(curl_handle);

	return ret;
}

int http_init(void)
{
	CURLcode curl_res;
	
	curl_res = curl_global_init(CURL_GLOBAL_ALL);

	//  printf("param is: %s\n", param);
	if(curl_res != CURLE_OK)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "CURL ERROR : %s", curl_easy_strerror(curl_res));
		return -1;
	}

	return 0;
}

void http_deinit(void)
{
	curl_global_cleanup();
}

int http_body_parse(UINT8 *body, HTTP_BODY *respone)
{
	cJSON *root = NULL;
	cJSON *tmp = NULL;
	cJSON *array = NULL;

	if((body == NULL) || (respone == NULL))
	{
		return ERR_PARAMETER_CHECK_FAILED;
	}

	root = cJSON_Parse((char *)body);
	if(root == NULL)
	{
		return ERR_PARAMETER_CHECK_FAILED;
	}

	/*1.Parse the return code and its description*/
	tmp = NULL;
	tmp = cJSON_GetObjectItem(root, JSON_BODY_RETURN_CODE_STR);
	if((tmp != NULL) && !(cJSON_IsNull(tmp)))
	{
		respone->HttpRetCode = tmp->valueint;
		//DEBUG_PRINT("%s:%d\r\n", JSON_EVIDENCE_RETURN_CODE_STR, respone->SaveRetCode);
	}
	else
	{
		respone->HttpRetCode = ERR_PARAMETER_CHECK_FAILED;
	}

	tmp = NULL;
	tmp = cJSON_GetObjectItem(root, JSON_BODY_RETURN_DESC_STR);
	if((tmp != NULL) && !(cJSON_IsNull(tmp)))
	{
		memset(respone->HttpRetDesc, 0, sizeof(respone->HttpRetDesc));
		sprintf((char *)respone->HttpRetDesc, "%s", tmp->valuestring);
		//DEBUG_PRINT("%s:%s\r\n", JSON_RETURN_DESC_STR, respone->SaveRetDesc);
	}

	/*2.Ckeck the return code to determine whether the query is successful.*/
	if(respone->HttpRetCode != ERR_REQUEST_SUCCESS)
	{
		cJSON_Delete(root); 
		return respone->HttpRetCode;
	}
	else
	{
		respone->HttpRetCode = ERR_SUCCESS;
	}


	tmp = NULL;
	tmp = cJSON_GetObjectItem(root, JSON_BODY_DATA_STR);
	if((tmp != NULL) && !(cJSON_IsNull(tmp)))
	{
		memset(respone->HttpData, 0, sizeof(respone->HttpData));
		sprintf((char *)respone->HttpData, "%s", tmp->valuestring);
	}

	cJSON_Delete(root); 

	return ERR_SUCCESS;
}

static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	return fwrite(pBuffer, nSize, nMemByte, (FILE*)pParam);
}


int http_download_file(char *urlPath, char *localpath)
{
	struct stat file_info;
	int use_resume = 0;
	curl_off_t local_file_len = -1;
	uint8_t timeout = 120U;
	FILE *fp = NULL;

	if (stat(localpath, &file_info) == 0)
	{
		local_file_len = file_info.st_size;
		use_resume = 1;
	}
	
	fp = fopen(localpath, "ab+");
	if (fp == NULL) 
	{
		return 1;
	}
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, urlPath);
	//设置接收数据的回调  
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	//curl_easy_setopt(curl, CURLOPT_INFILESIZE, lFileSize);  
	//curl_easy_setopt(curl, CURLOPT_HEADER, 1);  
	//curl_easy_setopt(curl, CURLOPT_NOBODY, 1);  
	// 设置文件续传的位置给libcurl
	curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, use_resume ? local_file_len : 0);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
	// 设置重定向的最大次数  
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// 设置301、302跳转跟随location  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);  // 设置连接超时，单位秒
	//设置进度回调函数  
	//curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	//curl_easy_getinfo(curl,  CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lFileSize);  
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	//开始执行请求  
	CURLcode retcCode = curl_easy_perform(curl);
	if(retcCode != CURLE_OK)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "curl_easy_perform() failed: %s", curl_easy_strerror(retcCode));
	}
	//查看是否有出错信息  
	//const char* pError = curl_easy_strerror(retcCode);
	//清理curl，和前面的初始化匹配  
	curl_easy_cleanup(curl);
	fclose(fp);
	return retcCode;
}


