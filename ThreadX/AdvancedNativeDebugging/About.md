## System View Description (SVD) files are part of the Cortex Microcontroller Software Interface Standard (CMSIS)

 Theses files provide a detailed description of the memory-mapped registers of peripherals in microcontrollers, especially those based on Arm Cortex-M processors2.

 They work natively in Visual Studio 2022, adding additional debugging windows including support for ThreadX (previously azure)


Detailed Peripheral Information:

They offer comprehensive details about the peripherals, including functional descriptions and individual bit fields in memory-mapped registers.

SVD files help debuggers provide detailed views of device peripherals, displaying the current register state.

## Use
This is an example of use in the launch.vs.json file for Visual Studio 2022

``` "svdPath": "C:/nf-interpreter/targets-community/ThreadX/AdvancedNativeDebugging/rp2350.svd",```


