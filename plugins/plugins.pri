
include(../beebeep.pri)
CONFIG += plugin
unix: CONFIG += unversioned_libname
win32: CONFIG += skip_target_version_ext
