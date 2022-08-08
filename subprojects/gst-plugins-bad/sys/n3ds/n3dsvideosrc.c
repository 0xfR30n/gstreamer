#include "n3dsvideosrc.h"

#include <3ds.h>

#define WIDTH  400
#define HEIGHT 240

#define WAIT_TIMEOUT 1000000000ULL

#define N3DS_VIDEO_SRC_CAPS "video/x-raw, " \
        "format = (string) { RGB16 }, " \
        "framerate = (fraction) [ 0, MAX ], " \
        "width = (int) 400, " "height = (int) 240 "

static GstStaticPadTemplate srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (N3DS_VIDEO_SRC_CAPS));

GST_DEBUG_CATEGORY_STATIC (n3ds_video_src_debug);
#define GST_CAT_DEFAULT n3ds_video_src_debug

enum
{
  PROP0,
};

struct _N3DSVideoSrc
{
  GstBaseSrc element;

  u32 bufsize;
  u32 transersize;
};

struct _N3DSVideoSrcClass
{
  GstBaseSrcClass parent_class;
};

#define n3ds_video_src_parent_class parent_class
G_DEFINE_TYPE (N3DSVideoSrc, n3ds_video_src, GST_TYPE_BASE_SRC);

GST_ELEMENT_REGISTER_DEFINE_WITH_CODE (n3dsvideosrc, "n3dsvideosrc",
    GST_RANK_NONE, N3DS_TYPE_VIDEO_SRC,
    GST_DEBUG_CATEGORY_INIT (n3ds_video_src_debug, "n3dsvideosrc", 0,
        "N3DS Video Source"));


static gboolean
n3ds_video_src_start (GstBaseSrc * basesrc)
{
  N3DSVideoSrc *src = N3DS_VIDEO_SRC_CAST (basesrc);

  CAMU_SetSize (SELECT_IN1, SIZE_CTR_TOP_LCD, CONTEXT_A);
  CAMU_SetOutputFormat (SELECT_IN1, OUTPUT_RGB_565, CONTEXT_A);
  CAMU_SetFrameRate (SELECT_IN1, FRAME_RATE_30);

  CAMU_SetNoiseFilter (SELECT_IN1, TRUE);
  CAMU_SetAutoExposure (SELECT_IN1, TRUE);
  CAMU_SetAutoWhiteBalance (SELECT_IN1, TRUE);

  CAMU_SetTrimming (PORT_CAM1, FALSE);

  CAMU_GetMaxBytes (&src->transersize, WIDTH, HEIGHT);
  CAMU_SetTransferBytes (PORT_CAM1, src->transersize, WIDTH, HEIGHT);

  CAMU_Activate (SELECT_IN1);

  return TRUE;
}

static gboolean
n3ds_video_src_stop (GstBaseSrc * basesrc)
{
  CAMU_StopCapture (PORT_CAM1);

  return TRUE;
}

static GstFlowReturn
n3ds_video_src_create (GstBaseSrc * basesrc, guint64 offset, guint length,
    GstBuffer ** ret)
{
  N3DSVideoSrc *src = N3DS_VIDEO_SRC_CAST (basesrc);
  GstBuffer *buf;
  GstMapInfo map;

  s32 index = 0;
  Handle recv_events[2];
  gboolean got_frame = FALSE;

  buf = gst_buffer_new_allocate (NULL, src->bufsize, NULL);
  if (!gst_buffer_map (buf, &map, GST_MAP_WRITE)) {
    gst_buffer_unmap (buf, &map);
    gst_buffer_unref (buf);
    *ret = NULL;
    return GST_FLOW_ERROR;
  }

  CAMU_GetBufferErrorInterruptEvent (&recv_events[0], PORT_CAM1);
  CAMU_SetReceiving (&recv_events[1], map.data, PORT_CAM1, src->bufsize,
      (s16) src->transersize);
  CAMU_StartCapture (PORT_CAM1);

  while (!got_frame) {
    svcWaitSynchronizationN (&index, recv_events, 2, FALSE, WAIT_TIMEOUT);

    switch (index) {
      case 0:
        svcCloseHandle (recv_events[1]);
        CAMU_SetReceiving (&recv_events[1], map.data, PORT_CAM1, src->bufsize,
            (s16) src->transersize);
        CAMU_StartCapture (PORT_CAM1);
        break;
      case 1:
        svcCloseHandle (recv_events[0]);
        svcCloseHandle (recv_events[1]);
        got_frame = TRUE;
        break;
      default:
        break;
    }
  }

  gst_buffer_unmap (buf, &map);

  *ret = buf;
  return GST_FLOW_OK;
}

static void
n3ds_video_src_finalize (GObject * obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
n3ds_video_src_class_init (N3DSVideoSrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSrcClass *gstbase_src_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstbase_src_class = GST_BASE_SRC_CLASS (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (n3ds_video_src_finalize);

  gstbase_src_class->start = GST_DEBUG_FUNCPTR (n3ds_video_src_start);
  gstbase_src_class->stop = GST_DEBUG_FUNCPTR (n3ds_video_src_stop);
  gstbase_src_class->create = GST_DEBUG_FUNCPTR (n3ds_video_src_create);

  gst_element_class_set_static_metadata (gstelement_class,
      "N3DS Video Source",
      "Source", "Video source for the N3DS", "fR30n <0xfR30n@protonmail.com>");

  gst_element_class_add_static_pad_template (gstelement_class, &srctemplate);
}

static void
n3ds_video_src_init (N3DSVideoSrc * src)
{
  src->bufsize = WIDTH * HEIGHT * 2;

  gst_base_src_set_live (GST_BASE_SRC (src), TRUE);
}
