# SSBuffer AIV/AIC通信样例

## 概述

本样例演示基础API场景下如何在同一个`__mix__(1, 2)`任务中使用SSBuffer实现AIV到AIC的消息通信。任务包含1个AIC和2个AIV，两个AIV分别通过独立的环形消息队列发送消息，AIC接收并检查消息，最后将结果写入GM，由Host侧逐条校验。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

本样例固定使用`dav-3510`架构，仅支持NPU和仿真运行模式。

## 目录结构介绍

```
├── ssbuf_aiv_aic_comm
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── ssbuf_aiv_aic_comm.asc         // Ascend C算子实现、Kernel调用和结果校验
│   ├── README.md                      // 中文样例说明文档
│   └── README_en.md                   // 英文样例说明文档
```

## 样例描述

### 样例功能

- AIC初始化SSBuffer中的两条消息队列。
- AIC通过模式4核间同步通知两个AIV开始发送。
- 两个AIV分别发送12条消息，AIC按发送方和消息序号接收24条消息。
- AIC检查消息头、发送方编号、消息序号和校验值，并将结果写入GM。
- Host侧回读GM中的结果，确认消息没有丢失、覆盖或进入错误的消息队列。

### 样例规格

<table>
<caption>表1：样例输入输出规格</caption>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">SSBuffer AIV/AIC Communication</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">无</td><td align="center">-</td><td align="center">-</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[24]</td><td align="center">CommResult</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">ssbuf_aiv_aic_comm</td></tr>
<tr><td rowspan="1" align="center">核数量</td><td colspan="4" align="center">1个AIC和2个AIV</td></tr>
</table>

### SSBuffer空间布局

- SSBuffer总容量为3KB，本样例使用前2KB。
- AIV0使用`[0, 1024)`，包含8条128B消息。
- AIV1使用`[1024, 2048)`，包含8条128B消息。
- `[2048, 3072)`在本样例中不使用。
- 两条消息队列互不重叠，每个AIV只写自己的消息队列。

每条消息由`CommMessage`表示，包含消息头、消息序号、消息数据和校验值。`alignas(128)`保证消息按128B对齐，跨核共享的消息字段使用`volatile`。

## 样例实现

### 1. 获取SSBuffer工作区

基础API通过`AscendC::GetSsbufBaseAddr()`获取SSBuffer基地址。返回值属于`__ssbuf__`地址空间，转换后的指针和后续消息指针都必须保留该地址空间限定，不能当作普通GM地址使用。

```cpp
__aicore__ inline __ssbuf__ SsbufWorkspace* GetWorkspace()
{
    return reinterpret_cast<__ssbuf__ SsbufWorkspace*>(AscendC::GetSsbufBaseAddr());
}
```

### 2. AIC初始化消息队列

SSBuffer初始内容不保证为0，`AscendC::InitSocState()`也不会清空SSBuffer。AIC启动后遍历两个AIV的消息队列，清除全部消息头，再通过模式4核间同步分别通知AIV0和AIV1开始工作。

```cpp
InitQueues(workspace);
AscendC::CrossCoreSetFlag<4, PIPE_S>(GetAicFlagId(0, START_FLAG_ID));
AscendC::CrossCoreSetFlag<4, PIPE_S>(GetAicFlagId(1, START_FLAG_ID));
```

模式4用于同一AI Core内AIC与单个AIV之间的同步。AIV0和AIV1都使用本地标记ID `0~15`，AIC侧使用`0~15`对应AIV0，使用`16~31`对应AIV1。`GetAicFlagId`按照`sender * 16 + aivFlagId`计算AIC侧标记ID。

### 3. AIV发送消息

AIV通过`AscendC::GetSubBlockIdx()`获取自己的Vector核编号，并选择对应的消息队列。发送第`sequence`条消息时，使用下面的公式计算队列位置：

```cpp
queueIndex = sequence & (QUEUE_DEPTH - 1);
```

当前队列深度为8，是2的幂，因此位与计算等价于对8取模。AIV依次写入`sequence`、`value`、`checksum`和`head`，再调用：

```cpp
AscendC::CrossCoreSetFlag<4, PIPE_S>(MESSAGE_READY_FLAG_ID);
```

该通知表示消息已经准备完成，AIC收到通知后才读取消息。当序号超过队列深度时，新的消息会回到队列开头的存储位置。例如序号0和序号8都会使用位置0。AIV在复用位置前等待ACK，确保旧消息已经由AIC处理完成。

### 4. AIC接收消息

AIC按发送方编号和消息序号读取两条消息队列。每次读取前通过模式4核间同步等待对应AIV的消息准备完成通知，然后检查以下内容：

- 功能号是否为`0x5A01`。
- `valid`位是否已经设置。
- 发送方编号是否与当前消息队列一致。
- 消息序号是否为当前期待值。
- 校验值是否与发送方、序号和消息数据匹配。

检查结果和消息内容写入GM中的`CommResult`。AIC完成读取后清除消息头，并通过发送方对应的ACK标记ID通知AIV释放该消息空间：

```cpp
ClearMessage(message);
AscendC::CrossCoreSetFlag<4, PIPE_S>(GetAicFlagId(expectedSender, MESSAGE_ACK_FLAG_ID));
```

### 5. Kernel入口和Host调用

Kernel使用`ASCEND_IS_AIC`和`ASCEND_IS_AIV`区分AIC、AIV执行分支，并在同一个`__mix__(1, 2)`任务中访问同一片SSBuffer。Host侧为24条`CommResult`申请GM空间，通过Kernel调用符启动一个Mix block，等待流同步后回读结果并校验。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`。

- 编译和执行

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=npu ..
  make -j
  ./demo
  ```

  使用NPU仿真模式时，将`CMAKE_ASC_RUN_MODE`设置为`sim`：

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=sim ..
  make -j
  ./demo
  ```

  > **注意：** 切换运行模式前需清理build目录中的CMake缓存，或重新创建build目录。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，对应Ascend 950PR/Ascend 950DT |

## 执行结果

执行成功后，Host侧完成24条结果校验，终端输出如下：

```text
test pass!
```

## 使用约束

- AIC和两个AIV必须在同一个`__mix__(1, 2)`任务中访问SSBuffer。
- 自定义消息布局不能超过3KB，也不能与其他SSBuffer协议重叠。
- 消息字段的写入顺序、`valid`位和消息头清理顺序不能随意调整。
- 队列深度需要保持为2的幂，才能使用位与方式计算队列位置。
- 跨核共享字段需要使用`volatile`，消息发布和空间复用需要使用配对的模式4核间同步通知。
- AIV侧标记ID范围为`0~15`；AIC侧通过`0~15`和`16~31`分别与AIV0和AIV1配对。
