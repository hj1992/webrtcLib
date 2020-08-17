# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_compile_options(-std=c++17)

include_directories(src/main/jni)
include_directories(src/main/jni/janus)
include_directories(src/main/jni/depends)
include_directories(src/main/jni/depends/openssl/include)
include_directories(src/main/jni/depends/websockets/include)


aux_source_directory(src/main/jni/janus/. janus_src)


add_library( boost_date_time-lib  STATIC  IMPORTED )
set_target_properties( boost_date_time-lib  PROPERTIES IMPORTED_LOCATION       ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libboost_date_time-clang-mt-a32-1_70.a )
add_library( boost_filesystem-lib  STATIC  IMPORTED )
set_target_properties( boost_filesystem-lib  PROPERTIES IMPORTED_LOCATION       ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libboost_filesystem-clang-mt-a32-1_70.a )
add_library( boost_system-lib  STATIC  IMPORTED )
set_target_properties( boost_system-lib  PROPERTIES IMPORTED_LOCATION       ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libboost_system-clang-mt-a32-1_70.a )
add_library( boost_thread-lib  STATIC  IMPORTED )
set_target_properties( boost_thread-lib  PROPERTIES IMPORTED_LOCATION       ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libboost_thread-clang-mt-a32-1_70.a )

add_library( libssl-lib STATIC IMPORTED )
set_target_properties( libssl-lib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libssl.a )
add_library( libcrypto-lib STATIC IMPORTED )
set_target_properties( libcrypto-lib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libcrypto.a )

add_library( libwebsockets-lib STATIC IMPORTED )
set_target_properties( libwebsockets-lib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libwebsockets.a )


add_library( libz-lib STATIC IMPORTED )
set_target_properties( libz-lib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libz.a )

add_library( libwebrtc-lib STATIC IMPORTED )
set_target_properties( libwebrtc-lib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libwebrtc.a )

add_library( jingle_peerconnection_so-lib
        SHARED
        IMPORTED )
set_target_properties( jingle_peerconnection_so-lib
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libjingle_peerconnection_so.so )
find_library(log-lib log)
find_library(media-lib mediandk)
find_library(android-lib android)
find_library(OpenSLES-lib OpenSLES)




add_library( # Sets the name of the library.
        webrtcmodule-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/jni/cJSON.cpp
        src/main/jni/signalingsession.cpp
        src/main/jni/WebrtcClient.cpp
        src/main/jni/WebrtcDef.cpp
        src/main/jni/webrtcmodule.cpp
        src/main/jni/jniHelper.cpp
        ${janus_src}
        )

target_compile_definitions(webrtcmodule-lib PUBLIC "ANDROID")
target_compile_definitions(webrtcmodule-lib PUBLIC "DEBUG_WEBRTC")
target_compile_definitions(webrtcmodule-lib PUBLIC "DEBUG_WEBRTC")

target_link_libraries( # Specifies the target library.
        webrtcmodule-lib
        jingle_peerconnection_so-lib

        boost_date_time-lib
        boost_filesystem-lib
        boost_system-lib
        boost_thread-lib
        libwebsockets-lib
        #libwebrtc-lib

        libssl-lib
        libcrypto-lib
        ${log-lib}
        ${media-lib}
        ${android-lib}
        ${libz-lib}
        EGL
        GLESv2
        GLESv3
        )