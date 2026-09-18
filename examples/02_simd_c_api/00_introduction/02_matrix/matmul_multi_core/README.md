# 基于SIMD C API实现多核Matmul计算

## 概述

本样例基于SIMD C API实现多核矩阵乘计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── matmul_multi_core
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_multi_core.asc   // SIMD C API样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  本样例使用SIMD C API实现一个最基础的矩阵乘法（Matmul）[核函数](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/kernel_function.md)。矩阵乘法的计算公式如下：
  $$
  C = A * B
  $$
  其中，A矩阵的形状为`[M, K]`，B矩阵的形状为`[K, N]`，输出C矩阵的形状为`[M, N]`。对输出矩阵C中的每一个元素`C[m, n]`，都会累加A矩阵第`m`行和B矩阵第`n`列在K轴上的乘积。在矩阵乘法中，**M方向**指矩阵C的行方向，**N方向**指矩阵C的列方向，**K方向**指矩阵C乘法的内维（累加维度）。

- 样例规格：  
  本样例参数`M = 256, N = 256, K = 64`，输入输出均为`half`类型、[`ND`](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md)格式。样例启动2个核完成计算，每个核负责输出矩阵C在M轴方向的128行、N轴方向的全部256列：
  - 第0个核计算C矩阵的第`0~127`行。
  - 第1个核计算C矩阵的第`128~255`行。

  输入输出规格如下表所示：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">matmul_multi_core_custom</td></tr>
  </table>

- 样例实现：
  - Kernel侧整体思路
    - `matmul_multi_core_custom`是一个[`__global__`](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md) [`__cube__`](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md)核函数，表示该函数运行在[AI Core](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/glossary.md)的[Cube](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/glossary.md)计算单元上，主要用于矩阵计算。
    - 样例使用SIMD C API编程方式，通过`__gm__`、`__cbuf__`、`__ca__`、`__cb__`和`__cc__`地址空间限定符声明GM以及L1 Buffer、L0A Buffer、L0B Buffer和L0C Buffer中的数据。
    - `CUBE_BLOCK = 16`表示half数据类型分形为`16 x 16`，代码中按`16 x 16`的分形为单位调用`asc_copy_l12l0a`和`asc_copy_l12l0b_transpose`进行搬运。

  - Kernel侧详细流程
    - 核函数参数`a`、`b`、`c`为[GM（Global Memory，全局内存）](../../../../../docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md)中的A、B、C矩阵地址，在Kernel侧分别转换为`__gm__ half*`类型的`aGM`、`bGM`、`cGM`。
    - 通过内置变量`block_idx`获取当前核号，并计算`mIterIdx`。本样例只沿M轴切分任务，因此每个核只需要处理A矩阵和C矩阵中属于自己的M轴分片。
    - 设置GM地址偏移：
      - `aGM`偏移`mIterIdx * singleCoreM * K`，使当前核读取自己负责的A矩阵行块。
      - `bGM`不偏移，因为每个核都需要读取完整B矩阵。
      - `cGM`偏移`mIterIdx * singleCoreM * N`，使当前核把结果写回C矩阵中自己负责的行块。
    - 通过地址空间限定符声明片上数组：
      - `a1Local`：A矩阵在L1 Buffer中的临时存储。
      - `a2Local`：A矩阵在L0A Buffer中的临时存储，供`asc_mmad`读取。
      - `b1Local`：B矩阵在L1 Buffer中的临时存储。
      - `b2Local`：B矩阵在L0B Buffer中的临时存储，供`asc_mmad`读取。
      - `cLocal`：矩阵乘结果在L0C Buffer中的临时存储。
    - 调用[`asc_set_gm2l1_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_nz_para.md)配置目的Nz排布，再调用[`asc_copy_gm2l1_nd2nz`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md)将A、B矩阵从GM搬运到L1 Buffer，在搬运过程中将输入的[ND](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md)格式数据转换为Cube计算需要的[Nz](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md)格式。
    - 调用[`asc_sync_notify`](../../../../../docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md)和[`asc_sync_wait`](../../../../../docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md)进行同步。`asc_copy_gm2l1_nd2nz`属于MTE2流水，后续L1 Buffer到L0A Buffer/L0B Buffer的搬运属于[MTE1](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/glossary.md)流水，MTE1必须等待MTE2完成，避免读取到尚未搬运完成的L1 Buffer数据。
    - 调用[`asc_copy_l12l0a`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)将A矩阵从L1 Buffer搬运到L0A Buffer，调用[`asc_copy_l12l0b_transpose`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)将B矩阵从L1 Buffer转置搬运到L0B Buffer。L0A Buffer和L0B Buffer是Cube矩阵计算单元直接读取的输入缓存。
    - 调用`asc_sync_notify`和`asc_sync_wait`进行同步。L1 Buffer到L0A Buffer/L0B Buffer的搬运属于MTE1流水，后续`asc_mmad`属于PIPE_M流水，PIPE_M流水必须等待MTE1完成，避免读取到尚未搬运完成的L0A Buffer/L0B Buffer数据。
    - 调用[`asc_mmad`](../../../../../docs/zh/api/SIMD-API/c_api/cube_compute/asc_mmad.md)`(cLocal, a2Local, b2Local, baseM, baseK, baseN, asc_unit_flag_mode::DISABLE, false, false, true)`执行矩阵乘。这里`baseM = 128`、`baseN = 256`、`baseK = 64`，对应单个核一次计算的矩阵块大小。
    - 调用`asc_sync_notify`和`asc_sync_wait`进行同步。`asc_mmad`属于PIPE_M流水，后续[`asc_copy_l0c2gm`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)属于FIX流水，FIX流水必须等待PIPE_M流水完成，避免读取到尚未计算完成的L0C Buffer结果。
    - 调用[`asc_set_l0c_copy_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_nz_para.md)配置Nz到ND的单矩阵搬运，再调用`asc_copy_l0c2gm`将L0C Buffer中的`float`累加结果转换为`half`并搬运回GM中的C矩阵输出位置。
    - 最后调用[`asc_sync_pipe`](../../../../../docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_pipe.md)`(PIPE_ALL)`，确保当前核内相关流水任务完成。

  - 调用实现  
    使用[内核调用符](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/kernel_function.md)`<<<>>>`调用[核函数](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/kernel_function.md)。调用时模板参数传入矩阵规格、单核计算量和基础Tile大小，运行时参数传入Device侧A、B、C矩阵地址。

- 接口参数说明：

  **[`asc_set_gm2l1_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_nz_para.md)和[`asc_copy_gm2l1_nd2nz`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md)** — 配置目的Nz排布，并完成GM到L1 Buffer的ND→Nz格式转换：
  ```cpp
  asc_set_gm2l1_nz_para(
      matrix_num,             // 源操作数中ND矩阵的数量
      dst_nz_n_stride,        // 目的Nz矩阵中相邻行的起始地址偏移，单位：32B
      dst_nz_c0_stride,       // 目的Nz矩阵中相邻Z分形的起始地址偏移，单位：32B
      dst_nz_matrix_stride);  // 目的操作数中相邻Nz矩阵的起始地址偏移，单位：32B

  asc_copy_gm2l1_nd2nz(
      dst,                    // 目的L1 Buffer地址
      src,                    // 源GM地址
      loop1_src_stride,       // 源矩阵相邻行的起始地址偏移，单位：字节
      l2_cache_ctl,           // L2 Cache管理策略
      n_value,                // ND矩阵行数，单位：元素
      d_value,                // ND矩阵列数，单位：元素
      loop4_src_stride,       // 相邻ND矩阵的起始地址偏移，单位：字节
      enable_small_c0);       // 是否开启SmallC0模式
  ```
  例如搬运A矩阵时，先调用`asc_set_gm2l1_nz_para(1, 1, baseM, 0)`，再调用`asc_copy_gm2l1_nd2nz(a1Local, aGM, K * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, baseM, baseK, 0, false)`，将baseM×baseK的ND数据转为Nz格式。

  **[`asc_copy_l12l0a`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)和[`asc_copy_l12l0b_transpose`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)** — 描述Ascend 950PR/Ascend 950DT产品中A矩阵从L1 Buffer到L0A Buffer和B矩阵从L1 Buffer到L0B Buffer的数据搬运参数：
  ```cpp
  asc_copy_l12l0a/asc_copy_l12l0b_transpose(
      dst,               // 目的L0A Buffer/L0B Buffer地址
      src,               // 源L1 Buffer地址
      m_start_position,  // M方向起始分形位置，单位：16个元素
      k_start_position,  // K方向起始分形位置，单位：32B
      m_step,            // M方向搬运的分形行数
      k_step,            // K方向搬运的分形列数
      src_stride,        // 源矩阵K方向相邻分形的起始地址间隔，单位：512B
      dst_stride);       // 目的矩阵K方向相邻分形的起始地址间隔，单位：512B
  ```
  Ascend 950PR/Ascend 950DT产品中，L0A Buffer上的排布格式为Nz。搬运A矩阵时使用`asc_copy_l12l0a(a2Local, a1Local, 0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, baseM / CUBE_BLOCK)`，一次完成A矩阵Nz到Nz搬运；搬运B矩阵时使用`asc_copy_l12l0b_transpose(b2Local, b1Local, 0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK)`，一次完成B矩阵Nz到Zn搬运。

  **[`asc_mmad`](../../../../../docs/zh/api/SIMD-API/c_api/cube_compute/asc_mmad.md)** — 描述矩阵乘参数：
  ```cpp
  asc_mmad(
      c_matrix,          // 结果矩阵C在L0C Buffer中的起始地址
      a_matrix,          // 左矩阵A在L0A Buffer中的起始地址
      b_matrix,          // 右矩阵B在L0B Buffer中的起始地址
      m,                 // 左矩阵Height（M维），[0, 4095]
      k,                 // 左矩阵Width/右矩阵Height（K维），[0, 4095]
      n,                 // 右矩阵Width（N维），[0, 4095]
      unit_flag_mode,    // asc_mmad与矩阵搬出指令细粒度并行控制
      disable_gemv,      // M为1时是否关闭GEMV模式
      c_matrix_source,   // C矩阵初始值来源，false=L0C Buffer，true=BiasTable
      c_matrix_init_val);// 是否将C矩阵初始值设置为0
  ```
  本样例使用`asc_mmad(cLocal, a2Local, b2Local, baseM, baseK, baseN, asc_unit_flag_mode::DISABLE, false, false, true)`，计算baseM×baseN输出块并在K方向累加baseK长度，计算前将C矩阵初始值清零。

  **[`asc_set_l0c_copy_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_nz_para.md)和[`asc_copy_l0c2gm`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)** — 配置Nz矩阵转换参数，完成L0C Buffer到GM的数据搬运和精度转换：
  ```cpp
  asc_set_l0c_copy_nz_para(
      matrix_num,          // 源Nz矩阵数量
      src_matrix_stride,   // 相邻源Nz矩阵的起始地址偏移
      dst_matrix_stride);  // 相邻目的矩阵的起始地址偏移

  asc_copy_l0c2gm(
      dst,                   // 目的GM地址
      src,                   // 源L0C Buffer地址
      n_size,                // 源Nz矩阵N方向大小
      m_size,                // 源Nz矩阵M方向大小
      dst_stride,            // 目的ND矩阵每行元素数
      src_stride,            // 源Nz矩阵相邻Z排布的起始地址偏移，单位：64B
      l2_cache_mode,         // 输出GM数据的L2 Cache管理策略
      unit_flag_mode,        // asc_mmad与矩阵搬出指令细粒度并行控制
      quant_pre_mode,        // 预处理量化模式，F322F16表示float→half
      relu_pre_mode,         // 预处理ReLU模式
      enable_channel_split,  // 是否开启ChannelSplit
      enable_nz2nd,          // 是否开启Nz→ND转换
      enable_nz2dn,          // 是否开启Nz→DN转换
      enable_clip_relu_pre); // 是否开启预处理阶段Clip ReLU
  ```
  本样例先调用`asc_set_l0c_copy_nz_para(1, 0, 0)`配置单矩阵搬运，再调用`asc_copy_l0c2gm(cGM, cLocal, baseN, baseM, N, baseM, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, QuantMode_t::F322F16, asc_relu_pre_mode::NONE, false, true, false, false)`，将L0C Buffer中的baseM×baseN float32结果转换为half、从Nz转换为ND并写回GM。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为`${HOME}/Ascend`。
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：dav-3510对应Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
