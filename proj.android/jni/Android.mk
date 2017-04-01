LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp
MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/**/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/**/**/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/**/**/**/*.cpp)
LOCAL_SRC_FILES += $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,./prebuilt-mk)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
