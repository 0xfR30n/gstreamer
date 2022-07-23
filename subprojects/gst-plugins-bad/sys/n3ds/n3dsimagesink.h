#ifndef _3DS_IMAGE_SINK_
#define _3DS_IMAGE_SINK_

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS
#define N3DS_TYPE_IMAGE_SINK \
  (n3ds_image_sink_get_type())
#define N3DS_IMAGE_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),N3DS_TYPE_IMAGE_SINK,N3DSImageSink))
#define N3DS_IMAGE_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),N3DS_TYPE_IMAGE_SINK,N3DSImageSinkClass))
#define IS_N3DS_IMAGE_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),N3DS_TYPE_IMAGE_SINK))
#define IS_N3DS_IMAGE_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),N3DS_TYPE_IMAGE_SINK))
#define N3DS_IMAGE_SINK_CAST(obj) ((N3DSImageSink *)obj)

typedef struct _N3DSImageSink N3DSImageSink;
typedef struct _N3DSImageSinkClass N3DSImageSinkClass;

GType n3ds_image_sink_get_type (void);

G_END_DECLS
#endif /* _3DS_IMAGE_SINK_ */
