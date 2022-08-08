#include "n3dsaudiosrc.h"

#include <malloc.h>
#include <stdlib.h>

#include <gst/audio/audio.h>

#include <3ds.h>

#define SAMPLERATE     16364
#define BYTESPERSAMPLE 2

#define WAIT_TIMEOUT 1000000000ULL

#define N3DS_AUDIO_SRC_CAPS "audio/x-raw, " \
        "format = (string) "GST_AUDIO_NE(S16)", " \
        "layout = (string) { interleaved }, " \
        "rate = (int) 16364, " \
        "channels = (int) 1;"

static GstStaticPadTemplate srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (N3DS_AUDIO_SRC_CAPS));

GST_DEBUG_CATEGORY_STATIC (n3ds_audio_src_debug);
#define GST_CAT_DEFAULT n3ds_audio_src_debug

enum
{
  PROP0,
};

struct _N3DSAudioSrc
{
  GstBaseSrc element;

  u8 *buf;
  u32 totalbufsize;
};

struct _N3DSAudioSrcClass
{
  GstBaseSrcClass parent_class;
};

#define n3ds_audio_src_parent_class parent_class
G_DEFINE_TYPE (N3DSAudioSrc, n3ds_audio_src, GST_TYPE_BASE_SRC);

GST_ELEMENT_REGISTER_DEFINE_WITH_CODE (n3dsaudiosrc, "n3dsaudiosrc",
    GST_RANK_NONE, N3DS_TYPE_AUDIO_SRC,
    GST_DEBUG_CATEGORY_INIT (n3ds_audio_src_debug, "n3dsaudiosrc", 0,
        "N3DS Audio Source"));


static gboolean
n3ds_audio_src_start (GstBaseSrc * basesrc)
{
  N3DSAudioSrc *src = N3DS_AUDIO_SRC_CAST (basesrc);
  Result res;

  res = micInit (src->buf, src->totalbufsize);
  if (R_FAILED (res)) {
    GST_ERROR ("micInit failed! res:%ld", res);
    return FALSE;
  }

  res =
      MICU_StartSampling (MICU_ENCODING_PCM16_SIGNED, MICU_SAMPLE_RATE_16360, 0,
      src->totalbufsize, TRUE);
  if (R_FAILED (res)) {
    GST_ERROR ("MICU_StartSampling failed! res:%ld", res);
    return FALSE;
  }

  return TRUE;
}

static gboolean
n3ds_audio_src_stop (GstBaseSrc * basesrc)
{
  MICU_StopSampling ();
  micExit ();

  return TRUE;
}

static GstFlowReturn
n3ds_audio_src_create (GstBaseSrc * basesrc, guint64 offset, guint length,
    GstBuffer ** ret)
{
  N3DSAudioSrc *src = N3DS_AUDIO_SRC_CAST (basesrc);
  GstBuffer *buf;
  GstMapInfo map;

  s32 index;
  Handle handle;
  gboolean got_frame = FALSE;

  buf = gst_buffer_new_allocate (NULL, src->totalbufsize, NULL);
  if (!gst_buffer_map (buf, &map, GST_MAP_WRITE)) {
    gst_buffer_unmap (buf, &map);
    gst_buffer_unref (buf);
    *ret = NULL;
    return GST_FLOW_ERROR;
  }

  MICU_GetEventHandle (&handle);

  while (!got_frame) {
    svcWaitSynchronizationN (&index, &handle, 1, FALSE, WAIT_TIMEOUT);

    switch (index) {
      case 0:
        got_frame = TRUE;

        break;
      default:
        return GST_FLOW_ERROR;
        break;
    }
  }

  memcpy (map.data, src->buf, src->totalbufsize);

  gst_buffer_unmap (buf, &map);

  *ret = buf;
  return GST_FLOW_OK;
}

static void
n3ds_audio_src_finalize (GObject * obj)
{
  N3DSAudioSrc *src = N3DS_AUDIO_SRC_CAST (obj);

  free (src->buf);
  src->buf = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
n3ds_audio_src_class_init (N3DSAudioSrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSrcClass *gstbase_src_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstbase_src_class = GST_BASE_SRC_CLASS (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (n3ds_audio_src_finalize);

  gstbase_src_class->start = GST_DEBUG_FUNCPTR (n3ds_audio_src_start);
  gstbase_src_class->stop = GST_DEBUG_FUNCPTR (n3ds_audio_src_stop);
  gstbase_src_class->create = GST_DEBUG_FUNCPTR (n3ds_audio_src_create);

  gst_element_class_set_static_metadata (gstelement_class,
      "N3DS Audio Source",
      "Source", "Audio source for the N3DS", "fR30n <0xfR30n@protonmail.com>");

  gst_element_class_add_static_pad_template (gstelement_class, &srctemplate);
}

static void
n3ds_audio_src_init (N3DSAudioSrc * src)
{
  src->totalbufsize = 0x10000;
  src->buf = memalign (0x1000, src->totalbufsize);

  gst_base_src_set_live (GST_BASE_SRC (src), TRUE);
}
