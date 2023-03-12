#include <Python.h>

#include "interval_timer.h"

typedef struct {
    PyObject_HEAD
    interval_timer_t itimer;
} IntervalTimerObject;

static void IntervalTimer_dealloc(IntervalTimerObject *self)
{
    interval_timer_destruct(self->itimer);
    self->itimer = NULL;
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *IntervalTimer_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    IntervalTimerObject *self;
    self = (IntervalTimerObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->itimer = NULL;
    }
    return (PyObject *)self;
}

static int IntervalTimer_init(IntervalTimerObject *self, PyObject *args, PyObject *kwds)
{
    self->itimer = interval_timer_construct();
    if (self->itimer == NULL) {
        Py_DECREF(self);
        return 1;
    }
    return 0;
}

static PyObject* py_interval_timer_settime(IntervalTimerObject *self, PyObject *args)
{
    long interval;
    if (!PyArg_ParseTuple(args, "l", &interval)){
        Py_RETURN_FALSE;
    }
    if (interval_timer_settime(self->itimer, interval) != 0) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject* py_interval_timer_wait(IntervalTimerObject *self, PyObject *args)
{
    interval_timer_wait(self->itimer);
    Py_RETURN_NONE;
}

static PyMethodDef IntervalTimer_methods[] = {
    { "settime", (PyCFunction)py_interval_timer_settime, METH_VARARGS, "wait" },
    { "wait", (PyCFunction)py_interval_timer_wait, METH_NOARGS, "wait" },
    { NULL, NULL, 0, NULL },
};

static PyTypeObject IntervalTimerType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "interval_timer.IntervalTimer",
    .tp_doc = "IntervalTimer objects",
    .tp_basicsize = sizeof(IntervalTimerObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = IntervalTimer_new,
    .tp_init = (initproc) IntervalTimer_init,
    .tp_dealloc = (destructor) IntervalTimer_dealloc,
    .tp_methods = IntervalTimer_methods,
};

static struct PyModuleDef interval_timer_module = {
    PyModuleDef_HEAD_INIT,
    "interval_timer",
    "Python3 C API Module(IntervalTimer)",
    -1,
};

PyMODINIT_FUNC PyInit_interval_timer(void)
{
    PyObject *m;
    if (PyType_Ready(&IntervalTimerType) < 0) {
        return NULL;
    }
    m = PyModule_Create(&interval_timer_module);
    if (m == NULL) {
        return NULL;
    }
    Py_INCREF(&IntervalTimerType);
    if (PyModule_AddObject(m, "IntervalTimer", (PyObject *)&IntervalTimerType) < 0) {
        Py_DECREF(&IntervalTimerType);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}
