# Batch Matmul样例

## 概述

本样例采用Tensor API编程范式实现带偏置（Bias）的批量矩阵乘法（Batch Matmul）运算。输入矩阵A、矩阵B与偏置Bias的数据类型统一为half，输出矩阵C数据类型同样为half；矩阵A、矩阵B均不做转置处理，核心计算逻辑定义如下：
```text
C[b] = A[b] * B[b] + Bias[b], b = 0, 1, ..., B - 1
```

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| <cann-filter npu-type="950">Ascend 950PR&950DT系列产品</cann-filter> | >= CANN 9.3.0 |

## 目录结构介绍

```text
├── batch_matmul_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                 // 输入数据和真值数据生成脚本
│   │   └── verify_result.py            // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                  // 编译工程文件
│   ├── data_utils.h                    // 数据读入写出函数
│   ├── batch_matmul_tensor_api.asc     // Ascend C样例实现和调用样例
│   └── README.md                       // 样例说明文档
```

## 样例描述

本样例的数据流如下：

1. Global Memory -> L1 Buffer：调用copy接口通过`copy_gm_to_l1` `Operation`搬运能力，从Global Memory将`l1_batch_size`的矩阵A、矩阵B和Bias搬运到L1 Buffer。
2. L1 Buffer -> L0A Buffer/L0B Buffer：调用copy接口通过`copy_l1_to_l0a`和`copy_l1_to_l0b` `Operation`搬运能力，从L1 Buffer将`l0_batch_size`的矩阵A、矩阵B搬运到L0 Buffer。
3. L1 Buffer -> BiasTable Buffer：调用copy接口通过`copy_l1_to_biastable` `Operation`搬运能力，从L1 Buffer将`l0_batch_size`的Bias搬运到BiasTable Buffer。
4. mmad：循环遍历L0 Buffer上的矩阵A、batch和Bias `l0_batch_size`次，每次计算一组batch的A、batch矩阵乘加Bias的结果，并写入L0C Buffer。
5. L0C Buffer -> Global Memory：调用copy接口通过 `copy_l0c_to_gm` `Operation`搬运能力，一次性将`l0_batch_size`个结果矩阵C搬出到Global Memory。

### batch matmul定义

批量矩阵乘法（batch matmul）是普通矩阵乘法在批次维度（Batch Dimension）上的扩展，核心逻辑是：对一个包含多个矩阵的批次数据，逐一对批次内的矩阵执行标准矩阵乘法，最终输出同批次数量的结果矩阵。

在带Bias场景下，若输入矩阵A、矩阵B和Bias的形状分别为`[batch, m, k]`、`[batch, k, n]`和`[batch, 1, n]`，输出矩阵C的形状为`[batch, m, n]`。对任意批次`i`，取`A[i]`和`B[i]`执行普通矩阵乘法，再与`Bias[i]`相加，最终得到`C[i]`。

需要注意的是，不同批次的矩阵之间不会互相计算。

### 样例规格

本样例中输入输出矩阵的规格，如下表1所示。

**表 1**  输入输出的规格

| 输入输出 | 数据类型 | Shape | Format |
|----------|----------|-------|--------|
| 输入矩阵A | half | [128, 32, 32] | ND |
| 输入矩阵B | half | [128, 32, 32] | ND |
| Bias | half | [128, 1, 32] | ND |
| 输出矩阵C | half | [128, 32, 32] | ND |

每次从Global Memory搬入L1 Buffer参与处理的默认batch数量为`l1_batch_size=32 `。
每次从L1 Buffer搬入L0A Buffer、L0B Buffer、L0C Buffer参与计算的batch默认数量为`l0_batch_size=4`。

### Tensor构造

矩阵A、B、C在Global Memory的格式是`ND`，矩阵A、B在L1 Buffer的格式是`NZ`，矩阵A在L0 Buffer的格式是`NZ`，矩阵B在L0B Buffer的格式是`ZN`，矩阵C在L0C Buffer的格式是`NZ`，Bias在Global Memory、L1 Buffer、L0 Buffer的格式都是`ND`。

```cpp
//通过make_frame_layout构造Layout，make_tensor构建Tensor。

auto gm_a = make_tensor(make_mem_ptr(a), make_frame_layout<nd_ext_layout_ptn>(batch, m, k));
auto gm_b = make_tensor(make_mem_ptr(b), make_frame_layout<nd_ext_layout_ptn>(batch, k, n));
auto gm_c = make_tensor(make_mem_ptr(c), make_frame_layout<nd_ext_layout_ptn>(batch, m, n));
auto gm_bias = make_tensor(make_mem_ptr(bias), make_frame_layout<nd_ext_layout_ptn>(batch, 1, n));

auto l1_a_tensor = make_tensor(make_mem_ptr(l1_a_buf), make_frame_layout<nz_layout_ptn, T>(l1_batch_size, m, k));
auto l1_b_tensor = make_tensor(make_mem_ptr(l1_b_buf), make_frame_layout<nz_layout_ptn, T>(l1_batch_size, k, n));
auto l1_bias_tensor = make_tensor(make_mem_ptr(l1_bias_buf), make_frame_layout<nd_ext_layout_ptn, T>(l1_batch_size, 1, n));
auto l0_a_tensor = make_tensor(make_mem_ptr(l0_a_buf), make_frame_layout<nz_layout_ptn, T>(l0_batch_size, m, k));
auto l0_b_tensor = make_tensor(make_mem_ptr(l0_b_buf), make_frame_layout<zn_layout_ptn, T>(l0_batch_size, k, n));
auto l0_c_tensor = make_tensor(make_mem_ptr(l0_c_buf), make_frame_layout<nz_layout_ptn>(l0_batch_size, m, n));
auto l0_bias_tensor = make_tensor(make_mem_ptr(l0_bias_buf), make_frame_layout<nd_ext_layout_ptn>(l0_batch_size, 1, n));
```

### 矩阵批量搬入

样例通过`l1_batch_size`控制每次从Global Memory进入L1 Buffer的batch数量，通过`l0_batch_size`控制每次从L1 Buffer搬入L0A Buffer、L0B Buffer、L0C Buffer的batch数量，从而避免一次搬运全部batch数据造成片上内存超限。

```cpp
for (uint32_t l1_batch_index = b_index_start; l1_batch_index < b_index_end; l1_batch_index += l1_batch_size) {
    uint32_t cur_l1_batch_size = min(l1_batch_size, b_index_end - l1_batch_index);
    copy(copy_gm2l1_atom, l1_a_tensor,
         gm_a.slice(make_coord(l1_batch_index, make_coord(0, 0)), make_shape(cur_l1_batch_size, make_shape(m, k))));
    copy(copy_gm2l1_atom, l1_b_tensor,
         gm_b.slice(make_coord(l1_batch_index, make_coord(0, 0)), make_shape(cur_l1_batch_size, make_shape(k, n))));
    copy(copy_gm2l1_atom, l1_bias_tensor,
         gm_bias.slice(make_coord(l1_batch_index, make_coord(0, 0)), make_shape(cur_l1_batch_size, make_shape(1, n))));

    for (uint32_t l0_batch_index = 0; l0_batch_index < cur_l1_batch_size; l0_batch_index += l0_batch_size) {
        uint32_t cur_l0_batch_size = min(l0_batch_size, cur_l1_batch_size - l0_batch_index);
        copy(copy_l12l0a_atom, l0_a_tensor,
             l1_a_tensor.slice(make_coord(l0_batch_index, make_coord(0, 0)), make_shape(cur_l0_batch_size, make_shape(m, k))));
        copy(copy_l12l0b_atom, l0_b_tensor,
             l1_b_tensor.slice(make_coord(l0_batch_index, make_coord(0, 0)), make_shape(cur_l0_batch_size, make_shape(k, n))));
        copy(copy_l12bt_atom, l0_bias_tensor,
             l1_bias_tensor.slice(make_coord(l0_batch_index, make_coord(0, 0)), make_shape(cur_l0_batch_size, make_shape(1, n))));
    }
}
```

### mmad批量计算

每个Batch的mmad运算对应独立的Bias。由于硬件mad指令不支持Batch数据的矩阵乘加运算，需要在Batch维度循环执行mmad运算，每次将单个输入矩阵A、矩阵B和Bias作为输入操作数执行计算。

```cpp
for (uint32_t l0_c_batch_index = 0; l0_c_batch_index < cur_l0_batch_size; l0_c_batch_index++) {
    mmad(mmad_atom.with(mmad_params{static_cast<uint16_t>(m), static_cast<uint16_t>(n), static_cast<uint16_t>(k), unit_flag_mode::disable, true}),
        l0_c_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(m, n))),
        l0_a_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(m, k))),
        l0_b_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(k, n))),
        l0_bias_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(1, n))));
}
```

### 矩阵批量搬出

每次完成`l0_batch_size`数量的矩阵乘加计算后，调用copy接口通过`copy_l0c_to_gm` `Operation`的数据搬运能力将L0C Buffer上的结果矩阵C搬出到Global Memory。
```cpp
copy(copy_l0c2gm_atom,
     gm_c.slice(make_coord(l1_batch_index + l0_batch_index, make_coord(0, 0)),
               make_shape(cur_l0_batch_size, make_shape(m, n))),
     l0_c_tensor);
```

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
  python3 ../scripts/gen_data.py                                                  # 生成测试输入数据
  ./demo                                                                          # 执行编译生成的可执行程序，执行样例
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
  | `CMAKE_ASC_ARCHITECTURES` |`dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品 |

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
