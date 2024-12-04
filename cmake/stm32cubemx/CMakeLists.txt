cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE 
	USE_HAL_DRIVER 
	STM32F303xC
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../Core/Inc
    ../../USB_DEVICE/App
    ../../USB_DEVICE/Target
    ../../Drivers/STM32F3xx_HAL_Driver/Inc
    ../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ../../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc
    ../../Drivers/CMSIS/Device/ST/STM32F3xx/Include
    ../../Drivers/CMSIS/Include
)

target_sources(stm32cubemx INTERFACE
    ../../Core/Src/main.c
    ../../Core/Src/gpio.c
    ../../Core/Src/adc.c
    ../../Core/Src/dma.c
    ../../Core/Src/spi.c
    ../../Core/Src/tim.c
    ../../Core/Src/usart.c
    ../../Core/Src/stm32f3xx_it.c
    ../../Core/Src/stm32f3xx_hal_msp.c
    ../../USB_DEVICE/App/usb_device.c
    ../../USB_DEVICE/App/usbd_desc.c
    ../../USB_DEVICE/App/usbd_custom_hid_if.c
    ../../USB_DEVICE/Target/usbd_conf.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_usb.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dma.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_cortex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_exti.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_spi.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_spi_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_uart.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_uart_ex.c
    ../../Core/Src/system_stm32f3xx.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c
    ../../Core/Src/sysmem.c
    ../../Core/Src/syscalls.c
    ../../startup_stm32f303xc.s
)

target_link_directories(stm32cubemx INTERFACE
)

target_link_libraries(stm32cubemx INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


