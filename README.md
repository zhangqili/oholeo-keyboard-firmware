# Oholeo键盘固件

硬件工程: <https://oshwhub.com/dwdwdw37/keyboard_origin>

配置器: <https://github.com/zhangqili/EMIKeyboardConfigurator>

旧版配置器(固件版本v1.0): <https://github.com/dwdwdw5163/oholeo_keyboard_software>

## 特性
+ 支持传统触发、快速触发(Rapid Trigger)、差速触发三种模式
+ 支持Mod tap、Mutex(类似Rappy Snappy)、DKS等高级按键
+ 支持绑定键盘，鼠标，媒体控制，系统控制按键
+ 支持板载Flash切换配置
+ 支持最多5层按键绑定
+ 支持切换6键无冲与全键无冲模式
+ 支持模拟游戏摇杆与按键，可通过Steam重映射
+ 支持模拟MIDI键盘
+ 支持低延迟模式，该模式会关闭大部分灯效
+ 配套免驱配置器
+ ~~没有主机也能玩贪食蛇~~

## TODO
+ 免映射模拟游戏手柄
+ 使用蜂鸣器播放MIDI音符

## 构建

### 前置条件
- CMake
- Ninja
- 在ST官网上下载安装STM32Cube开发套件 <https://www.st.com.cn/zh/development-tools/stm32cubeclt.html>
- 将该仓库克隆到本地并在该仓库文件夹打开命令行终端
```PowerShell
git clone --recurse-submodules https://github.com/zhangqili/oholeo-keyboard-firmware.git
cd oholeo-keyboard-firmware
```


### 设置环境变量
以STM32CubeCLT 1.17.0为例，该开发套件默认安装至C:\ST\STM32CubeCLT_1.17.0，接下来你需要将开发套件中的编译器目录添加至环境变量

以PowerShell为例
```PowerShell
$Env:PATH += ";C:\ST\STM32CubeCLT_1.17.0\GNU-tools-for-STM32\bin"
```

### 运行编译命令
```PowerShell
mkdir build
cd build
cmake .. -G"Ninja" --preset=Release
cd Release
ninja
```

如果编译成功，你会看到类似以下的结果
```
[65/65] Linking C executable oholeo-keyboard-firmware.elf
C:/ST/STM32CubeCLT_1.17.0/GNU-tools-for-STM32/bin/../lib/gcc/arm-none-eabi/12.3.1/../../../../arm-none-eabi/bin/ld.exe: warning: oholeo-keyboard-firmware.elf has a LOAD segment with RWX permissions
Memory region         Used Size  Region Size  %age Used
             RAM:       19144 B        32 KB     58.42%
          CCMRAM:          0 GB         8 KB      0.00%
           FLASH:       85936 B       128 KB     65.56%
```

### 使用Keil uVision(不推荐)

使用Keil uVision打开`MDK-ARM/oholeo-keyboard-firmware.uvprojx`，工程默认使用ARM Compiler 6

## 烧录

### SWD
将STLink连接至PCB的STLink接口，使用以下命令烧录

```PowerShell
C:/ST/STM32CubeCLT_1.17.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe -c port=SWD -w ./oholeo-keyboard-firmware.elf -v -s

```

### USB
短接PCB上的BOOT跳线孔上电可进入Bootloader，如果PCB已经烧录了该固件，PCB左上角需要安装磁轴，在PCB上电时按住左上角的Esc键会进入Bootloader。之后使用以下命令烧录
```PowerShell
C:/ST/STM32CubeCLT_1.17.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe -c port=USB1 -w ./oholeo-keyboard-firmware.elf -v -s
```

## 使用说明

第一次烧录启动时，请在上电时按住FN键恢复出厂设置

### 上电至灯效结束后

|按键|功能|
|---|---|
|`Esc`|进入Bootloarder|
|`FN`|清除所有数据，恢复出厂设置并重启|
|`Backspace`|恢复默认配置并重启|

### 默认快捷键

|按键|功能|
|---|---|
|`FN`+`DEL`+`D`|进入Debug状态|
|`FN`+`DEL`+`B`|切换使能蜂鸣器|
|`FN`+`DEL`+`M`|切换使能电磁阀|
|`FN`+`DEL`+`N`|恢复默认状态，取消使能蜂鸣器，取消使能电磁阀，关闭低延迟模式，开启全键无冲|
|`FN`+`DEL`+`R`|重启|
|`FN`+`DEL`+`F`|清除所有数据并恢复出厂设置|
|`FN`+`DEL`+`S`|将当前配置写入至Flash|
|`FN`+`DEL`+`L`|切换低延迟模式|
|`FN`+`DEL`+`T`|切换全键无冲与六键无冲模式|
|`FN`+`DEL`+`Esc`|进入Bootloader|
|`FN`+`DEL`+`Backspace`|恢复默认配置|
|`FN`+`DEL`+`1`|使用配置文件0|
|`FN`+`DEL`+`2`|使用配置文件1|
|`FN`+`DEL`+`3`|使用配置文件2|
|`FN`+`DEL`+`4`|使用配置文件3|
|`FN`+`DEL`+`CapsLock`|在键盘上玩贪食蛇|

### 贪食蛇快捷键

|按键|功能|
|---|---|
|`Backspace`|退出|
|`方向键`|~~应该不用说~~|
|`Right Shift`|减速|
|`DEL`|加速|
|`Space`|暂停|
