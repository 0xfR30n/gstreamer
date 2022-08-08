#ifndef _3DS_AUDIO_SRC_H_
#define _3DS_AUDIO_SRC_H_
#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>

G_BEGIN_DECLS

#define N3DS_TYPE_AUDIO_SRC \
  (n3ds_audio_src_get_type())
#define N3DS_AUDIO_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),N3DS_TYPE_AUDIO_SRC,N3DSAudioSrc))
#define N3DS_AUDIO_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),N3DS_TYPE_AUDIO_SRC,N3DSAudioSrcClass))
#define N3DS_IS_AUDIO_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),N3DS_TYPE_AUDIO_SRC))
#define N3DS_IS_AUDIO_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),N3DS_TYPE_AUDIO_SRC))
#define N3DS_AUDIO_SRC_CAST(obj) ((N3DSAudioSrc *)obj)

typedef struct _N3DSAudioSrc N3DSAudioSrc;
typedef struct _N3DSAudioSrcClass N3DSAudioSrcClass;

GType n3ds_audio_src_get_type (void);

G_END_DECLS

#endif /* _3DS_AUDIO_SRC_H_ */
