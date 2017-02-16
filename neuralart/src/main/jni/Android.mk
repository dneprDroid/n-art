

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := neuralart

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_SRC_FILES := torchandroid.cpp torchdemo.cpp android_fopen.c ApkFile.c  THApkFile.c

LOCAL_LDLIBS := -llog -landroid -L $(LOCAL_PATH)/prebuilts  -lluaT -lluajit -lTH  -lTHNN   -ltorch -lnnx -limage -lluaT -lluajit -lTH -lTHNN  -ltorch -lnnx -limage

include $(BUILD_SHARED_LIBRARY)