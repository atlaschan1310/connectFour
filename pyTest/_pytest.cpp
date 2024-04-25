//
//  _pytest.cpp
//  Network_C
//
//  Created by yang qian on 2024/4/19.
//
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include "_pytest.hpp"
#include "ConnectFour.hpp"

PyMODINIT_FUNC PyInit_connectFour(void){
    return PyModule_Create(&_connectFour);
}

static PyObject *_solve(PyObject *self, PyObject *args, PyObject *kwargs) {
    char* p;
    int depth;
    static char* kwlist[] = {"p", "d", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "si", kwlist, &p, &depth)) {
        return NULL;
    }
    std::string s = p;
    connectFour::connectFour four(depth);
    int nextMove = four.solve(s);
    PyObject *res = Py_BuildValue("i", nextMove);
    return res;
}
