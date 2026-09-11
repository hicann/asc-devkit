# 原子操作接口使用样例

## 概述

本样例展示了Ascend C SIMT原子操作接口的用法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── atomic_intrinsics
│   ├── CMakeLists.txt         // 编译工程文件
│   └── atomic_intrinsics.asc  // Ascend C SIMT核函数实现 & Host调用样例
```

## 样例描述

- 样例功能：在核函数中调用Ascend C SIMT提供的原子操作接口，对`output_data`数组的11个元素分别执行加法、减法、交换、比较交换、自增、自减、按位与、按位或、按位异或等原子操作，结果分别存放在`output_data`数组的11个元素中。

- 计算过程：

  - 核函数启动`64`个线程块，每块`256`个线程，共`16384`个线程
  - 每个线程按全局线程号`tid`，对长度为`11`的`output_data`数组各元素执行一次原子操作
  - Host侧在核函数执行完毕后，将结果拷回并与CPU参考实现逐项比对

- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型（OpType）</td><td colspan="4" align="center">Atomic Intrinsics</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">output_data</td><td align="center">[11]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output_data</td><td align="center">[11]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">atomic_intrinsics_kernel</td></tr>
  </table>

## 样例实现

核函数`atomic_intrinsics_kernel`中，每个线程对`output_data`数组的11个元素各执行一次原子操作，不同索引的元素对应不同的原子操作，对应关系如下表所示：

| 数组索引 | 接口 |
| :--: | :-- |
| 0 | asc_atomic_add |
| 1 | asc_atomic_sub |
| 2 | asc_atomic_exch |
| 3 | asc_atomic_max |
| 4 | asc_atomic_min |
| 5 | asc_atomic_inc |
| 6 | asc_atomic_dec |
| 7 | asc_atomic_cas |
| 8 | asc_atomic_and |
| 9 | asc_atomic_or |
| 10 | asc_atomic_xor |

关键代码：

```cpp
__global__ void atomic_intrinsics_kernel(int* output_data)
{
    const unsigned int tid = blockIdx.x * blockDim.x + threadIdx.x;

    asc_atomic_add(&output_data[0], 5);
    asc_atomic_sub(&output_data[1], 5);
    asc_atomic_exch(&output_data[2], tid);
    asc_atomic_max(&output_data[3], tid);
    asc_atomic_min(&output_data[4], tid);
    asc_atomic_inc((unsigned int*)&output_data[5], 11u);
    asc_atomic_dec((unsigned int*)&output_data[6], 100u);
    asc_atomic_cas(&output_data[7], tid - 1, tid);
    asc_atomic_and(&output_data[8], 2 * tid + 1);
    asc_atomic_or(&output_data[9], 1u << (tid & 31u));
    asc_atomic_xor(&output_data[10], tid);
}
```


## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
   请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行
  在本样例目录下执行如下命令。
  ```bash
  mkdir build && cd build;                              # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # 编译工程
  ./atomic_intrinsics                                   # 执行样例
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项                      | 可选值     | 说明                                                       |
  | ------------------------- | ---------- | ---------------------------------------------------------- |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明原子操作校验成功。

  ```text
  Atomic Intrinsics Example starting...
  Atomic Intrinsics Example completed, returned OK
  ```
