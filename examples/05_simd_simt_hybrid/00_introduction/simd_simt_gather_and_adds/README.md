# SIMD与SIMT混合编程实现gather和adds计算


## 概述
本样例基于SIMD和SIMT混合编程模式实现gather和adds计算，以SIMT编程方式实现离散内存访问操作gather，以SIMD编程方式实现连续内存访问操作adds。


## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── simd_simt_gather_and_adds
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // cmake编译文件
│   ├── gather_and_adds.asc     // Ascend C样例实现 & 调用样例
│   ├── data_utils.h            // 数据读入写出函数
│   └── README.md
```

## 样例描述

- 样例功能：  
  计算公式：

  ```
  output[i] = input[index[i]] + 1
  ```

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">gather & adds</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[8192]</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gather_and_adds_kernel</td></tr>
  </table>

- 样例实现：  
  Vector Core中SIMT单元和SIMD单元共享片上存储，可以使用片上存储完成SIMT和SIMD的混合编程。本例中样例输入index的shape为[8192]，可设置核数为8，每个核处理数据量为1024，设置线程数`THREAD_COUNT`为1024，每个线程处理1个数据元素，单个核只需调用1次simt_gather函数即可完成gather运算。

  > ⚠️ **注意** 当单核处理数据量大于设置的线程数时，需要切分数据到多个线程块，可使用asc_vf_call多次调用simt_gather函数启动多个线程块完成获取指定索引数据的操作。

  基于上述数据拆分，在simd_adds函数中，处理1024个数据元素的加1操作。

  > ⚠️ **注意** simd_adds中加1运算实际可以直接在simt_gather函数中快速实现，本例目的仅仅是通过一个简单用例展示SIMT和SIMD两种编程模式的混合编程方式，不是该样例最佳实践。

  gather & adds样例的实现流程主要分为3个步骤：simt_gather，simd_adds和数据搬出。

  （1）simt_gather从GM（Global Memory）输入中获取指定索引的数据。
  ```
  uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
  ...
  uint32_t gather_idx = index[idx];
  ...
  local_output[threadIdx.x] = input[gather_idx];
  ```

  （2）simd_adds将UB（Unified Buffer）中数据做加1操作。调用asc_loadalign将数据从UB（Unified Buffer）搬运到寄存器上，调用asc_add_scalar完成加1运算并输出到目标寄存器，最后调用asc_storealign将数据从寄存器搬运到UB。重复上述操作即可完成1024个数据元素的加1运算。
  ```
  for (uint16_t i = 0; i < repeat_times; i++) {
      mask_reg = asc_update_mask_b32(count);
      asc_loadalign(src_reg0, input + i * one_repeat_size);
      asc_add_scalar(dst_reg0, src_reg0, ADDS_ADDEND, mask_reg);
      asc_storealign(output + i * one_repeat_size, dst_reg0, mask_reg);
  }
  ```

  （3）将输出数据从UB（Unified Buffer）搬出至GM（Global Memory）上。

- 调用实现：  
  使用内核调用符<<<>>>调用核函数。

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
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确
  ```

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```
  test pass!
  ```
