#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
list(APPEND TOUCH_INCLUDES
            ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Devices
)
list(APPEND TOUCH_SOURCES
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor.cpp
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core/TouchPanel.cpp

            ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/${TOUCH_INTERFACE_CONTROLLER}
            ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/${TOUCH_INTERFACE_DRIVER}

)
