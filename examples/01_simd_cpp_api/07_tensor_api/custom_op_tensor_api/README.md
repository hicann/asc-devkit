# 基于Tensor API实现的自定义Op样例

## 概述

本样例基于Tensor API实现固定Shape的Query-Key-Value（QKV）投影，重点展示如何定义一个自定义Copy Operation，并将它接入Tensor API的Atom调用流程。

在Transformer的自注意力计算中，输入隐藏状态通常需要分别经过三个线性投影，得到查询（Q）、键（K）和值（V）。三路结果的隐藏维度均为`H`，为了合并计算，可以沿最后一维连续拼接为`[B, S, 3H]`，其中`3H = H(Q) + H(K) + H(V)`。因此，本样例使用`weight[H, 3H]`和`bias[1, 3H]`一次完成三路投影，输出最后一维的前、中、后三段分别对应Q、K、V；后续注意力计算可再按这三个`H`维分段拆分。

输入是Transformer中的隐藏状态（hidden_states），在样例中以Batch模式的三维Batch ND（N-Dimension，标准数据排列）张量表示，逻辑Shape为[B, S, H]。自定义Operation在Global Memory到L1 Buffer的搬运过程中，将输入的Batch维和Sequence维展平，即逻辑Shape从[B, S, H]转换到[B * S, H]，同时完成ND到NZ（分形矩阵排列）的格式转换：

```text
input_flat = reshape(input, [B * S, H])
output = input_flat * weight + bias
```

其中`weight[H, 3H]`和`bias[1, 3H]`在所有Batch之间共享。Q、K、V三个投影结果在输出的最后一维中连续排列，本样例只负责投影计算，不再拆分Q、K、V。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|--------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── scripts
│   ├── gen_data.py             // 生成输入数据和CPU侧真值
│   └── verify_result.py        // 比对样例输出和真值
├── CMakeLists.txt                  // 编译工程文件
├── custom_op_tensor_api.asc       // 自定义Operation、Kernel和Host调用
├── data_utils.h                   // 二进制数据读写工具
├── README_en.md                   // 英文说明文档
└── README.md                      // 中文说明文档
```

## 样例描述

### 样例规格

| Tensor | 逻辑Shape | Global Memory Format | 数据类型 | 说明 |
|--------|------------|-----------|----------|------|
| `input` | `[B, S, H] = [16, 64, 256]` | Batch ND | `half` | 输入隐藏状态 `hidden_states` |
| `weight` | `[H, 3H] = [256, 768]` | ND | `half` | 共享投影权重 |
| `bias` | `[1, 3H] = [768]` | ND | `half` | 共享偏置 |
| `output` | `[B*S, 3H] = [1024, 768]` | ND | `half` | 投影结果 |

输出在逻辑上也可以解释为`[B, S, 3H] = [16, 64, 768]`。展平只改变逻辑索引，不需要额外的数据搬运。

矩阵乘法参数为：

```text
M = B * S = 1024
K = H     = 256
N = 3 * H = 768
```

### 自定义Copy Operation

本样例的核心是`copy_slice_gm_to_l1`。输入需要自定义的原因是它的Global Memory源Tensor保留三维Batch ND的Stride，而L1 Buffer目标Tensor是二维NZ；权重和Bias是普通二维ND搬运，仍使用标准`copy_gm_to_l1`即可。

矩阵乘法本身使用Tensor API已有的`mmad`和标准搬运Atom；只有输入从Global Memory搬到L1 Buffer时，需要同时完成“Batch ND展平”和“ND到NZ格式转换”，因此定义了这个自定义Copy Operation来封装底层搬运，并将不符合标准二维Copy语义的Batch输入搬运接入统一的Tensor API `copy(atom, dst, src)`调用流程。

#### Operation实现

`copy_slice_gm_to_l1`实现了一个静态模板函数`copy`，从源、目的Tensor中读取元素类型、Shape、Stride、目的NZ布局和Cache Mode，再根据这些信息生成底层C API所需的参数。它处理的逻辑格式为：

| Tensor | 存储位置 | Shape | LayoutPattern |
|--------|----------|-------|---------------|
| 源 | Global Memory | `[B_tile, S, K_tile]` | Batch模式`nd_layout_ptn` |
| 目的 | L1 Buffer | `[B_tile * S, K_tile]` | `nz_layout_ptn` |

当前Kernel每次处理一个`base_m`乘`base_k`的输入Tile，因此：

```text
B_tile = base_m / S = 128 / 64 = 2
K_tile = base_k = 64
[2, 64, 64] Batch ND -> [128, 64] NZ
```

Tensor API通过`copy_traits`将一个Copy Operation与其执行Trait绑定，`make_copy`再根据该Traits创建可调用的Atom。为了把`copy_slice_gm_to_l1`接入这条通用流程，样例提供了两个`copy_traits`特化：

```cpp
// 传入自定义Trait时，保持该Trait并绑定当前Operation。
template <typename Trait>
struct copy_traits<copy_slice_gm_to_l1, Trait>
    : public copy_traits<copy_slice_gm_to_l1, Trait, copy_slice_gm_to_l1, Trait> {};

// 未显式传入Trait时，默认使用Global Memory到L1 Buffer的标准Trait。
template <>
struct copy_traits<copy_slice_gm_to_l1> : public copy_traits<copy_slice_gm_to_l1, gm_to_l1_trait_default> {};
```

这样，`copy_slice_gm_to_l1`就能像标准Copy Operation一样创建和调用：

```cpp
const auto custom_copy_atom = make_copy(copy_slice_gm_to_l1{}, gm_to_l1_trait_default{});
copy(custom_copy_atom, input_l1_tensor, input_gm_slice);
```

在Kernel中，自定义Atom只用于输入：

```cpp
copy(custom_copy_atom, input_l1_tensor, input_gm_tensor.slice(...));
copy(weight_l1_tensor, weight_gm_tensor.slice(...));
```

#### 参数推导

Operation不把当前参数写死，而是从Tensor Layout推导搬运参数。对于当前`[2, 64, 64]`输入Slice和`half`类型，推导关系如下：

| 参数 | 当前值 | 推导方式与含义 |
|------------|------------|------------------|
| `matrix_num` | 2 | 源Shape的Batch Tile，即`base_m / S` |
| `n_value` | 64 | 源Shape的Sequence长度，即`S` |
| `d_value` | 64 | 源Shape的K方向Tile，即`base_k` |
| `src_d_value` | 512 Byte | 源Stride中相邻Sequence行的间隔，即`H * sizeof(half)` |
| `src_nd_matrix_stride` | 32768 Byte | 源Stride中相邻Batch矩阵的间隔，即`S * H * sizeof(half)` |
| `dst_nz_n_stride` | 1 | 目标NZ相邻行的起始地址间隔，单位为32 Byte |
| `dst_nz_c0_stride` | 128 | 从目标NZ Layout的列Stride除以`c0_element<half>`得到，单位为32 Byte |
| `dst_nz_matrix_stride` | 64 | 目标NZ中相邻Batch矩阵的起始地址间隔，取值为`n_value`，单位为32 Byte；当前值对应2048 Byte |
| `enable_small_c0` | `false` | 当前`d_value=64`，不使用SmallC0模式 |

其中，`src_d_value`和`src_nd_matrix_stride`使用源Tensor的原始Stride计算，而不是假定源数据已经展平；这样第二个Batch会从正确的Global Memory地址开始读取。`dst_nz_matrix_stride`保证多个Batch依次写入同一个`[base_m, base_k]` NZ目标，不发生覆盖。

#### 底层C API调用

参数推导完成后，`copy`函数将工作交给私有辅助函数`copy_gm_to_cbuf_multi_nd2nz`。该辅助函数负责数据类型分派、架构条件编译和底层指针转换：它根据数据类型将Tensor指针转换为`__gm__`和`__cbuf__`指针，当前支持`half`和`float`两种类型，并在编译期选择对应的C API重载。3510架构分支中，调用顺序固定为先设置NZ搬运参数，再执行ND2NZ搬运：

```cpp
asc_set_gm2l1_nz_para(matrix_num, dst_nz_n_stride, dst_nz_c0_stride, dst_nz_matrix_stride);
asc_copy_gm2l1_nd2nz(
    dst, src, src_d_value, l2_cache_mode, n_value, d_value, src_nd_matrix_stride, enable_small_c0);
```

源Tensor的Cache Mode通过`src.engine().get_cache_mode()`取得，并转换为C API要求的`asc_load_l2_cache_mode`后传入。`asc_set_gm2l1_nz_para`负责配置目标NZ的步长关系，`asc_copy_gm2l1_nd2nz`负责按源Stride读取多Batch ND矩阵并完成格式转换；两者缺一不可。

#### 使用约束

- 源Tensor必须是Global Memory中的三维Batch模式`nd_layout_ptn` Layout，目的Tensor必须是L1 Buffer中的二维`nz_layout_ptn` Layout，且二者的Shape满足`[B_tile, S, K_tile] -> [B_tile * S, K_tile]`。
- 源、目的Tensor元素类型必须一致，且必须是当前实现支持的`half`或`float`。
- 目的Tensor的行数必须等于`B_tile * S`，K方向大小必须等于`K_tile`。
- 目标NZ Layout必须提供可推导的C0列Stride。
- 当前底层实现只在`current_arch_version == arch_version::v3510`分支调用3510 C API；移植到其他架构时，需要补充对应架构的ND2NZ实现。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;                                                     # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                            # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py                                                  # 生成测试输入数据和真值
  ./demo                                                                          # 执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin         # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` |`dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
