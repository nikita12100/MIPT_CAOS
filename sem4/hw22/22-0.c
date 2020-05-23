#include "openssl/sha.h"
#include "stdio.h"
#include "stdint.h"
#include "inttypes.h"
#include "limits.h"
#include "unistd.h"

enum {HashSize = 512/CHAR_BIT };

int main(int argc, char** argv)
{
    SHA512_CTX context;
    SHA512_Init(&context);
    char buffer[HashSize];
    size_t n;

    while ((n = read(0,buffer, sizeof(buffer))) > 0){
        SHA512_Update(&context, buffer, n);
    }
    char hash[HashSize];
    SHA512_Final(hash, &context);
    printf("0x");
    uint32_t cur_byte;
    for(size_t i=0; i < sizeof(hash); ++i){
        cur_byte =  (uint32_t)hash[i] & 0xff;
        printf("%02"PRIx32, cur_byte);
    }
    printf("\n");
}