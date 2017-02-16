APP_ABI := armeabi-v7a
APP_STL :=gnustl_static
APP_CFLAGS += -fopenmp
APP_LDFLAGS += -fopenmp
APP_PLATFORM := android-14
APP_CFLAGS += -Wno-error=format-security

#APP_STL := stlport_shared