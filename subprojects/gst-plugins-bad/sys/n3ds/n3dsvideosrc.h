#ifndef _3DS_VIDEO_SOURCE_H_
#define _3DS_VIDEO_SOURCE_H_

#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>

G_BEGIN_DECLS

#define N3DS_TYPE_VIDEO_SRC \
  (n3ds_video_src_get_type())
#define N3DS_VIDEO_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),N3DS_TYPE_VIDEO_SRC,N3DSVideoSrc))
#define N3DS_VIDEO_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),N3DS_TYPE_VIDEO_SRC,N3DSVideoSrcClass))
#define N3DS_IS_VIDEO_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),N3DS_TYPE_VIDEO_SRC))
#define N3DS_IS_VIDEO_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),N3DS_TYPE_VIDEO_SRC))
#define N3DS_VIDEO_SRC_CAST(obj) ((N3DSVideoSrc *)obj)

typedef struct _N3DSVideoSrc N3DSVideoSrc;
typedef struct _N3DSVideoSrcClass N3DSVideoSrcClass;

GType n3ds_video_src_get_type (void);

G_END_DECLS

#endif /* _3DS_VIDEO_SOURCE_H_ */