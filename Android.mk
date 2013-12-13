LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SDL_PATH := ../SDL
LOCAL_MODULE := gear2d_static
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -DLOGTRACE
LOCAL_CFLAGS := -DYAML_VERSION_MAJOR=0 -DYAML_VERSION_MINOR=1 -DYAML_VERSION_PATCH=4 -DYAML_VERSION_STRING=\"0.1.4-with-SDLRwops\"
LOCAL_C_INCLUDES := $(LOCAL_PATH)/thirdparty/yaml-0.1.4/include \
  $(LOCAL_PATH)/thirdparty/SDL2_framerate \
  $(LOCAL_PATH)/$(SDL_PATH)/include
  


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src

LOCAL_SRC_FILES := \
  $(subst $(LOCAL_PATH)/,, \
  $(wildcard $(LOCAL_PATH)/src/*.cc) \
  $(wildcard $(LOCAL_PATH)/thirdparty/SDL2_framerate/*.c) \
  $(wildcard $(LOCAL_PATH)/thirdparty/yaml-0.1.4/src/*.c))

LOCAL_MODULE_FILENAME := libgear2d

LOCAL_STATIC_LIBRARIES := SDL2_static

include $(BUILD_STATIC_LIBRARY)
