#ifndef _LZW
#define _LZW

#ifdef __cplusplus
extern "C" {
#endif

int lzwdecoding(char *src, char *dest, int len);
int lzwencoding(char *src, char *dest, int len);

#ifdef __cplusplus
}
#endif

#endif
