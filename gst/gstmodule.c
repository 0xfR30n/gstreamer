/* -*- Mode: C; ; c-file-style: "k&r"; c-basic-offset: 4 -*- */
/* gst-python
 * Copyright (C) 2002 David I. Lehn
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 * Author: David I. Lehn <dlehn@users.sourceforge.net>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>

/* include this first, before NO_IMPORT_PYGOBJECT is defined */
#include <pygobject.h>
#include <gst/gst.h>
#include <gst/gstversion.h>

void pygst_register_classes (PyObject *d);
void pygst_add_constants(PyObject *module, const gchar *strip_prefix);
void _pygst_register_boxed_types(PyObject *moddict);
		
extern PyMethodDef pygst_functions[];
extern GSList *mainloops;
extern void _pygst_main_quit(void);
extern PyObject *PyGstExc_LinkError;


/* This is a timeout that gets added to the mainloop to handle SIGINT (Ctrl-C)
 * Other signals get handled at some other point where transition from
 * C -> Python is being made.
 */
static gboolean
python_do_pending_calls(gpointer data)
{
    gboolean quit = FALSE;
    PyGILState_STATE state;

    if (PyOS_InterruptOccurred()) {
	 state = pyg_gil_state_ensure();
	 PyErr_SetNone(PyExc_KeyboardInterrupt);
	 pyg_gil_state_release(state);
	 quit = TRUE;
    }

    if (quit && mainloops != NULL)
	 _pygst_main_quit();

    return TRUE;
}

static void
sink_gstobject(GObject *object)
{
     if (GST_OBJECT_FLOATING(object)) {
	  g_object_ref(object);
	  gst_object_sink(GST_OBJECT(object));
     }
}

DL_EXPORT(void)
init_gst (void)
{
     PyObject *m, *d;
     PyObject *av, *tuple;
     int argc, i;
     char **argv;

     init_pygobject ();

     /* pull in arguments */
     av = PySys_GetObject ("argv");
     if (av != NULL) {
	  argc = PyList_Size (av);
	  argv = g_new (char *, argc);
	  for (i = 0; i < argc; i++)
	       argv[i] = g_strdup (PyString_AsString (PyList_GetItem (av, i)));
     } else {
          /* gst_init_check does not like argc == 0 */
	  argc = 1;
	  argv = g_new (char *, argc);
	  argv[0] = g_strdup("");
     }
     if (!gst_init_check (&argc, &argv)) {
	  if (argv != NULL) {
	       for (i = 0; i < argc; i++)
                    g_free (argv[i]);
	       g_free (argv);
	  }
	  PyErr_SetString (PyExc_RuntimeError, "can't initialize module gst");
	  setlocale(LC_NUMERIC, "C");
	  return;
     }
     
     setlocale(LC_NUMERIC, "C");
     if (argv != NULL) {
	  PySys_SetArgv (argc, argv);
	  for (i = 0; i < argc; i++)
	       g_free (argv[i]);
	  g_free (argv);
     }

     _pygst_register_boxed_types (NULL);
     pygobject_register_sinkfunc(GST_TYPE_OBJECT, sink_gstobject);

     m = Py_InitModule ("_gst", pygst_functions);
     d = PyModule_GetDict (m);

     /* gst+ version */
     tuple = Py_BuildValue ("(iii)", GST_VERSION_MAJOR, GST_VERSION_MINOR,
			    GST_VERSION_MICRO);
     PyDict_SetItemString(d, "gst_version", tuple);    
     Py_DECREF(tuple);
     
     /* gst-python version */
     tuple = Py_BuildValue ("(iii)", PYGST_MAJOR_VERSION, PYGST_MINOR_VERSION,
			    PYGST_MICRO_VERSION);
     PyDict_SetItemString(d, "pygst_version", tuple);
     Py_DECREF(tuple);

     PyModule_AddIntConstant(m, "SECOND", GST_SECOND);
     PyModule_AddIntConstant(m, "MSECOND", GST_MSECOND);
     PyModule_AddIntConstant(m, "NSECOND", GST_NSECOND);

     /* LinkError exception */
     PyGstExc_LinkError = PyErr_NewException("gst.LinkError",
					     PyExc_RuntimeError,
					     NULL);
     PyDict_SetItemString(d, "LinkError", PyGstExc_LinkError);

     pygst_register_classes (d);
     pygst_add_constants (m, "GST_");

     g_timeout_add_full (0, 100, python_do_pending_calls, NULL, NULL);
     
     if (PyErr_Occurred ()) {
	  Py_FatalError ("can't initialize module gst");
     }
}
