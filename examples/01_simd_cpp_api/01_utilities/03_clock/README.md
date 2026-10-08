# 基于SIMD&SIMT混编的时间戳打点功能实现样例

## 概述

本样例基于 Gather 和 Adds 融合计算，演示在 SIMD&SIMT 混合编程场景中使用 `clock()` 接口实现时间戳打点的方法。样例以 SIMT 方式实现离散访存的 Gather，以 SIMD 方式实现连续访存的 Adds，并在 SIMT Gather 阶段前后打点，用于记录执行周期。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.0.0-beta.2 |

## 目录结构

```
├── 03_clock
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── clock.asc                   // Ascend C样例实现 & 调用样例
│   └── README.md
```

## 样例描述

- 样例功能：

  本样例参考[SIMT与SIMD混合编程实现gather和adds计算](../../../05_simd_simt_hybrid/00_introduction/simd_simt_gather_and_adds/README.md)，完成 Gather 和 Adds 的融合计算，计算公式如下：

  ```text
  output[i] = input[index[i]] + 1
  ```

  Host侧直接构造输入数据和golden数据，执行后在进程内完成结果校验，不依赖额外的数据生成或校验脚本。

- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">gather & adds</td></tr>
  <tr><td align="center"></td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[8192]</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gather_and_adds_kernel</td></tr>
  </table>

- 混编流程：

  Vector Core中的SIMT单元和SIMD单元共享片上存储，可以使用片上存储完成SIMT和SIMD的混合编程。本样例中`index`的Shape为`[8192]`，核数为`8`，每个核处理`1024`个数据。线程数`THREAD_COUNT`为`1024`，每个线程处理1个数据元素，单个核调用1次`simt_gather`即可完成Gather计算。

  Gather和Adds融合计算主要分为3个步骤：

  1. `simt_gather`使用SIMT编程方式，根据`index[i]`从Global Memory中读取离散位置的数据，并写入Unified Buffer。

     ```cpp
     uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
     uint32_t gatherIdx = index[idx];
     gatherOutput[threadIdx.x] = input[gatherIdx];
     ```

  2. `simd_adds`使用SIMD编程方式，从Unified Buffer中读取连续数据，通过`Reg::LoadAlign`加载到寄存器，调用`Reg::Adds`完成加1计算，再通过`Reg::StoreAlign`写回Unified Buffer。

     ```cpp
     AscendC::Reg::LoadAlign(srcReg0, input + i * oneRepeatSize);
     AscendC::Reg::Adds(dstReg0, srcReg0, addsAddend, maskReg);
     AscendC::Reg::StoreAlign(output + i * oneRepeatSize, dstReg0, maskReg);
     ```

  3. `DataCopy`将结果从Unified Buffer搬运到Global Memory。

  本样例的重点是展示`clock()`打点，因此计算流程保持为SIMT Gather和SIMD Adds两段；实际开发中，简单加1也可以直接在SIMT阶段完成。

- clock打点说明：

  `simt_gather` 中在 Gather 计算前调用一次 `clock()` 记录开始时间戳，在 Gather 计算后再次调用 `clock()` 记录结束时间戳，并输出两次打点之间的周期差。为避免每个 SIMT 线程都打印一行日志，样例仅由第一个线程输出周期统计结果。

  ```text
  execute_cycle = clock_after_gather - clock_before_gather
  ```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j
  ./demo
  ```

  使用 NPU 仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU 仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：`dav-3510` 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明时间打点功能和精度对比成功。

  ```text
  simt_gather execute cycle : 22289
  test pass!
  ```
