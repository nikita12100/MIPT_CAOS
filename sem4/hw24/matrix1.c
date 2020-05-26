#include <Python.h>


// add 0 for row
static void matrix_row_layer(PyObject * str, double * raw, int size_mtrx) {
    int list_size = PyList_Size(str);

    for (int i = 0; i < size_mtrx; i++) {
        if (i >= list_size) {
            for (int j = i; j < size_mtrx; ++j) {
                raw[j] = 0;
            }
            break;
        }
        raw[i] = PyFloat_AsDouble(PyList_GetItem(str, (Py_ssize_t) i));
    }
}

static void matrix_str_layer(PyObject * str, double * raw, int size_mtrx) {
    int list_size = PyList_Size(str);

    for (int i = 0; i < size_mtrx; i++) {
        if (list_size <= i) {
            for (int j = i * size_mtrx; j < size_mtrx * size_mtrx; ++j) {
                raw[j] = 0;
            }
            break;
        }
        PyObject* sub_list = PyList_GetItem(str, (Py_ssize_t) i);
        matrix_row_layer(sub_list, raw + i * size_mtrx, size_mtrx);
    }
}

void multiply_matrix(double* matrix1, double* matrix2, double* result, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            result[i * size + j] = 0;
            for (int k = 0; k < size; ++k) {
                result[i * size + j] = result[i * size + j] + matrix1[i * size + k] * matrix2[k * size + j];
            }
        }
    }
}

PyObject * construct_matrix(double* matrix, int size) {
    PyObject * py_matrix = PyList_New(0);
    for (int i = 0; i < size; ++i) {
        PyObject * py_row = PyList_New(0);
        for (int j = 0; j < size; ++j) {
            PyList_Append(py_row, PyFloat_FromDouble(matrix[i * size + j]));
        }
        PyList_Append(py_matrix, py_row);
    }

    return py_matrix;
}

static PyObject* dot(PyObject* self, PyObject* args, PyObject* kwargs) {
    int size_mtrx;
    PyObject * first_m;
    PyObject * second_m;
    if (!PyArg_ParseTuple(args, "KOO", &size_mtrx, &first_m, &second_m)) {
        return NULL;
    }

    double * matrix1 = calloc(size_mtrx * size_mtrx, sizeof(PyObject *));
    double * matrix2 = calloc(size_mtrx * size_mtrx, sizeof(PyObject *));
    double * matrix3 = calloc(size_mtrx * size_mtrx, sizeof(PyObject *));

    matrix_str_layer(first_m, matrix1, size_mtrx);
    matrix_str_layer(second_m, matrix2, size_mtrx);

    multiply_matrix(matrix1, matrix2, matrix3, size_mtrx);

    PyObject * m3 = construct_matrix(matrix3, size_mtrx);
    free(matrix1);
    free(matrix2);
    free(matrix3);
    return m3;
}

static PyMethodDef methods[] = {
        {"dot", (PyCFunction)dot, METH_VARARGS, "dot"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
        PyModuleDef_HEAD_INIT, "matrix", "Test module", -1, methods
};

PyMODINIT_FUNC PyInit_matrix(void) {
    PyObject* mod = PyModule_Create(&module);
    return mod;
}

//[[7.0, 10.0, 0.0], [15.0, 22.0, 0.0], [0.0, 0.0, 0.0]]
