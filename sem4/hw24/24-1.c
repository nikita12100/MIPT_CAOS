#include "openssl/evp.h"
#include "openssl/err.h"
#include "stdio.h"
#include <sys/mman.h>
#include "string.h"
#include <Python.h>

// IDE generate aes256.so file witch used in program
// $ python3 pytest

//def decode(encrypted: bytes(), password: str() = None) -> bytes:
//  pass
//def set_default_password(password: str()) -> None:
//  pass

char msg_PyArg_ParseTuple[256] = "Problem with func arguments.";
char msg_EVP_BytesToKey[256] = "Wrong key or init vector for decode.";
char msg_EVP_DecryptInit[256] = "Problems with initializing content.";
char msg_EVP_DecryptUpdate[256] = "Problems with decrypting.";
char msg_EVP_DecryptFinal[256] = "Problems with decrypting.";
char msg_EVP_CIPHER_CTX_cleanup[256] = "Problems with decrypting.";

struct password {
    int size;
    unsigned char* pswd;
};

struct password password;

// decoded = aes256.decode(encoded, "qwerty")
static PyObject* decode(PyObject* self, PyObject* argsTuple, PyObject* argsVar) {
    // initialize arguments
    int size = 0;
    unsigned char* encoded;
    unsigned char* my_password = password.pswd; // remember password
    if ( 0 == PyArg_ParseTuple(argsTuple, "y#|s", &encoded, &size, &my_password)){
        PyErr_SetString(PyExc_ValueError, msg_PyArg_ParseTuple);
        return 0;
    }

    /// start decoding here
    //  соль для усложения шифрования
    unsigned char* salt = encoded + 8;
    // ключ
    unsigned char key[256];
    // начальный вектор
    unsigned char initialize_vector[128];
    unsigned char* decoded = malloc(size + EVP_MAX_IV_LENGTH);
    int decoded_size = 0;
    // создадим ключ, начальный вектор из пароля и соли
    if ( 0 == EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt,
            my_password, strlen(my_password), 1, key, initialize_vector)){
        PyErr_SetString(PyExc_ValueError, msg_EVP_BytesToKey);
        return 0;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (0 == EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key, initialize_vector)) {
        PyErr_SetString(PyExc_ValueError, msg_EVP_DecryptInit);
        return 0;
    }

    unsigned char* input = encoded + 16;
    int len_out = size - 16;
    if (0 == EVP_DecryptUpdate(ctx, decoded, &decoded_size, input, len_out)) {
        PyErr_SetString(PyExc_ValueError, msg_EVP_DecryptUpdate);
        return 0;
    }

    int rest_decode = 0;
    if (0 == EVP_DecryptFinal(ctx, decoded + decoded_size, &rest_decode)) {
        PyErr_SetString(PyExc_ValueError, msg_EVP_DecryptFinal);
        return 0;
    }
    decoded_size += rest_decode;

//    EVP_CIPHER_CTX_reset(ctx); for openssl v1.0.1f
    if (0 == EVP_CIPHER_CTX_cleanup(ctx)) {
        PyErr_SetString(PyExc_ValueError, msg_EVP_CIPHER_CTX_cleanup);
        return 0;
    }
    /// finish decoding here

    // python object to return
    PyObject* pydecoded = PyBytes_FromStringAndSize(decoded, decoded_size);
    free(decoded);

    return pydecoded;
}

//  aes256.set_default_password("qwerty")
static PyObject* set_default_password(PyObject* self, PyObject* argsTuple, PyObject* argsVar) {
    // clean old password
    munmap(password.pswd, password.size);
    // and write new password
    if (0 == PyArg_ParseTuple(argsTuple, "s#", &password.pswd, &password.size)) {
        PyErr_SetString(PyExc_ValueError, msg_PyArg_ParseTuple);
        return 0;
    }

    // set password and nothing to return
    Py_RETURN_NONE;
}

static PyMethodDef methods[] = {
        {.ml_name = "decode", .ml_meth = (PyCFunction)decode,
                .ml_flags = METH_VARARGS,
                .ml_doc = "Decode message."
        },
        {.ml_name = "set_default_password", .ml_meth = (PyCFunction)set_default_password,
                .ml_flags = METH_VARARGS,
                .ml_doc = "Set default password from string."
        },
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef aes256_module = {
        PyModuleDef_HEAD_INIT,
        "aes256",                        // module name to use with Python import statements
        "Provides decode and set_default_password"
        " function, but faster.",  // module description
        -1,
        methods                   // structure that defines the methods of the module
};

//Функция должна создавать и возвращать объект модуля,
// по аналогии с расширением интерпретатора встроенным модулем.
PyMODINIT_FUNC PyInit_aes256() {
    return PyModule_Create(&aes256_module);
}

//find_package(PythonLibs 3.6 REQUIRED)
//include_directories(${PYTHON_INCLUDE_DIRS})
//python_add_module(aes256 24-1.c)
//target_link_libraries(aes256 ${PYTHON_LIBRARIES})

/// gcc 24-1.c -o n -I/usr/include/python3.8 -lpython3.8

//example of usage
//import aes256
//
//with open("data.aes", "rb") as f:
//  encoded = f.read()
//decoded = aes256.decode(encoded, "qwerty")
//print(decoded)
//aes256.set_default_password("qwerty")
//print(aes256.decode(encoded))
//try:
//  aes256.decode(encoded, "wrong password")
//except BaseException as e:
//  print("Can't decode: {}".format(e))