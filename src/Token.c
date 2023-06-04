/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stddef.h>
#include "Token.h"

/*********************************************************************
 * LOCAL VARIABLES
 */
static char s_base64Table[] = 
{
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z','a','b','c','d',
    'e','f','g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v','w','x',
    'y','z','0','1','2','3','4','5','6','7',
    '8','9','+', '/', '\0'
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 @brief Base64编码
 @param pInData -[in] 源字符串
 @param inLen -[in] 源字符串长度
 @param pOutData -[out] 编码后字符串
  @param pOutLen -[out] 解码后字符串长度
 @return 1 - 成功；0 - 失败
*/

char token[256] = {0};

char accessKey[] = "MVhDTnBOb0dmak9tWHB4M3B1cFI5aG9UZGdZcFdqeXA=";
char devName[] = "865396056277961";
u8 Base64_Encode(char *pInData, u32 inLen, char *pOutData, u32 *pOutLen)
{
    if(NULL == pInData || 0 == inLen)
    {
        return 0;
    }

    u32 i = 0;
    u32 j = 0;
    u32 temp = 0;
    // 3字节一组进行转换
    for(i = 0; i < inLen; i += 3) 
    {
        // 获取第一个6位
        temp = (*(pInData + i) >> 2) & 0x3F;
        *(pOutData + j++) = s_base64Table[temp];

        // 获取第二个6位的前两位
        temp = (*(pInData + i) << 4) & 0x30;
        // 如果只有一个字符，那么需要做特殊处理
        if(inLen <= (i + 1)) 
        {
            *(pOutData + j++) = s_base64Table[temp];
            *(pOutData + j++) = '=';
            *(pOutData + j++) = '=';
            break;
        }
        // 获取第二个6位的后四位
        temp |= (*(pInData + i + 1) >> 4) & 0x0F;
        *(pOutData + j++) = s_base64Table[temp];

        // 获取第三个6位的前四位
        temp = (*(pInData+ i + 1) << 2) & 0x3C;
        if(inLen <= (i + 2))
        {
            *(pOutData + j++) = s_base64Table[temp];
            *(pOutData + j++) = '=';
            break;
        }
        // 获取第三个6位的后两位
        temp |= (*(pInData + i + 2) >> 6) & 0x03;
        *(pOutData + j++) = s_base64Table[temp];

        // 获取第四个6位
        temp = *(pInData + i + 2) & 0x3F;
        *(pOutData + j++) = s_base64Table[temp];
    }
    *(pOutData + j) = '\0';
    // 编码后的长度
    *pOutLen = inLen * 8 / 6;
    return 1;
}

/**
 @brief Base64解码
 @param pInData -[in] 源字符串
 @param inLen -[in] 源字符串长度
 @param pOutData -[out] 解码后字符串
 @param pOutLen -[out] 解码后字符串长度
 @return 1 - 成功；0 - 失败
*/
u8 Base64_Decode(char *pInData, u32 inLen, char *pOutData, u32 *pOutLen)
{
    if(NULL == pInData || 0 == inLen || inLen % 4 != 0)
    {
        return 0;
    }

    u32 i = 0;
    u32 j = 0;
    u32 k = 0;
    char temp[4] = "";
    // 4字节一组进行转换
    for(i = 0; i < inLen; i += 4)
    {
        // 找到在编码索引表中对应的值
        for(j = 0; j < 64; j++) 
        {
            if(*(pInData + i) == s_base64Table[j])
            {
                temp[0] = j;
            }
        }        
        for(j = 0; j < 64; j++)
        {
            if(*(pInData + i + 1) == s_base64Table[j])
            {
                temp[1] = j;
            }
        }
        for(j = 0; j < 64; j++)
        {
            if(*(pInData + i + 2) == s_base64Table[j])
            {
                temp[2] = j;
            }
        }
        for(j = 0; j < 64; j++) 
        {
            if(*(pInData + i + 3) == s_base64Table[j]) 
            {
                temp[3] = j;
            }
        }

        // 获取第一个6位和第二个6位的前两位组成一个8位
        *(pOutData + k++) = ((temp[0] << 2) & 0xFC) | ((temp[1] >> 4) & 0x03);
        if(*(pInData + i + 2) == '=')
        {
            break;
        }
        // 获取第二个6位的前四位和第三个6位的前四位组成一个8位
        *(pOutData + k++) = ((temp[1] << 4) & 0xF0) | ((temp[2] >> 2) & 0x0F);
        if(*(pInData + i + 3) == '=')
        {
            break;
        }
        // 获取第三个6位的后两位和第四个6位组成一个8位
        *(pOutData + k++) = ((temp[2] << 6) & 0xF0) | (temp[3] & 0x3F);
    }
    *pOutLen = k;
    return 1;
}


/****************************************************END OF FILE****************************************************/



/*

u32 dev_token_generate(u8 *token, u8 *dev_name, char *access_key)
{
    u8 base64_data[64] = { 0 };
    u8 str_for_sig[64] = { 0 };
    u8 sign_buf[128] = { 0 };
    u32 base64_data_len = sizeof(base64_data);
    u8 *sig_method_str = NULL;
    u32 sign_len = 0;
    u8 i = 0;
    u8 *tmp = NULL;
    //Hmac hmac;

    sprintf(token, "version=%s", DEV_TOKEN_VERISON_STR);

    sprintf(token + strlen(token), "&res=products%%2F%s%%2Fdevices%%2F%s", DEV_PRODUCT_ID, dev_name);

    sprintf(token + strlen(token), "&et=%u", DEV_STOP_TIME);

    u8 bok = 0;
    bok =  Base64_Decode(access_key, strlen(access_key), base64_data, &base64_data_len);

    sig_method_str = DEV_TOKEN_SIG_METHOD_SHA256 ;
    sign_len = 16;
    //hmac_sha256(&access_key, sign_len, base64_data_len ,base64_data);




    sprintf(token + strlen(token), "&method=%s", sig_method_str);

    sprintf(str_for_sig, "%u\n%s\nproducts/%s/devices/%s\n%s", DEV_STOP_TIME, sig_method_str, DEV_PRODUCT_ID, dev_name, DEV_TOKEN_VERISON_STR);
    //HmacUpdate(&hmac, str_for_sig, strlen(str_for_sig));
   // HmacFinal(&hmac, sign_buf);

    memset(base64_data, 0, sizeof(base64_data));
    base64_data_len = sizeof(base64_data);
    Base64_Encode(sign_buf, sign_len, base64_data, &base64_data_len);

    strcat(token, "&sign=");
    tmp = token + strlen(token);

    for(i = 0; i < base64_data_len; i++)
    {
        switch(base64_data[i])
        {
        case '+':
            strcat(tmp, "%2B");
            tmp += 3;
            break;
        case ' ':
            strcat(tmp, "%20");
            tmp += 3;
            break;
        case '/':
            strcat(tmp, "%2F");
            tmp += 3;
            break;
        case '?':
            strcat(tmp, "%3F");
            tmp += 3;
            break;
        case '%':
            strcat(tmp, "%25");
            tmp += 3;
            break;
        case '#':
            strcat(tmp, "%23");
            tmp += 3;
            break;
        case '&':
            strcat(tmp, "%26");
            tmp += 3;
            break;
        case '=':
            strcat(tmp, "%3D");
            tmp += 3;
            break;
        default:
            *tmp = base64_data[i];
            tmp += 1;
            break;
        }
    }

    return *tmp;
}


*/


