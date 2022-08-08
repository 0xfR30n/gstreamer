if host_system != 'horizon'
  subdir_done()
endif

n3ds_sources = [
  'n3dsvideosrc.c',
  'n3dsimagesink.c',
  'n3dsaudiosrc.c',
  'n3ds.c'
]

gstn3ds = library('gstn3ds',
  n3ds_sources,
  c_args : [gst_plugins_bad_args],
  include_directories : [configinc],
  dependencies : [gst_dep, gstbase_dep, gstaudio_dep],
  install : true,
  install_dir : plugins_install_dir)

pkgconfig.generate(gstn3ds, install_dir : plugins_pkgconfig_install_dir)
plugins += [gstn3ds]

