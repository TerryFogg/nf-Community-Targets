# Developing native ports with Visual Studio 2022

### Requires the following installation options

- Linux development with C++
- Visual C++ tools for Cmake and Linux

------

## OpenOCD

The official versions of OPENOCD doesn't work for RP2040 and STM32U5A9. 

So we need additional copies of openocd from STMicroelectronics and RaspberryPi.

#### Release binaries for the official windows can be found at https://github.com/openocd-org/openocd/releases

------

### Debugging with Visual Studio 17.7.6 requires the following files in the `C:\nf-interpreter\.vs` directory.

## **Tasks.vs.json**

```json
 {
  "version": "0.2.1",
  "tasks": [
    {
      "taskLabel": "Flash nanoBooter on STM32F769_Discovery",
      "appliesTo": "*.*",
      "type": "default",
      "command": "${env.COMSPEC}",
      "args": [
        "openocd -f interface/stlink.cfg  -f target/stm32f7x.cfg -c \"program build/nanoBooter.elf verify\" -c \"reset halt\" -c shutdown"
      ],
      "windows": {
        "options": {
          "shell": {
            "executable": "cmd.exe",
            "args": [ "/c" ]
          }
        }
      }
    },
    {
      "taskLabel": "Flash nanoCLR on STM32F769_Discovery",
      "default": null,
      "appliesTo": "*.*",
      "command": "${env.COMSPEC}",
      "args": [
        "openocd -f interface/stlink.cfg  -f target/stm32f7x.cfg -c \"program build/nanoCLR.elf verify\" -c \"reset halt\" -c shutdown"
      ],
      "windows": {
        "options": {
          "shell": {
            "executable": "cmd.exe",
            "args": [ "/c" ]
          }
        }
      }
    },
    {
      "taskLabel": "Flash nanoCLR on STM32H7",
      "default": null,
      "appliesTo": "*.*",
      "command": "${env.COMSPEC}",
      "args": [
        "openocd -f interface/stlink.cfg  -f target/stm32h7x.cfg -c \"program build/nanoCLR.elf verify\" -c \"reset halt\" -c shutdown"
      ],
      "postRemoteConnectCommands": [
        { "text": "reset_config srst_only srst_nogate connect_assert_srst" }
      ],
      "windows": {
        "options": {
          "shell": {
            "executable": "cmd.exe",
            "args": [ "/c" ]
          }
        }
      }
    },
    {
      "taskLabel": "Flash nanoCLR on STM32U5",
      "default": null,
      "appliesTo": "*.*",
      "command": "${env.COMSPEC}",
      "args": [
        "C:/nftools/openocd_stm32/bin/openocd -f interface/stlink-dap.cfg  -f target/stm32u5x.cfg -c \"program build/nanoCLR.elf verify\" -c \"reset halt\" -c shutdown"
      ],
      "postRemoteConnectCommands": [
        { "text": "reset_config srst_only srst_nogate connect_assert_srst" }
      ],
      "windows": {
        "options": {
          "shell": {
            "executable": "cmd.exe",
            "args": [ "/c" ]
          }
        }
      }
    },
    {
      "taskLabel": "Flash nanoCLR on RP2040",
      "default": null,
      "appliesTo": "*.*",
      "command": "${env.COMSPEC}",
      "args": [
        "c:/nftools/openocd_stm32/bin/openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c \"adapter speed 5000\" -c \"program build/nanoCLR.elf\" -c \"verify reset exit\""
      ],
      "windows": {
        "options": {
          "shell": {
            "executable": "cmd.exe",
            "args": [ "/c" ]
          }
        }
      }
    },
    {
      "taskLabel": "Erase ESP32 flash",
      "appliesTo": "*.*",
      "type": "default",
      "command": "${env.COMSPEC}",
      "args": [
        "python c:/nfTools/Espressif/frameworks/esp-idf-v4.4.5/components/esptool_py/esptool/esptool.py --chip auto --port com11 --baud 1500000 --before default_reset --after hard_reset erase_flash"
      ]
    },
    {
      "taskLabel": "Flash ESP32",
      "appliesTo": "*.*",
      "type": "default",
      "command": "${env.COMSPEC}",
      "args": [
        "python c:/nfTools/Espressif/frameworks/esp-idf-v4.4.6/components/esptool_py/esptool/esptool.py --chip auto --port com11 --baud 1500000 --before default_reset --after hard_reset write_flash -z --flash_size detect 0x1000 ${workspaceRoot}/build/bootloader/bootloader.bin 0x10000 ${workspaceRoot}/build/nanoCLR.bin 0x8000 ${workspaceRoot}/build/partitions_4mb.bin"
      ]
    }
  ]
}
```

## **launch.vs.json**

```json
{
  "version": "0.2.1",
  "configurations": [
    {
      "type": "cppdbg",
      "name": "ST_STM32F769I_DISCOVERY",
      "program": "build/nanoCLR.elf",
      "projectTarget": "nanoCLR.elf",
      "project": "CMakeLists.txt",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "processName": "stm32F769I_Discovery",
      "MIMode": "gdb",
      "miDebuggerPath": "c:/nfTools/GNU_Tools_ARM_Embedded/12.3.rel1/bin/arm-none-eabi-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "C:/nftools/OpenOCD-20230621/bin/openocd.exe",
      "debugServerArgs": "-s \"c:/nfTools/openocd/scripts/\" -f interface/stlink.cfg -f board/stm32f7discovery.cfg",
      "serverStarted": "Listening on port 3333 for gdb connections",
      "stopOnEntry": false,
      "postRemoteConnectCommands": [
        { "text": "monitor reset init" },
        { "text": "load" }
      ],
      "launchCompleteCommand": "exec-run",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    },
    {
      "type": "cppdbg",
      "name": "STM32H735G_DISCOVERY",
      "program": "build/nanoCLR.elf",
      "projectTarget": "nanoCLR.elf",
      "project": "CMakeLists.txt",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "processName": "STM32H735G_DISCOVERY",
      "MIMode": "gdb",
      "miDebuggerPath": "c:/nfTools/GNU_Tools_ARM_Embedded/12.3.rel1/bin/arm-none-eabi-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "C:/nftools/openocd/bin/openocd.exe",
      "debugServerArgs": "-c \"debug_level 3\" -s \"c:/nfTools/openocd/scripts/\" -f interface/stlink.cfg -f board/stm32h735g-disco.cfg",
      "serverStarted": "Listening on port 3333 for gdb connections",
      "stopOnEntry": true,
      "postRemoteConnectCommands": [
        { "text": "monitor halt" },
        { "text": "monitor reset init" },
        { "text": "load" }
      ],
      "launchCompleteCommand": "exec-run",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    },
    {
      "type": "cppdbg",
      "name": "STM32H7B3I_DK",
      "program": "build/nanoCLR.elf",
      "projectTarget": "nanoCLR.elf",
      "project": "CMakeLists.txt",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "processName": "STM32H7B3I_DK",
      "MIMode": "gdb",
      "miDebuggerPath": "c:/nfTools/GNU_Tools_ARM_Embedded/12.3.rel1/bin/arm-none-eabi-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "C:/nftools/openocd/bin/openocd.exe",
      "debugServerArgs": "-s \"c:/nfTools/openocd/scripts/\" -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg  -c init -c \"reset init\"",
      "serverStarted": "Listening on port 3333 for gdb connections",
      "stopOnEntry": true,
      "postRemoteConnectCommands": [
        {
          "text": "monitor halt",
          "ignoreFailures": true
        },
        {
          "text": "monitor reset init",
          "ignoreFailures": true
        },
        { "text": "load" }
      ],
      "launchCompleteCommand": "exec-run",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    },
    {
      "type": "cppdbg",
      "name": "STM32U5A9J-DK",
      "program": "build/nanoCLR.elf",
      "projectTarget": "nanoCLR.elf",
      "project": "CMakeLists.txt",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "processName": "STM32U5A9J_DK",
      "MIMode": "gdb",
      "miDebuggerPath": "c:/nfTools/GNU_Tools_ARM_Embedded/12.3.rel1/bin/arm-none-eabi-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "C:/nftools/openocd/bin/openocd.exe",
      "debugServerArgs": "-s \"c:/nfTools/openocd/scripts/\" -f interface/stlink.cfg -f target/stm32u5x.cfg  -c init -c \"reset init\"",
      "serverStarted": "Listening on port 3333 for gdb connections",
      "stopOnEntry": true,
      "postRemoteConnectCommands": [
        {
          "text": "monitor halt",
          "ignoreFailures": true
        },
        {
          "text": "monitor reset init",
          "ignoreFailures": true
        },
        { "text": "load" }
      ],
      "launchCompleteCommand": "exec-run",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    },
    {
      "type": "cppdbg",
      "name": "NUCLEO-H743ZI2",
      "program": "build/nanoCLR.elf",
      "projectTarget": "nanoCLR.elf",
      "project": "CMakeLists.txt",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "processName": "NUCLEO-H743ZI2",
      "MIMode": "gdb",
      "miDebuggerPath": "c:/nfTools/GNU_Tools_ARM_Embedded/12.3.rel1/bin/arm-none-eabi-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "C:/nftools/openocd/bin/openocd.exe",
      "debugServerArgs": "-s \"c:/nfTools/openocd/scripts/\" -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg  -c init -c \"reset init\"",
      "serverStarted": "Listening on port 3333 for gdb connections",
      "stopOnEntry": true,
      "postRemoteConnectCommands": [
        {
          "text": "monitor halt",
          "ignoreFailures": true
        },
        {
          "text": "monitor reset init",
          "ignoreFailures": true
        },
        { "text": "load" }
      ],
      "launchCompleteCommand": "exec-run",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    },
    {
      "name": "ESP_WROVER_KIT",
      "type": "cppdbg",
      "request": "launch",
      "program": "build/nanoCLR.elf",
      "project": "CMakeLists.txt",
      "projectTarget": "nanoCLR.elf",
      "cwd": "${workspaceRoot}",
      "MIMode": "gdb",
      "stopAtEntry": false,
      "externalConsole": false,
      "miDebuggerPath": "c:/nftools/espressif/tools/xtensa-esp32-elf/esp-2021r2-patch5-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "c:/nftools/Espressif/tools/openocd-esp32/v0.12.0-esp32-20230419/openocd-esp32/bin/openocd.exe",
      "debugServerArgs": "-s \"c:/nftools/Espressif/tools/openocd-esp32/v0.12.0-esp32-20230419/openocd-esp32/share/openocd/scripts\" -f interface/ftdi/esp32_devkitj_v1.cfg -f target/esp32.cfg ",
      "serverStarted": "Info : .*Tensilica.*0x1.",
      "postRemoteConnectCommands": [
        { "text": "monitor reset halt" },
        { "text": "flushregs" },
        { "text": "thb app_main" },
        { "text": "x $a1=0" }
      ],
      "launchCompleteCommand": "exec-continue",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    },
    {
      "type": "cppdbg",
      "name": "RP2040",
      "program": "build/nanoCLR.elf",
      "projectTarget": "nanoCLR.elf",
      "project": "CMakeLists.txt",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "processName": "RP2040",
      "MIMode": "gdb",
      "miDebuggerPath": "c:/nfTools/GNU_Tools_ARM_Embedded/12.3.rel1/bin/arm-none-eabi-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "C:/Users/Terry/AppData/Local/VisualGDB/EmbeddedDebugPackages/com.sysprogs.arm.openocd/bin/openocd.exe",
      "debugServerArgs": "-f interface/picoprobe.cfg -c \"adapter speed 3000\" -c \"transport select swd\" -f target/rp2040.cfg -c init -c \"reset init\"",
      "serverStarted": "Listening on port 3333 for gdb connections",
      "stopOnEntry": true,
      "postRemoteConnectCommands": [
        { "text": "monitor halt" },
        { "text": "monitor reset" },
        { "text": "load" }
      ],
      "launchCompleteCommand": "exec-run",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    }
  ]
}
```

### Diagnosing problems with the debug and launch.vs.json

You can get a better understanding of problems that may be occuring with the gdb/openocd debug setup. In Visual Studio select the Command Window Ctrl+Alt+A, and turn on debug logging with the following command.

```
debug.midebuglog /On:c:\Temp\debug.log
```

Run a debugging session to collect data and turn it off with the following command.

```
debug.midebuglog /Off
```

This will give you a log of the operation and communications occuring. ( Good luck!)

