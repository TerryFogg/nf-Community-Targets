#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Use modified official source code or official source code
if( NOT DEFINED GRAPHICS_PATH)
  # Use official source 
  SET(GRAPHICS_PATH  src)
endif()

 list(APPEND GRAPHICS_INCLUDES 
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Gif
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Displays
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Native
 )

 list(APPEND GRAPHICS_SOURCES
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Graphics.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/GraphicsDriver.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/GraphicsMemoryHeap.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Bmp/Bitmap_Decoder.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Fonts/Font.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Gif/Gif.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Gif/GifDecoder.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Gif/lzwread.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jbytearraydatasrc.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcapimin.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcapistd.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jccoefct.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jccolor.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcdctmgr.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jchuff.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jchuff.h
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcinit.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcmainct.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcmarker.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcmaster.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcomapi.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jconfig.h
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcparam.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcphuff.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcprepct.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcsample.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jctrans.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdapimin.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdapistd.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdcoefct.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdcolor.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jddctmgr.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdhuff.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdinput.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmainct.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmarker.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmaster.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmerge.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdphuff.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdpostct.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdsample.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdtrans.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jerror.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jfdctflt.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jfdctfst.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jfdctint.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctflt.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctfst.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctint.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctred.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jmemmgr.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jmemnanoclr.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/Jpeg.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jquant1.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jquant2.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jutils.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/mcbcr.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/mfint.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/miint.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/pfint.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/piint.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/transupp.c
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics.h
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_Bitmap.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_DisplayControl.cpp
             ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_Font.cpp
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Graphics_Memory.cpp
 )
 
 # Display hardware
if(GRAPHICS_SUPPORT)
    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Driver/${DISPLAY_DRIVER}
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Interface/${DISPLAY_INTERFACE}
    )
endif()

 # Touch Display
if(TOUCH_DISPLAY_SUPPORT)
     list(APPEND TOUCH_INCLUDES
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/
                ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/TouchPanel/Core
                ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/TouchPanel/Devices
     )
    list(APPEND TOUCH_SOURCES
                ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor.cpp
                ${CMAKE_SOURCE_DIR}/${GRAPHICS_PATH}/nanoFramework.Graphics/TouchPanel/Core/TouchPanel.cpp

                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/TouchPanel/Devices/${TOUCH_INTERFACE_CONTROLLER}
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/TouchPanel/Interface/${TOUCH_INTERFACE_DRIVER}
    )
endif()

 list(APPEND OPTIONAL_INCLUDES
     ${GRAPHICS_INCLUDES}
     ${TOUCH_INCLUDES}
)

list(APPEND OPTIONAL_SOURCES
     ${GRAPHICS_SOURCES}
     ${TOUCH_SOURCES}
)
