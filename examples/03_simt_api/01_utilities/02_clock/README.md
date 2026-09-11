# 基于SIMT clock接口的时间戳打点功能实现样例

## 概述

本样例演示在SIMT编程下使用```clock()```接口实现时间戳打点的方法。样例以SIMT方式实现Gather计算，在核函数执行前后使用`clock()`记录时间戳，用于统计执行周期。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── 02_clock
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── clock.asc              // Ascend C算子实现加clock打点的调用样例
│   └── README.md
```

## 算子描述

- 算子功能:

  本样例完成 Gather计算，计算公式如下：

  ```text
  output[i] = input[index[i]]
  ```

  Host侧直接构造输入数据和golden数据，执行后在进程内完成结果校验，不依赖额外的数据生成或校验脚本。

- 算子规格:

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">gather</td></tr>
  <tr><td align="center"></td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[12288]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[12288]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">simt_gather</td></tr>
  </table>

- clock打点说明:

  `simt_gather`核函数中在Gather计算前调用一次`clock()`记录开始时间戳，在计算后再次调用`clock()`记录结束时间戳，并输出两次打点之间的周期差。为避免每个SIMT线程都打印一行日志，样例仅由第一个线程输出周期统计结果。

  ```text
  execute_cycle = clock_after_compute - clock_before_compute
  ```

- 算子实现:

  ```cpp
  __global__ void simt_gather(float* input, int32_t* index, float* output, uint64_t index_total_length)
  {
      uint64_t start = clock();
  
      int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
  
      // Maps to the index of output tensor
      if (idx >= index_total_length) {
          return;
      }
      output[idx] = input[index[idx]];
  
      uint64_t end = clock();
      if (blockIdx.x == 0 && threadIdx.x == 0) {
          printf("%s execute cycle : %lu\n", "simt_gather", end - start);
      }
  }
  ```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # 编译工程
  ./demo                        # 执行样例
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行后有如下打印信息，说明时间打点功能和精度对比成功。

  ```text
  simt_gather execute cycle : 3479
  [Success] Case accuracy is verification passed.
  ```
