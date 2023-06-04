#ifndef _TOKEN_H_
#define _TOKEN_H_

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "AnyID_SM5001_Config.h"

#define DEV_TOKEN_VERISON_STR "2018-10-31"
#define DEV_STOP_TIME          "1903568910"
#define DEV_TOKEN_SIG_METHOD_SHA256 "sha256"

#define DEV_PRODUCT_ID          "598243"
/*********************************************************************
 * API FUNCTIONS
 */
   
extern char token[256];
extern char accessKey[];
extern char devName[];
   
u8 Base64_Encode(char *pInData, u32 inLen, char *pOutData, u32 *pOutLen);
u8 Base64_Decode(char *pInData, u32 inLen, char *pOutData, u32 *pOutLen);
u32 dev_token_generate(u8 *token, u8 *dev_name, char *access_key);

void hmac_sha256(const unsigned char *data, u8 len, const unsigned char *key, int len_key, unsigned char *out) ;
#endif /* _BASE64_H_ */
