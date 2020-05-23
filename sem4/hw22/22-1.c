#include "openssl/sha.h"
#include "openssl/evp.h"
#include "stdio.h"
#include "stdint.h"
#include "inttypes.h"
#include "limits.h"
#include "unistd.h"
#include "string.h"

enum {HashSize = 256/CHAR_BIT };

int main(int argc, char** argv)
{
    EVP_CIPHER *type;
    //  соль для усложения шифрования
    unsigned char salt[8];
    // зашифрованные данные
    const char* data = argv[1];
    // ключ
    unsigned char key[256];
    // начальный вектор
    unsigned char initialize_vector[128];

    data;
    // первые 8 байт оглавление
    read(0, salt, sizeof(salt));    // чтоб "пройти оглавление" и считывать дальше саму соль
    // тут считываем уже саму соль
    read(0, salt, sizeof(salt));

    // создадим ключ, начальный вектор из пароля и соли
    EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt, (const unsigned char*)data, (int)strlen(data), 1, key, initialize_vector);

    // Создание контекста
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key, initialize_vector);


    unsigned char* in = (unsigned char*)malloc(sizeof(char)*5256);
    unsigned char* out = (unsigned char*)malloc(sizeof(char)*75526);
    int len_in, len_out;
    while ((len_in = read(0,in, sizeof(in))) > 0){
        EVP_DecryptUpdate(ctx, out, &len_out, in, len_in);
        write(1, out, len_out); // print decrypted
    }
    EVP_DecryptFinal(ctx, out, &len_out);
    write(1, out, len_out); // print decrypted
    printf("\n");
    free(out);
    free(in);
}

// flags to compile -lcrypto -lssl