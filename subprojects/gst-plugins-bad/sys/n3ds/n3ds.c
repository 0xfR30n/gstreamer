#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "n3dsimagesink.h"

#include <gst/gst.h>

GST_DEBUG_CATEGORY (gst_n3ds_debug);
#define GST_CAT_DEFAULT gst_n3ds_debug

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  GST_DEBUG_CATEGORY_INIT (gst_n3ds_debug, "n3ds", 0, "gst-n3ds");

  ret |=
      gst_element_register (plugin, "n3dsimagesink", GST_RANK_NONE,
      N3DS_TYPE_IMAGE_SINK);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    n3ds,
    "Elements for access features on the Nintendo 3DS",
    plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
