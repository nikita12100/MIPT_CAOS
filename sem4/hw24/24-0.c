#include <Python.h>

// IDE generate matrix.so file witch used in program
// $ python3 pytest

double* pymatrix_to_cmatrix(PyObject* pymatrix, int size) {
    double* cmatrix = calloc(size * size, sizeof(PyObject*));
    int size_pylist = PyList_Size(pymatrix);

    for (int i = 0; i < size; ++i) {
        if (size_pylist <= i) {
            for (int j = i * size; j < size * size; ++j)
                cmatrix[j] = 0;
            break;
        }

        // add null missing row
        PyObject* missing_row = PyList_GetItem(pymatrix, (Py_ssize_t) i);
        double* shift = cmatrix + i * size;
        for (int k = 0; k < size; ++k) {
            if (k >= size_pylist) {
                for (int s = k; s < size; ++s)
                    shift[s] = 0;
                break;
            }
            shift[k] = PyFloat_AsDouble(PyList_GetItem(missing_row, (Py_ssize_t) k));
        }
    }

    return cmatrix;
}

//    python example code
//    A = [[1,2], [3,4]]
//    B = [[1,2], [3,4]]
//    matrix.dot(2, A, B)
static PyObject* dot(PyObject* self, PyObject* argsTuple, PyObject* argsVar) {
    int size;
    PyObject* A;
    PyObject* B;
    PyArg_ParseTuple(argsTuple, "KOO", &size, &A, &B);

    double* matrix_a;
    // calloc here
    matrix_a = pymatrix_to_cmatrix(A, size);

    double* matrix_b;
    //and calloc here
    matrix_b = pymatrix_to_cmatrix(B, size);

    // multiplication of matrix
    /// calloc !!!
    double* matrix_res = calloc(size * size, sizeof(PyObject*));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix_res[j + i * size] = 0;
            for (int k = 0; k < size; ++k)
                matrix_res[j + i * size] = matrix_res[j + i * size] + matrix_a[k + i * size] * matrix_b[j + k * size];
        }
    }
    free(matrix_a);
    free(matrix_b);

    // and transform cmatrix to pymatrix, to return
    PyObject* pymatrix_res = PyList_New(0);
    for (int i = 0; i < size; ++i) {
        PyObject* pymatrix_curr_row = PyList_New(0);
        for (int j = 0; j < size; ++j)
            PyList_Append(pymatrix_curr_row, PyFloat_FromDouble(matrix_res[j + i * size]));
        PyList_Append(pymatrix_res, pymatrix_curr_row);
    }
    free(matrix_res);

    return pymatrix_res;
}

static PyMethodDef methods[] = {
        {.ml_name = "dot", .ml_meth = (PyCFunction)dot,
                .ml_flags = METH_VARARGS,
                .ml_doc = "Algebraic multiplication of "
                          "two square real matrices of"
                          " a given size."
        },
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef matrix_module = {
        PyModuleDef_HEAD_INIT,
        "matrix",                        // module name to use with Python import statements
        "Provides dot function, but faster.",  // module description
        -1,
        methods                   // structure that defines the methods of the module
};

//Функция должна создавать и возвращать объект модуля,
// по аналогии с расширением интерпретатора встроенным модулем.
PyMODINIT_FUNC PyInit_matrix() {
    return PyModule_Create(&matrix_module);
}

//find_package(PythonLibs 3.6 REQUIRED)
//include_directories(${PYTHON_INCLUDE_DIRS})
//python_add_module(matrix 24-0.c)
//target_link_libraries(matrix ${PYTHON_LIBRARIES})

/// gcc 24-0.c -o n -I/usr/include/python3.8 -lpython3.8