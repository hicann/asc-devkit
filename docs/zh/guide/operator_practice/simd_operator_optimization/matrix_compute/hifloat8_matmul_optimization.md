# 使用hifloat8_t实现Matmul性能优化

【优先级】高

【描述】hifloat8_t是占用1字节的低精度浮点数据类型，主要用于矩阵计算场景。Matmul的A矩阵和B矩阵使用hifloat8_t后，相比使用half可减少输入矩阵的存储空间和搬运数据量；C矩阵使用hifloat8_t时，可在矩阵计算结果从L0C Buffer搬出到Global Memory的过程中，通过Fixpipe完成随路量化，避免增加独立的量化计算和中间结果读写。

本文以`C = A * B`为例，介绍如何通过hifloat8_t输入和Fixpipe随路量化构建低精度Matmul数据通路，并说明配套样例的配置。

>[!NOTE]说明
>该性能优化手段适用于如下产品型号：
><!-- npu="950" id1 -->
>- Ascend 950PR&950DT系列产品
><!-- end id1 -->

## 优化原理

### 使用hifloat8_t存储输入矩阵

Matmul高阶API支持A矩阵和B矩阵使用hifloat8_t，且两者的数据类型需要保持一致。A、B矩阵为hifloat8_t时，矩阵乘结果的数据类型为float，结果保存在L0C Buffer中；C矩阵为hifloat8_t时，通过Fixpipe将L0C Buffer中的float结果量化后搬出。

对于shape分别为`[M, K]`、`[K, N]`和`[M, N]`的A、B、C矩阵，不考虑Cache复用时，各方案处理一组矩阵所需的理论Global Memory数据量如下。

| 方案 | A/B输入类型 | C输出类型 | 理论数据量（字节） |
| --- | --- | --- | --- |
| 基线 | half | float | $2MK + 2KN + 4MN$ |
| 仅输入量化 | hifloat8_t | float | $MK + KN + 4MN$ |
| 输入输出量化 | hifloat8_t | hifloat8_t | $MK + KN + MN$ |

仅将A、B矩阵从half改为hifloat8_t，可使输入矩阵的数据量减半；C矩阵也使用hifloat8_t后，输出矩阵的数据量由每元素4字节降为每元素1字节。

hifloat8_t的数据格式、取值范围和转换规则请参见[内置数据类型](../../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md)。

### 使用Fixpipe完成输出量化

当L0C Buffer中的float结果需要输出为hifloat8_t时，Matmul可通过Fixpipe在结果搬出过程中完成`float -> hifloat8_t`转换。该方式将类型转换与L0C Buffer到Global Memory的数据搬运合并，无需将矩阵乘结果先搬运到Global Memory，再搬运到UB进行量化计算。

使用同一量化系数时，Host侧通过[SetDequantType](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetDequantType.md)配置`DequantType::SCALAR`，Kernel侧在`Iterate`或`IterateAll`前调用[SetQuantScalar](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantScalar.md)。输出采用`QF322HIF8_PRE`量化模式，采用Half to Away Round方式量化。量化系数不能为Inf、NaN或非规格化数。

【反例】先将float结果写回Global Memory，再搬运到UB进行量化计算，会增加一次float矩阵写入、一次float矩阵读取和一次hifloat8_t矩阵写入。

【正例】将Matmul的C矩阵类型直接配置为hifloat8_t，使用Fixpipe随路量化输出，缩短数据通路并降低输出带宽压力。

## 实现方法

### 配置输入和输出类型

在Tiling阶段，将A、B矩阵的数据类型配置为`DT_HIFLOAT8`。当C矩阵也需要量化输出时，将C矩阵的数据类型配置为`DT_HIFLOAT8`，并配置Scalar量化模式。

```cpp
tiling.SetAType(TPosition::GM, CubeFormat::ND, DataType::DT_HIFLOAT8, false);
tiling.SetBType(TPosition::GM, CubeFormat::ND, DataType::DT_HIFLOAT8, false);
tiling.SetCType(TPosition::GM, CubeFormat::ND, DataType::DT_HIFLOAT8);
tiling.SetDequantType(DequantType::SCALAR);
```

当A、B矩阵为hifloat8_t、C矩阵为float时，将C矩阵的数据类型配置为`DT_FLOAT`，无需配置Fixpipe随路量化。

### 创建Matmul对象

Kernel侧创建Matmul对象时，通过`MatmulType`配置A、B、C矩阵的数据类型，并与Tiling配置保持一致。以下代码省略了矩阵切分和地址偏移等通用实现。

```cpp
AscendC::Matmul<
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, hifloat8_t>,
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, hifloat8_t>,
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, hifloat8_t>,
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>, CFG_MDL>
    matmulObj;
```

### 设置随路量化系数

调用`SetQuantScalar`设置C矩阵共用的量化系数，本示例中量化系数为1.0。

```cpp
float quantScale = 1.0f;
uint64_t quantScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantScale));
matmulObj.SetQuantScalar(quantScalar);
matmulObj.SetTensorA(aGlobal, false);
matmulObj.SetTensorB(bGlobal, false);
matmulObj.IterateAll(cGlobal);
matmulObj.End();
```

`SetQuantScalar`必须在`Iterate`或`IterateAll`前调用，并与Tiling阶段的`DequantType::SCALAR`保持一致。Fixpipe随路量化的模式和量化参数说明请参见[随路量化](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md)。

## 样例配置

配套样例的矩阵shape为`M = N = K = 1024`，A、B、C矩阵均为ND格式且不转置。样例提供以下三种配置。

| 场景 | A/B输入类型 | C输出类型 | 配置说明 |
| --- | --- | --- | --- |
| Case 0 | half | float | half输入和float输出 |
| Case 1 | hifloat8_t | float | hifloat8_t输入和float输出 |
| Case 2 | hifloat8_t | hifloat8_t | hifloat8_t输入和Fixpipe随路量化输出 |

Case 2使用Scalar量化模式，量化系数为1.0。

完整的样例代码请参考[matmul_hif8_high_performance样例](../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_hif8_high_performance/README.md)。
