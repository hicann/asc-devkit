# MmadMx API Boundary Value Summary

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-29T10:42:50.657Z pushedAt=2026-10-29T11:06:34.688Z nl=2 -->

This section summarizes the output results of the MmadMx compute API under boundary value inputs. For data types, see [MmadMx](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md#data-types) under the basic API. The following content applies to the following products:

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products
<!-- end id1 -->

## Boundary Value Description
- The fp4x2_e1m2_t and fp4x2_e2m1_t data types have no +inf/-inf/nan definitions. When +inf/-inf/nan from other data types are converted to these two data types, they are directly converted to the boundary value or 0. The value conversion is shown in the following table:
    
    **Table 1**  Boundary value conversion results for the fp4x2_e1m2_t and fp4x2_e2m1_t data types

    |  | +inf | -inf | nan |
    | --- | --- | --- | --- |
    | fp4x2_e1m2_t | 1.75 | -1.75 | 0.0 |
    | fp4x2_e2m1_t | 6 | -6 | 0.0 |

- The fp8_e4m3fn_t data type has only a nan definition and no +inf/-inf definitions. When +inf/-inf from other data types are converted to the fp8_e4m3fn_t data type, they are converted to nan.
- MmadMx computation should avoid nan input; otherwise, an execution error may occur.

## Boundary Value Summary

### INF/NAN Mode Boundary Value Summary

**Table 2** MmadMx API boundary value summary in INF/NAN mode

| Data Type of Matrix C | MmadMx Sub-stage | Element Value of Matrix A | Element Value of Matrix B | Element Value of dst |
| --- | --- | --- | --- | --- |
| float | Matrix multiplication stage | +inf | norm(> 0.0)/+inf | +inf |
| float | Matrix multiplication stage | +inf | norm(< 0.0)/-inf | -inf |
| float | Matrix multiplication stage | -inf | norm(> 0.0)/+inf | -inf |
| float | Matrix multiplication stage | -inf | norm(< 0.0)/-inf | +inf |
| float | Matrix multiplication stage | +inf/-inf | 0.0 | nan |
| float | Matrix multiplication stage | norm(> 0.0) | +inf | +inf |
| float | Matrix multiplication stage | norm(> 0.0) | -inf | -inf |
| float | Matrix multiplication stage | norm(< 0.0) | +inf | -inf |
| float | Matrix multiplication stage | norm(< 0.0) | -inf | +inf |
| float | Matrix multiplication stage | 0.0 | +inf/-inf | nan |
| float | Matrix multiplication stage | nan | arbitrary input | nan |
| float | Matrix multiplication stage | arbitrary input | nan | nan |
| float | Matrix multiplication stage | arbitrary input | arbitrary input | In case of result overflow:<br>positive overflow yields +inf,<br>negative overflow yields -inf |
| float | Addition stage | +inf | -inf | -inf |
| float | Addition stage | +inf | +inf/norm | +inf |
| float | Addition stage | -inf | +inf | +inf |
| float | Addition stage | -inf | -inf/norm | -inf |
| float | Addition stage | nan | arbitrary input | nan |
| float | Addition stage | arbitrary input | nan | nan |

### Saturation Mode Boundary Value Summary

- Before quantization, boundary values undergo a saturation operation, after which the MmadMx computation is performed on the numeric values.
    - nan is converted to 0.0.
    - Among the four input types, only the fp8_e5m2_t data type defines +inf/-inf, which are converted to 57344.0/-57344.0.
- If the MmadMx computation result overflows, it is saturated to MAX (0x7F7FFFFF) or MIN (0xFF7FFFFF) depending on its sign.
- Before the addition stage in which the matrix multiplication result is added to the bias matrix, nan in the bias matrix is converted to 0.
