# Optimizing Matmul Performance with hifloat8_t

<!-- md-trans-meta sourceCommit=d50cf60b6fe47039c8a7d975803a9ba84c853c77 translatedAt=2026-11-09T10:34:38.357Z pushedAt=2026-11-19T06:32:19.766Z nl=2 -->

[Priority] High

[Description] hifloat8_t is a low-precision floating-point data type that occupies 1 byte and is mainly used in cube computation scenarios. When the A and B cubes of Matmul use hifloat8_t, the storage space and data transfer volume of the input cubes can be reduced compared with using half. When the C cube uses hifloat8_t, real-time quantization can be completed through Fixpipe during the process of moving the cube computation result from the L0C Buffer to Global Memory, avoiding additional independent quantization computation and intermediate result read/write.

This document uses `C = A * B` as an example to describe how to build a low-precision Matmul data path through hifloat8_t input and Fixpipe real-time quantization, and explains the configuration of the accompanying sample.

>[!NOTE] Note
>This performance optimization method applies to the following product models:
><!-- npu="950" id1 -->
>- Ascend 950PR&950DT products
><!-- end id1 -->

## Optimization Principle

### Storing Input Cubes Using hifloat8_t

The Matmul advanced API supports using hifloat8_t for both cube A and cube B, and the data types of the two must be consistent. When cubes A and B are hifloat8_t, the data type of the cube multiplication result is float, and the result is stored in the L0C Buffer. When cube C is hifloat8_t, the float result in the L0C Buffer is quantized and moved out through Fixpipe.

For cubes A, B, and C with shapes `[M, K]`, `[K, N]`, and `[M, N]` respectively, when cache reuse is not considered, the theoretical Global Memory data volume required by each scheme to process one group of cubes is as follows.

| Scheme | A/B Input Type | C Output Type | Theoretical Data Volume (Bytes) |
| --- | --- | --- | --- |
| Baseline | half | float | $2MK + 2KN + 4MN$ |
| Input quantization only | hifloat8_t | float | $MK + KN + 4MN$ |
| Input and output quantization | hifloat8_t | hifloat8_t | $MK + KN + MN$ |

Changing only cubes A and B from half to hifloat8_t halves the data volume of the input cubes. After cube C also uses hifloat8_t, the data volume of the output cube is reduced from 4 bytes per element to 1 byte per element.

For the data format, value range, and conversion rules of hifloat8_t, see [Built-in Data Types](../../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md).

### Using Fixpipe to Complete Output Quantization

When the float results in the L0C Buffer need to be output as hifloat8_t, Matmul can complete the `float -> hifloat8_t` conversion through Fixpipe while the results are being moved out. This approach merges type conversion with the data movement from the L0C Buffer to Global Memory, eliminating the need to first move the cube multiplication results to Global Memory and then move them to UB for quantization computation.

When the same quantization coefficient is used, the host side configures `DequantType::SCALAR` through [SetDequantType](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetDequantType.md), and the kernel side calls [SetQuantScalar](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantScalar.md) before `Iterate` or `IterateAll`. The output uses the `QF322HIF8_PRE` quantization mode and the **Half to Away Round** method for quantization. The quantization coefficient cannot be Inf, NaN, or a denormalized number.

[Counter example] First writing the float results back to Global Memory and then moving them to UB for quantization computation adds one float cube write, one float cube read, and one hifloat8_t cube write.

[Positive example] Directly configuring the C cube type of Matmul as hifloat8_t and using Fixpipe real-time quantization output shortens the data path and reduces output bandwidth pressure.

## Implementation

### Configuring Input and Output Types

In the tiling phase, configure the data type of cubes A and B to `DT_HIFLOAT8`. When cube C also requires quantized output, configure the data type of cube C to `DT_HIFLOAT8` and configure the scalar quantization mode.

```cpp
tiling.SetAType(TPosition::GM, CubeFormat::ND, DataType::DT_HIFLOAT8, false);
tiling.SetBType(TPosition::GM, CubeFormat::ND, DataType::DT_HIFLOAT8, false);
tiling.SetCType(TPosition::GM, CubeFormat::ND, DataType::DT_HIFLOAT8);
tiling.SetDequantType(DequantType::SCALAR);
```

When cubes A and B are hifloat8_t and cube C is float, configure the data type of cube C to `DT_FLOAT`, without configuring Fixpipe real-time quantization.

### Creating a Matmul Object

When creating a Matmul object on the kernel side, configure the data types of cubes A, B, and C through `MatmulType`, and keep them consistent with the tiling configuration. The following code omits common implementations such as cube splitting and address offset.

```cpp
AscendC::Matmul<
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, hifloat8_t>,
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, hifloat8_t>,
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, hifloat8_t>,
    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>, CFG_MDL>
    matmulObj;
```

### Setting the Real-Time Quantization Coefficient

Call `SetQuantScalar` to set the quantization coefficient shared by the C cubes. In this example, the quantization coefficient is 1.0.

```cpp
float quantScale = 1.0f;
uint64_t quantScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantScale));
matmulObj.SetQuantScalar(quantScalar);
matmulObj.SetTensorA(aGlobal, false);
matmulObj.SetTensorB(bGlobal, false);
matmulObj.IterateAll(cGlobal);
matmulObj.End();
```

`SetQuantScalar` must be called before `Iterate` or `IterateAll`, and must be consistent with `DequantType::SCALAR` in the tiling phase. For details about the mode and quantization parameters of Fixpipe real-time quantization, see [real-time quantization](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md).

## Sample Configuration

The cube shape of the accompanying sample is `M = N = K = 1024`, and cubes A, B, and C are all in ND format without transposition. The sample provides the following three configurations.

| Scenario | A/B Input Type | C Output Type | Description |
| --- | --- | --- | --- |
| Case 0 | half | float | half input and float output |
| Case 1 | hifloat8_t | float | hifloat8_t input and float output |
| Case 2 | hifloat8_t | hifloat8_t | hifloat8_t input and Fixpipe real-time quantization output |

Case 2 uses the scalar quantization mode, with a quantization coefficient of 1.0.

For the complete sample code, see the [matmul_hif8_high_performance sample](../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_hif8_high_performance/README.md).
