#include "n3dsimagesink.h"

#include <3ds.h>

#define N3DS_IMAGE_SINK_CAPS "video/x-raw, " \
        "format = (string) { BGR }, " \
        "framerate = (fraction) [ 0, MAX ], " \
        "width = (int) 400, " "height = (int) 240 "

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (N3DS_IMAGE_SINK_CAPS));

GST_DEBUG_CATEGORY_STATIC (n3ds_image_sink_debug);
#define GST_CAT_DEFAULT n3ds_image_sink_debug

enum
{
  PROP_0,
};

struct _N3DSImageSink
{
  GstBaseSink element;
};

struct _N3DSImageSinkClass
{
  GstBaseSinkClass parent_class;
};

#define n3ds_image_sink_parent_class parent_class
G_DEFINE_TYPE (N3DSImageSink, n3ds_image_sink, GST_TYPE_BASE_SINK);

GST_ELEMENT_REGISTER_DEFINE_WITH_CODE (n3dsimagesink, "n3dsimagesink",
    GST_RANK_NONE, N3DS_TYPE_IMAGE_SINK,
    GST_DEBUG_CATEGORY_INIT (n3ds_image_sink_debug, "n3dsimagesink", 0,
        "N3DS Image Sink"));

static void
rotate90 (unsigned char *buffer, const unsigned int width,
    const unsigned int height)
{
  const unsigned int sizeBuffer = width * height * 3;
  unsigned char *tempBuffer = g_malloc (sizeBuffer);

  for (int y = 0, destinationColumn = height - 1; y < height;
      ++y, --destinationColumn) {
    int offset = y * width;

    for (int x = 0; x < width; x++) {

      for (int i = 0; i < 3; i++)
        tempBuffer[(x * height + destinationColumn) * 3 + i] =
            buffer[(offset + x) * 3 + i];
    }
  }

  memcpy (buffer, tempBuffer, sizeBuffer);
  g_free (tempBuffer);
}

static GstFlowReturn
n3ds_image_sink_render (GstBaseSink * bsink, GstBuffer * buffer)
{
  GstMapInfo map;

  if (!gst_buffer_map (buffer, &map, GST_MAP_READWRITE))
    return GST_FLOW_ERROR;

  rotate90 (map.data, 400, 240);
  memcpy (gfxGetFramebuffer (GFX_TOP, GFX_LEFT, NULL, NULL), map.data,
      map.size);

  gfxFlushBuffers ();
  gspWaitForVBlank ();
  gfxSwapBuffers ();

  gst_buffer_unmap (buffer, &map);

  return GST_FLOW_OK;
}

static void
n3ds_image_sink_finalize (GObject * obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
n3ds_image_sink_class_init (N3DSImageSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbase_sink_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstbase_sink_class = GST_BASE_SINK_CLASS (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (n3ds_image_sink_finalize);
  gstbase_sink_class->render = GST_DEBUG_FUNCPTR (n3ds_image_sink_render);

  gst_element_class_set_static_metadata (gstelement_class,
      "N3DS Image Sink",
      "Sink", "Image sink for the N3DS", "fR30n <0xfR30n@protonmail.com>");

  gst_element_class_add_static_pad_template (gstelement_class, &sinktemplate);
}

static void
n3ds_image_sink_init (N3DSImageSink * sink)
{
  gst_base_sink_set_sync (GST_BASE_SINK (sink), FALSE);
  gst_base_sink_set_async_enabled (GST_BASE_SINK (sink), FALSE);
  gst_base_sink_set_last_sample_enabled (GST_BASE_SINK (sink), FALSE);
}
