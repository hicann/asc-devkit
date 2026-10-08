# 基于Tensor API实现的Matmul Bias CV融合样例

## 概述

本样例基于Tensor API编程方式实现Cube和Vector的CV融合计算。Cube侧使用Tensor API完成矩阵乘法和Bias融合，Vector侧使用VF接口将Cube结果与Residual逐元素相加，最终实现：

$$
Y = A \times B + Bias + Residual
$$

样例使用一个`__mix__(1, 2)` kernel，同时启动1个AIC和2个AIV。Cube将L0C结果通过`copy_l0c_to_ub`的`dual_dst_mode::split_m`按M轴拆分到两个Vector子块，Vector完成加法后将结果写回GM，展示同一kernel内的CV数据传递和流水协同。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.2.0 |

> **说明：** 该样例依赖尚未正式发布的CANN特性，请使用最新的CANN master包。

## 目录结构介绍

```text
├── matmul_bias_fusion
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和float32真值生成脚本
│   │   └── verify_result.py               // 输出与真值对比脚本
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── matmul_bias_fusion.asc             // Ascend C样例实现和调用样例
│   └── README.md                          // 样例说明文档
```

## 样例描述

- 样例功能：

  1. **Cube侧Matmul和Bias融合**

     Cube侧将A和B从GM搬运到L1，再搬运到L0A/L0B，使用`mmad`完成矩阵乘法。Bias从GM经L1和Bias表搬运到L0Bias，仅在每个输出tile的第一次`mmad`中传入；后续K迭代只累加矩阵乘结果，避免重复添加Bias。

  2. **Vector侧Residual加法**

     Cube侧把L0C中的`float`结果搬运到UB。`dual_dst_mode::split_m`将一个tile沿M轴平均拆为两个目标，分别提供给两个AIV。每个AIV从对应半块读取Cube结果和GM中的Residual，调用`ComputeAdd`完成逐元素相加，再将结果写回GM。

  3. **CV流水协同**

     输出tile使用两组UB slot（`PIPE_DEPTH=2`）交替复用。AIC在复用slot前等待两个AIV完成上一轮消费；完成L0C到UB搬运后通知AIV；两个AIV完成加法和UB到GM写回后再通知AIC。跨AIC/AIV通知使用`asc_sync_block_wait`和`asc_sync_block_arrive`，不会为Vector路径另起一个kernel。

  4. **计算公式**

     Bias按列广播，Residual与矩阵结果逐元素相加：

     $$
     T_{m,n}=\sum_{k=0}^{K-1} A_{m,k}B_{k,n}+Bias_n
     $$
     $$
     Y_{m,n}=T_{m,n}+Residual_{m,n}
     $$

- `ComputeAdd`实现：

  `ComputeAdd`是标记为`__simd_vf__`的Vector函数，输入和输出均为UB中的一维Tensor。函数先通过`update_mask<float>(length)`生成有效元素掩码，再对两个输入Tensor执行带掩码的`load`，计算`lhs + rhs`，最后使用`store`写入输出Tensor。`length`由当前列尾块的有效元素数决定，因此不会访问tile中的padding元素。

  ```cpp
  template <typename Src0Tensor, typename Src1Tensor, typename DstTensor>
  __simd_vf__ inline void ComputeAdd(Src0Tensor src0, Src1Tensor src1,
      DstTensor dst, uint32_t length)
  {
      const auto coord = asc::te::make_coord(0);
      auto mask = asc::te::experimental::update_mask<float>(length);
      auto lhs = asc::te::experimental::load(src0, coord).with_mask(mask);
      auto rhs = asc::te::experimental::load(src1, coord).with_mask(mask);
      asc::te::experimental::store(dst, coord, lhs + rhs);
  }
  ```

- 样例规格：

  默认Shape为`M=1024`、`N=1024`、`K=256`。每个Cube block处理`singleCoreM=256`、`singleCoreN=128`的输出区域，共启动32个mixed block。

  <table border="2">
  <caption>样例规格表</caption>
  <tr><td rowspan="1" align="left">样例类型(OpType)</td><td colspan="4" align="left">Matmul with Bias and Residual</td></tr>
  <tr><td rowspan="6" align="left">样例输入</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">A（矩阵A）</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">B（矩阵B）</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">Bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">Residual</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling结构体</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">样例输出</td><td align="left">Y（输出矩阵）</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">核函数名</td><td colspan="4" align="left">matmul_bias_fusion</td></tr>
  </table>

  编译期参数默认值如下：

  | 参数 | 默认值 | 说明 |
  | :--- | :---: | :--- |
  | `BASE_M` | 128 | Cube输出tile的M维大小 |
  | `BASE_N` | 128 | Cube输出tile的N维大小 |
  | `BASE_K` | 64 | Cube K维基础分块大小 |
  | `STEP_M` | 1 | L1 M维步进 |
  | `STEP_N` | 1 | L1 N维步进 |
  | `STEP_K` | 4 | L1 K维步进 |
  | `VECTOR_LEN` | 64 | 一次Vector加法的最大元素数 |
  | `PIPE_DEPTH` | 2 | UB slot数量 |

  运行时Tiling参数默认值如下：

  | 参数 | 默认值 | 说明 |
  | :--- | :---: | :--- |
  | `m` | 1024 | 矩阵A的行数 |
  | `n` | 1024 | 矩阵B的列数 |
  | `k` | 256 | 矩阵A的列数/矩阵B的行数 |
  | `singleCoreM` | 256 | 单个Cube block处理的M维大小 |
  | `singleCoreN` | 128 | 单个Cube block处理的N维大小 |
  | `singleCoreK` | 256 | 单个Cube block处理的K维大小 |

## 样例实现

### Tensor布局和数据搬运

| Tensor | 布局 | 用途 |
| :--- | :--- | :--- |
| GM中的A、B、Bias、Residual、Y | `nd_ext_layout_ptn`（ND） | 描述主存中的输入和输出 |
| L1中的A、B | `nz_layout_ptn`（NZ） | 为Cube搬运和计算准备数据 |
| L1中的Bias | `nd_ext_layout_ptn`（ND） | 暂存Bias列向量 |
| L0A | `nz_layout_ptn`（NZ） | Cube矩阵A |
| L0B | `zn_layout_ptn`（ZN） | Cube矩阵B |
| L0C | `nz_layout_ptn`（NZ） | float矩阵乘累加结果 |
| UB中的tile和Vector临时Tensor | ND/一维布局 | AIC到AIV的数据交接及VF计算 |

### 实现流程

| 步骤 | Tensor API/C API操作 | 功能 | 布局或执行单元 |
| :--- | :--- | :--- | :--- |
| 1 | `make_tensor`、`make_mem_ptr`、`make_frame_layout` | 创建GM、L1、L0和UB Tensor视图，并按block索引取得当前M/N区域 | GM为ND，Cube缓存为NZ/ZN |
| 2 | `copy(copy_gm_to_l1)` | 搬运A、B和Bias到L1 | A/B为ND到NZ，Bias保持ND |
| 3 | `copy(copy_l1_to_l0a/copy_l1_to_l0b/copy_l1_to_biastable)` | 搬运Cube输入和Bias到L0A、L0B、L0Bias | 准备Cube计算 |
| 4 | `mmad` | 按K维迭代完成矩阵乘累加，并在第一次迭代注入Bias | AIC/Cube |
| 5 | `copy(copy_l0c_to_ub)` | 使用`dual_dst_mode::split_m`把L0C tile拆成两个M半块 | L0C结果按M轴双目标搬运到两个AIV各自的UB |
| 6 | `asc_sync_block_wait/arrive` | 在AIC和两个AIV之间传递每个slot的就绪和消费完成事件 | CV同步 |
| 7 | `copy(copy_ub_to_ub)`、`copy(copy_gm_to_ub)` | 读取Cube半块和Residual到Vector临时UB | AIV/Vector |
| 8 | `ComputeAdd` | 对有效元素执行masked `load`、逐元素加法和`store` | AIV/VF |
| 9 | `copy(copy_ub_to_gm)` | 将加法结果写回Y对应的GM区域 | AIV到GM |

### Cube侧Matmul流程

每个AIC block根据`block_idx`定位一个`singleCoreM × singleCoreN`区域，在该区域内按`BASE_M`、`BASE_N`划分输出tile。对每个tile，A和B沿K轴按`BASE_K`循环搬运并调用`mmad`：第一次调用设置`init_with_zero=true`并传入L0Bias，后续调用设置`init_with_zero=false`，从而得到`A × B + Bias`的L0C结果。

### Vector侧加法流程

每个AIV通过`asc_get_sub_block_id()`确定自己负责的M半块。收到AIC对应slot的通知后，逐行、逐`VECTOR_LEN`列处理：先将UB中的Cube结果复制到Vector临时Tensor，再把Residual从GM搬到UB，调用`ComputeAdd`，最后将输出Tensor复制到Y的GM切片。两个AIV负责互不重叠的行区间。

### L0C到UB的双目标和同步

`copy_l0c_to_ub`使用以下trait：

```cpp
{ asc::te::round_mode::default_round, false, false,
  asc::te::dual_dst_mode::split_m }
```

对于每个输出tile，AIC把前半M写入sub-block 0、后半M写入sub-block 1。两组slot按`sequence % PIPE_DEPTH`复用，AIC到AIV的flag为8、9，AIV到AIC的flag为10、11。AIC在重用slot前通过`PIPE_MTE2`等待两个AIV完成，AIC完成搬运后在`PIPE_FIX`通知AIV；AIV在`PIPE_S`等待数据就绪，并在`PIPE_MTE3`完成写回后通知AIC，保证UB不会在Vector读取前被覆盖，形成完整的CV流水依赖。

## Tensor API实现特点

| 特性 | 本样例实现 |
| :--- | :--- |
| 张量表示 | 使用Tensor对象描述GM、L1、L0C和UB中的各级数据 |
| 矩阵计算 | 使用`mmad`完成Cube侧矩阵乘加，Bias仅在首次K迭代注入 |
| CV融合 | 使用一个`__mix__(1, 2)` kernel连接1个AIC和2个AIV |
| 双目标搬运 | 使用`copy_l0c_to_ub`的`dual_dst_mode::split_m`按M轴拆分 |
| Vector计算 | 使用`__simd_vf__`函数`ComputeAdd`完成加法 |
| 数据同步 | 使用固定flag的`asc_sync_block_wait/arrive`实现slot级交接 |
| 尾块处理 | 以`curM`、`curN`和`length`限制有效区域，避免读取padding |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。**当前仅支持使用[CANN master](../../../../docs/zh/quick_start.md#cann-install)**。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行
  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;                                                                  # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j;          # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py                                                               # 生成测试输入数据
  ./demo                                                                                       # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin                      # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：dav-3510对应Ascend 950PR&950DT系列产品 |
  | `CANN_ASC_USE_EXPERIMENTAL` | `ON` | 本样例编译所需的Tensor API开关 |

  > **说明：** 本样例仅支持dav-3510架构（对应Ascend 950PR&950DT系列产品）。

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
