#include <stdio.h>
#include <Python.h>

static PyObject* dot(PyObject* self, PyObject* argsTuple, PyObject* argsDict) {
    PyObject *ret = PyUnicode_FromString("Hello"); // ret->ob_refcnt=1

    return ret; // OK
}

static PyMethodDef methods[] = {
        {.ml_name = "dot", .ml_meth = (PyCFunction)dot,
                .ml_flags = METH_KEYWORDS,
                .ml_doc = "Algebraic multiplication of "
                          "two square real matrices of"
                          " a given size"
        },
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef matrix_module = {
        PyModuleDef_HEAD_INIT,
        "matrix",                        // Module name to use with Python import statements
        "Provides some functions, but faster",  // Module description
        0,
        methods                   // Structure that defines the methods of the module
};

PyMODINIT_FUNC PyInit_fasmatrix() {
    return PyModule_Create(&methods);
}

//find_package(PythonLibs 3.6 REQUIRED)
//include_directories(${PYTHON_INCLUDE_DIRS})
//python_add_module(matrix 24-0.c)
//target_link_libraries(matrix ${PYTHON_LIBRARIES})

/// gcc 24-0.c -o n -I/usr/include/python3.8 -lpython3.8