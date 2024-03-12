#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "basic_types.h"

#define JSON_BODY_RETURN_CODE_STR		"returnCode"
#define JSON_BODY_RETURN_DESC_STR		"returnDesc"
#define JSON_BODY_DATA_STR				"data"



typedef struct http_respone_t
{
	char *data;// 文件数据保存在内存中的指�? 用完后手动free 
	int data_len;
}http_respone;

/*HTTP content type.				*/
/*Because the uploading of files	*/ 
/*needs to be realized by post form,*/ 
/*this structure is defined here.	*/
typedef enum
{
	HTTP_CONTENT_TEXT_PLAIN,
	HTTP_CONTENT_OCTET_STREAM,
}HTTP_CONTENT_TYPE;


/*HTTP post form structure. 		*/
/*Because the uploading of files	*/ 
/*needs to be realized by post form,*/ 
/*this structure is defined here.	*/
typedef struct _HTTP_POST_FORM
{
	HTTP_CONTENT_TYPE content_type;
	UINT8 *content;
	UINT8 content_name[64];
	UINT8 content_encoding[64];
}HTTP_POST_FORM, *P_HTTP_POST_FORM;

/*poe save result data structure*/
 typedef struct _HTTP_BODY
 {
	UINT32	 	 HttpRetCode;				 		/*Return code of Poe save	 */
	UINT8		 HttpRetDesc[RETURN_DESC_LEN];	/*Return code description	 */
	UINT8		 HttpData[RETURN_DESC_LEN];
 }HTTP_BODY ,*P_HTTP_BODY; 



int http_post_file_octetstream(char *url, HTTP_POST_FORM *form, UINT32 form_num, 
				   				void *data_out, char *token);
int http_post(char *url, char *data_in, int len, void *data_out, char *token);
int http_get(char *url, void *data_out);
int http_init(void);
void http_deinit();
int http_body_parse(UINT8 *body, HTTP_BODY *respone);
int http_post_file_multipart(char *url, UINT8 *filename, void *data_out, char *token);
int http_download_file(char *urlPath, char *localpath);


#endif

