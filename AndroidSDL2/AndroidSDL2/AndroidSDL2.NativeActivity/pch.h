//
// pch.h
// Encabezado para archivos de inclusión estándar del sistema.
//
// Lo usa el sistema de compilación para generar el encabezado precompilado. Tenga en cuenta que
// pch.h no se incluyen automáticamente en todos los archivos cpp
// que forman parte del proyecto.
//

#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>

#include <android/log.h>
#include "android_native_app_glue.h"
