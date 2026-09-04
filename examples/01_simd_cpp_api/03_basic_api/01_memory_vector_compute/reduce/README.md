# Reduce样例

## 概述

本样例介绍了调用`ReduceRepeat<SUM>`和`ReduceDataBlock<SUM>`基础API实现reduce算子，实现了连续内存上数据元素的累加，返回累加结果的功能。

注：本样例使用重命名后的`ReduceRepeat`和`ReduceDataBlock` API，旧命名为`WholeReduceSum`和`BlockReduceSum`。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

``` text
├── reduce
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── reduce.asc              // Ascend C算子实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 算子描述

- 算子功能：  
  ReduceCustom算子实现了连续内存上数据元素的累加，返回累加结果的功能。对应的数学表达式为：

  ```cpp
  z = sum(x)
  ```

- 算子规格：  
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center"> reduce </td></tr>

  <tr><td rowspan="3" align="center">算子输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">4096</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">算子输出</td></tr>
  <tr><td align="center">y</td><td align="center">32</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_custom</td></tr>
  </table>

- 算子实现：  

  当输入数据类型为 `float` 时，输入数据长度的取值范围为 `(0, 4096]`；当输入数据类型为 `half` 时，输入数据长度的取值范围为 `(0, 16384]`。本样例以 `float`类型输入为例，固定输入 `x` 的 shape 为 `[4096]`，输出 `y` 的 shape 为 `[32]`。

    样例将一段连续的输入做累加，得到这段连续buffer内元素的和。

      - 1、在小于256B时，ComputeKey = 1，采用ReduceRepeat<SUM>一次性可以得到结果。

      - 2、长度在float输入(256B,2KB]，或者half输入(256B,4KB]时，ComputeKey = 2。由于同样长度的输入，ReduceDataBlock<SUM>比ReduceRepeat<SUM>的执行速度更快，所以采用一条ReduceDataBlock<SUM>加一条ReduceRepeat<SUM>的做法，得到更高的计算效率。

      - 3、长度在float输入(2KB,16KB]，或者half输入(4KB,32KB]时，ComputeKey = 3。由于一条ReduceRepeat<SUM>的累加效率比使用两条ReduceDataBlock<SUM>的累加效率更高。所以采用两条ReduceRepeat<SUM>（而不是两条ReduceDataBlock<SUM>+一条ReduceRepeat<SUM>），得到这段buffer的累加和。

      - 4、长度在float输入为10000时，ComputeKey = 4，对应ReduceRepeatSumImpl中的处理方法，在Counter模式下，采用ReduceRepeat<SUM>指令，循环处理二维数据中的每一行，得到每一行的归约运行结果。

      - 5、长度在float输入为20000时，ComputeKey = 5，对应BinaryReduceSumImpl中的处理方法，在Counter模式下，先将运算数据一分为二，使用Add指令将两部分数据相加，循环往复，最后一条ReduceRepeat<SUM>指令得到归约的运行结果。此种操作方式，相比较ReduceRepeat<SUM>单指令操作的方式，在数据量较大，循环次数较多的场景下，性能更优。  
      注意代码中使用了Counter模式。

  - Kernel实现  
    计算逻辑是：Ascend C提供的矢量计算接口的操作元素都为LocalTensor，输入数据需要先搬运进片上存储，然后使用Reduce基础API接口完成reduce计算，得到最终结果，再搬出到外部存储上。

    ReduceCustom算子的实现流程分为3个基本任务：CopyIn，Compute，CopyOut。CopyIn任务负责将Global Memory上的输入Tensor xGm存储在xLocal中，Compute任务对xLocal执行reduce计算，计算的方式通过ComputeKey参数决定，ComputeKey由输入的长度决定，计算结果存储在zLocal中，CopyOut任务负责将输出数据从zLocal搬运至Global Memory上的输出Tensor zGm。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # 默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明
- 
  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

  执行结果如下，说明精度对比成功。
  
  ```bash
  test pass!
  ```
