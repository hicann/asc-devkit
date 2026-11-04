# Cube Instruction Theoretical Performance Summary<a id="ZH-CN_TOPIC_0000002607728945"></a>

<!-- npu="950,A3,910b" id1 -->
<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-29T10:34:53.956Z pushedAt=2026-10-29T10:53:21.396Z nl=2 -->

This section summarizes the theoretical performance of the main Cube instructions. The following content applies to the following models:

<!-- npu="950" id2 -->
Ascend 950PR & 950DT products
<!-- end id2 -->
<!-- npu="A3" id3 -->
Atlas A3 products
<!-- end id3 -->
<!-- npu="910b" id4 -->
Atlas A2 products
<!-- end id4 -->

## Theoretical Performance Summary of Mmad Computation Instructions

The following are the theoretical performance calculation formulas for Mmad computation instructions. For different shape sizes, the unitFlag switch has almost no impact on performance.

$$
\begin{gathered}
{ceil_m} = \left\lceil \frac{m}{16} \right\rceil \times 16 \\[12pt]
{ceil_n} = \left\lceil \frac{n}{16} \right\rceil \times 16 \\[12pt]
{ceil_k} = \left\lceil \frac{k}{k_0} \right\rceil \times k_0 \\[16pt]
\text{cube utilization} =
\frac{ (m \times n \times k) / ({cube_m} \times {cube_n} \times {cube_k}) }
{ \Delta t + ({ceil_m} \times {ceil_n} \times {ceil_k}) / ({cube_m} \times {cube_n} \times {cube_k}) }
\end{gathered}
$$

Key variables and constant description:

- $m, n, k$: actual computation sizes of the mmad input parameters.
- $ceil_m, ceil_n, ceil_k$: values of $m, n, k$ after being aligned upward according to the tiling size.
- $cube_m, cube_n, cube_k$: actual hardware parallelism.
- $k_0$: minimum tiling size in the K direction on the L0 Buffer.
- $\Delta t$: header overhead in cycles.

<!-- npu="A3,910b" id5 -->
**Table 1** Values of parallelism and k0 in the theoretical performance calculation formulas for Mmad computation instructions ([NPU architecture version 2201](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123815211"></a>  

| API | Left Matrix A | Right Matrix B | cube<sub>m</sub> | cube<sub>n</sub> | cube<sub>k</sub> | k<sub>0</sub> |
| --- | --- | --- | --- | --- | --- | --- |
| [Mmad](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) | int4b_t | int4b_t | 16 | 16 | 64 | 64 |
| Mmad | int8_t | int8_t | 16 | 16 | 32 | 32 |
| Mmad | half | half | 16 | 16 | 16 | 16 |
| Mmad | bfloat16_t | bfloat16_t | 16 | 16 | 16 | 16 |
| Mmad | float | float | 16 | 16 | 4 | 8 |
| Mmad (with [HF32](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_key_features/HF32.md) enabled) | float | float | 16 | 16 | 8 | 8 |
| [MmadWithSparse](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md) | int8_t | int8_t | 16 | 16 | 32 | 32 |

<!-- end id5 -->

<!-- npu="950" id6 -->
**Table 2** Values of parallelism and k0 in the theoretical performance calculation formula for Mmad computation instructions ([NPU architecture version 3510](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123815985"></a>  

| API | Left Matrix A | Right Matrix B | cube<sub>m</sub> | cube<sub>n</sub> | cube<sub>k</sub> | k<sub>0</sub> |
| --- | --- | --- | --- | --- | --- | --- |
| [Mmad](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) | int8_t | int8_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e4m3fn_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e4m3fn_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e5m2_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e5m2_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |
| Mmad | hifloat8_t | hifloat8_t | 16 | 16 | 32 | 32 |
| Mmad | half | half | 16 | 16 | 16 | 16 |
| Mmad | bfloat16_t | bfloat16_t | 16 | 16 | 16 | 16 |
| Mmad | float | float | 16 | 16 | 1 | 8 |
| Mmad (with [HF32](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_key_features/HF32.md) enabled) | float | float | 16 | 16 | 8 | 8 |
| [MmadMx](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md) | fp4x2_e1m2_t | fp4x2_e1m2_t | 16 | 16 | 64 | 64 |
| MmadMx | fp4x2_e2m1_t | fp4x2_e1m2_t | 16 | 16 | 64 | 64 |
| MmadMx | fp4x2_e1m2_t | fp4x2_e2m1_t | 16 | 16 | 64 | 64 |
| MmadMx | fp4x2_e2m1_t | fp4x2_e2m1_t | 16 | 16 | 64 | 64 |
| MmadMx | fp8_e4m3fn_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| MmadMx | fp8_e4m3fn_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |
| MmadMx | fp8_e5m2_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| MmadMx | fp8_e5m2_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |

<!-- end id6 -->

## Theoretical Performance Summary of Cube Computation Copy-in Instructions

The following content provides a theoretical performance summary of cube computation copy-in instructions across multiple data paths: L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer, L1 Buffer->L0A_MX Buffer, L1 Buffer->L0B_MX Buffer, L1 Buffer->BiasTable Buffer, and L1 Buffer->Fixpipe Buffer.

The theoretical calculation formula for the number of cycles occupied by a data movement instruction:

$$
{cycles} = {data\_size} / {bandwidth} + \Delta T
$$

Key variables and constant description:

- $data\_size$: Total data volume moved, in Byte.
- $bandwidth$: Copy bandwidth, the amount of data that can be moved per cycle, in Byte/cycle.
- $\Delta T$: External overhead such as bandwidth latency during the copy process. The reference value varies by copy instruction.

Note that the theoretical performance shown in the following tables is the copy bandwidth $bandwidth$, without considering the external overhead $\Delta T$.

<!-- npu="A3,910b" id7 -->
**Table 3** Theoretical performance description of cube computation copy-in instructions ([NPU architecture version 2201](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123827121"></a>  

| API | Copy bandwidth (unit: Byte/cycle) |
| --- | --- |
| [LoadData (2D matrix copy)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D.md) (L1 Buffer->L0A Buffer) | 256 |
| LoadData (2D matrix copy) (L1 Buffer->L0B Buffer) | 128 |
| [LoadDataWithTranspose](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md) (L1 Buffer->L0A Buffer) | 256 |
| LoadDataWithTranspose (L1 Buffer->L0B Buffer) | 128 |
| [LoadDataWithSparse](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithSparse.md) (L1 Buffer->L0B Buffer) | 128 |
| [DataCopy (L1 Buffer->BiasTable Buffer)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_L1ToBiasTable.md) | 32 |
| [DataCopy (L1 Buffer->Fixpipe Buffer)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_L1ToFixpipe.md) | 32 |

The theoretical performance of the [LoadData (convolution data copy)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_3D.md) instruction depends on its parameters. For parameter descriptions, see [Parameter descriptions in the LoadData3DParamsV2 structure](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419). The theoretical performance is described in the following table:

**Table 4** Theoretical performance of the LoadData (convolution data copy) instruction ([NPU architecture version 2201](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123821113"></a>  

| strideW value | Number of rows n by which the convolution kernel slides in the H direction | L1 Buffer->L0A Buffer bandwidth (unit: Byte/cycle) | L1 Buffer->L0B Buffer bandwidth (unit: Byte/cycle) |
| --- | --- | --- | --- |
| 1 | 1 | 256 | 128 |
| 1 | Greater than 1 | $\left[\frac{512}{n+2}\right]$ (minimum not lower than 32) | $\left[\frac{512}{n+4}\right]$ (minimum not lower than $\left[\frac{512}{16+2}\right]$) |
| 2, 4, 8 | 1 | $\left[\frac{512}{(strideW \times 2)}\right]$ (minimum not lower than 32) | $\left[\frac{512}{(strideW \times 2 + 2)}\right]$ (minimum not lower than $\left[\frac{512}{16+2}\right]$) |
| 2, 4, 8 | Greater than 1 | $\left[\frac{512}{(strideW \times 2 + n)}\right]$ (minimum not lower than 32) | $\left[\frac{512}{(strideW \times 2 + 2 + n)}\right]$ (minimum not lower than $\left[\frac{512}{16+2}\right]$) |
| Greater than 1, and not equal to 2, 4, or 8 | any value | 32 | $\left[\frac{512}{16+2}\right]$ |

<!-- end id7 -->

<!-- npu="950" id8 -->
**Table 5** Theoretical performance of cube computation copy-in instructions ([NPU architecture version 3510](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123827112"></a>  

| API | Copy bandwidth (unit: Byte/cycle) |
| --- | --- |
| [LoadData (2D matrix copy V2)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md) (L1 Buffer->L0A Buffer) | 256 |
| LoadData (2D matrix copy V2) (L1 Buffer->L0B Buffer) | 256 |
| [LoadData (MX matrix copy)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md) (L1 Buffer->L0A Buffer) | 256 |
| LoadData (MX matrix copy) (L1 Buffer->L0B Buffer) | 256 |
| LoadData (MX matrix copy) (L1 Buffer->L0A_MX Buffer) | 32 |
| LoadData (MX matrix copy) (L1 Buffer->L0B_MX Buffer) | 32 |
| [LoadDataWithTranspose](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md) (L1 Buffer->L0A Buffer) | 256 |
| LoadDataWithTranspose (L1 Buffer->L0B Buffer) | 128 |
| [DataCopy (L1 Buffer->BiasTable Buffer)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_L1ToBiasTable.md) | 32 |
| [DataCopy (L1 Buffer->Fixpipe Buffer)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_L1ToFixpipe.md) | 32 |

For the Ascend 950PR&950DT products, the LoadData (2D matrix copy) API is provided only for compatibility. Internally, it uses the LoadData (2D matrix copy V2) API. For its theoretical performance, refer to the LoadData (2D matrix copy V2) API. Note, however, that this compatibility implementation incurs a performance loss.

Because the internal implementation of the LoadData (MX matrix copy) API involves data movement over two data paths, the formula for calculating the number of cycles it occupies differs. The specific formula is as follows:

$$
{cycles} = {data\_size} / {bandwidth} + {scale\_data\_size} / {bandwidth\_mx} + \Delta T
$$

Key variable and constant description:

- $data\_size$: Total data volume of the left/right matrix copy, in bytes.
- $scale\_data\_size$: Total data volume of the scale matrix copy, in bytes.
- $bandwidth$: Bandwidth for copying the left/right matrix, in Byte/cycle.
- $bandwidth\_mx$: Bandwidth for copying the scale matrix, in Byte/cycle.
- $\Delta T$: External overhead such as bandwidth latency during the copy process.

For the Ascend 950PR&950DT products, the theoretical performance of the [LoadData (convolution data copy)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_3D.md) and [LoadDataWithStride](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md) instructions depends on their parameters. For parameter descriptions, see [LoadData3DParamsV2 structure parameter description](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419). In addition, compared with the LoadData (convolution data copy) API, the LoadDataWithStride API does not internally include compatibility implementations for other chip versions, which reduces the external overhead caused by compatibility and improves performance. The theoretical performance of the two APIs is described in the following table:

**Table 6** Theoretical performance of LoadData (convolution data copy) and LoadDataWithStride ([NPU architecture version 3510](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877123821313"></a>  

| strideW value | Number of rows n that the convolution kernel slides in the H direction | L1 Buffer->L0A Buffer bandwidth (unit: Byte/cycle) | L1 Buffer->L0B Buffer bandwidth (unit: Byte/cycle) |
| --- | --- | --- | --- |
| 1 | 1 | 256 | 256 |
| 1 | Greater than 1 | $\left[\frac{512}{n+2}\right]$ (minimum not lower than 32) | $\left[\frac{512}{n+2}\right]$ (minimum not lower than 32) |
| 2, 4, 8 | 1 | $\left[\frac{512}{(strideW \times 2)}\right]$ (minimum not lower than 32) | $\left[\frac{512}{(strideW \times 2)}\right]$ (minimum not lower than 32) |
| 2, 4, 8 | Greater than 1 | $\left[\frac{512}{(strideW \times 2 + n)}\right]$ (minimum not lower than 32) | $\left[\frac{512}{(strideW \times 2 + n)}\right]$ (minimum not lower than 32) |
| Greater than 1, and not equal to 2, 4, 8 | any value | 32 | 32 |

<!-- end id8 -->

## Theoretical Performance Summary of Cube Computation Copy-Out Instructions

The following content summarizes the theoretical performance of cube computation copy-out instructions over the two data movement paths: L0C Buffer->L1 Buffer and L0C Buffer->UB.

The theoretical calculation formula for the number of cycles occupied by a data movement instruction:

$$
{cycles} = {data\_size} / {bandwidth} + \Delta T
$$

Key variables and constant description:

- $data\_size$: total data volume to be copied, in Byte.
- $bandwidth$: copy bandwidth, the amount of data that can be copied per cycle, in Byte/cycle.
- $\Delta T$: external overhead such as bandwidth latency during the copy process. The reference value varies with the copy instruction.

Note that the theoretical performance shown in the following tables is the copy bandwidth $bandwidth$, without considering the external overhead $\Delta T$.

For details about cube computation copy-out instructions and their accompanying functions, see [DataCopy (L0C-to-GM data movement)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L0CToGM.md), [Fixpipe (L0C-to-GM data movement)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md), and the corresponding materials for other paths in the same directory. The following is a list of theoretical performance descriptions:

<!-- npu="A3,910b" id9 -->
**Table 7** Theoretical performance description of cube copy-out instructions ([NPU architecture version 2201](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877112315211"></a>  

| Data Path | Data Type in L0C Buffer | Quantization Mode | Format Conversion Mode | Data Type in Destination Memory | Parallelism (unit: elements/cycle) | Copy Bandwidth (unit: Byte/cycle) |
| --- | --- | --- | --- | --- | --- | --- |
| L0C Buffer->L1 Buffer | float | F322F16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16 | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |

<!-- end id9 -->

<!-- npu="950" id10 -->
**Table 8** Theoretical performance description of cube copy-out instructions ([NPU architecture version 3510](../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="table1877112315211"></a>  

| Data Path | Data Type in L0C Buffer | Quantization Mode | Format Conversion Mode | Data Type in Destination Memory | Parallelism (unit: elements/cycle) | Copy Bandwidth (unit: Byte/cycle) |
| --- | --- | --- | --- | --- | --- | --- |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2NZ | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2NZ + Channel Split | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2ND | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride = 1 | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride > 1 | float | 8 | 32 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2ND | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2NZ + Channel Merge | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2ND | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride = 1 | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride > 1 | fp8_e4m3fn_t | 16 | 16 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2NZ + Channel Merge | hifloat8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2ND | hifloat8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride = 1 | hifloat8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride > 1 | hifloat8_t | 16 | 16 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2ND | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |
| L0C Buffer->UB (with [dual-target mode](../SIMD-API/basic_api/cube_compute_ISASI/cube_store_key_features/L0C_to_UB_dual_target_mode.md) enabled) | float | NoQuant | NZ2NZ | float | 32 + 32 | 128 + 128 |
| L0C Buffer->UB (with dual-target mode enabled) | float | NoQuant | NZ2ND | float | 32 + 32 | 128 + 128 |
| L0C Buffer->UB | float | NoQuant/(V)QF322F32_PRE | NZ2NZ | float | 32 | 128 |
| L0C Buffer->UB | float | NoQuant/(V)QF322F32_PRE | NZ2NZ + Channel Split | float | 32 | 128 |
| L0C Buffer->UB | float | NoQuant/(V)QF322F32_PRE | NZ2ND | float | 32 | 128 |
| L0C Buffer->UB | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride = 1 | float | 32 | 128 |
| L0C Buffer->UB | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride > 1 | float | 8 | 32 |
| L0C Buffer->UB | float | F322F16/(V)QF322F16_PRE | NZ2NZ | half | 64 | 128 |
| L0C Buffer->UB | float | F322F16/(V)QF322F16_PRE | NZ2ND | half | 64 | 128 |
| L0C Buffer->UB | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->UB | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->UB | float | F322BF16/(V)QF322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->UB | float | F322BF16/(V)QF322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->UB | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->UB | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->UB | float | (V)QF322FP8_PRE | NZ2NZ + Channel Merge | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322FP8_PRE | NZ2ND | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride = 1 | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride > 1 | fp8_e4m3fn_t | 16 | 16 |
| L0C Buffer->UB | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2NZ + Channel Merge | hifloat8_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2ND | hifloat8_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride = 1 | hifloat8_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride > 1 | hifloat8_t | 16 | 16 |
| L0C Buffer->UB | float | (V)QF322B8_PRE | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322B8_PRE | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->UB | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |
| L0C Buffer->UB | int32_t | (V)DEQF16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->UB | int32_t | (V)DEQF16 | NZ2ND | half | 64 | 128 |
| L0C Buffer->UB | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->UB | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->UB | int32_t | (V)QS322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->UB | int32_t | (V)QS322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->UB | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->UB | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->UB | int32_t | (V)REQ8 | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->UB | int32_t | (V)REQ8 | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->UB | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->UB | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |

Note: srcNzC0Stride is one of the parameters that must be configured to enable the NZ2DN feature. For details about this parameter, see [NZ2DN Key Feature Description](../SIMD-API/basic_api/cube_compute_ISASI/cube_store_key_features/NZ2DN.md).
<!-- end id10 -->
<!-- end id1 -->
