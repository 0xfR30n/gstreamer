/* -*- Mode: C; ; c-file-style: "python" -*- */
/* gst-python
 * Copyright (C) 2004 Johan Dahlin
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
 * Author: Johan Dahlin <johan@gnome.org>
 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <Python.h>
#include <gst/gst.h>

#include "pygobject.h"

#if (defined HAVE_OLD_PYGTK && (PY_VERSION_HEX < 0x02030000))
  typedef destructor freefunc;
#endif

typedef struct {
     PyGObject *pad;
     GClosure *link_function;
     GClosure *event_function;
     GClosure *chain_function;
     GClosure *get_function; 
     GClosure *getcaps_function; 
} PyGstPadPrivate;

typedef struct {
     PyObject *func, *data;
} PyGstCustomNotify;

gboolean pygst_data_from_pyobject(PyObject *object, GstData **data);
PyObject *pygst_data_to_pyobject(GstData *data);


#endif /* __COMMON_H__ */
