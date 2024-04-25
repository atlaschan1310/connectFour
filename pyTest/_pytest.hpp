//
//  _pytest.h
//  Network_C
//
//  Created by yang qian on 2024/4/8.
//

#ifndef _pytest_h
#define _pytest_h
#include <Python.h>
#include <stdlib.h>

/* Docstrings */
static char module_docstring[] =
"connectFour solver written in Cpp";

static char solve_docstring[] = "Runs the solver with given board status provided as a string";

/* Available functions */
static PyObject *_solve(PyObject *self, PyObject *args, PyObject *kwargs);

static PyMethodDef module_methods[] = {
    {"solve", (PyCFunction)_solve, METH_VARARGS | METH_KEYWORDS, solve_docstring},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef _connectFour = {
    PyModuleDef_HEAD_INIT,
    "_connectFour",
    module_docstring,
    -1,
    module_methods
};

#endif /* _pytest_h */
