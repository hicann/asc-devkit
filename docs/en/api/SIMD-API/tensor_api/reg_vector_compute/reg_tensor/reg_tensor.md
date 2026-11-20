# reg_tensor

<!-- md-trans-meta sourceCommit=43c68b9f8dfa7b21241d7454e33a40a2cca27cb9 translatedAt=2026-11-19T02:56:25.311Z pushedAt=2026-11-19T05:54:06.675Z nl=2 -->

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `"tensor_api/experimental/arch/vector/reg_tensor.h"`, and the namespace is `asc::te::experimental`.

`reg_tensor<DataType>` is a lightweight wrapper for vector register values. It stores both the vector data register `reg` and the mask register `mask`. When `DataType` is `bool`, both `reg` and `mask` of `reg_tensor<bool>` are mask registers.

## Prototype

```cpp
template <typename DataType>
struct reg_tensor {
    using elem_type = DataType;
    using reg_type = typename detail::type_get<elem_type>::type;

    __simd_callee__ reg_tensor() = default;
    __simd_callee__ reg_tensor(const vector_bool& maskValue);

    __simd_callee__ inline reg_tensor& with_mask(const reg_tensor<bool>& maskReg);

    reg_type reg;
    vector_bool mask;
}
```

## Parameters

| Parameter | Description |
| --- | --- |
| `DataType` | Element type of `reg_tensor`. For supported types, see [Data Type](#data-type). |
| `maskValue` | Mask register value. The parameterized constructor writes it to the `mask` member of the `reg_tensor` object. |

## Member Description

| Member | Description |
| --- | --- |
| `elem_type` | Element type, equivalent to `DataType`. |
| `reg_type` | Native vector register type corresponding to `DataType`. |
| `reg` | Native register data. For `reg_tensor<bool>`, this member holds the predicate register. |
| `mask` | Predicate associated with the data register, used for computation and single-source `asc::te::experimental::store`. |

## Data Type

| `DataType` | Corresponding native register type |
| --- | --- |
| `bool` | `vector_bool` |
| `uint8_t`, `int8_t` | `vector_uint8_t`, `vector_int8_t` |
| `uint16_t`, `int16_t` | `vector_uint16_t`, `vector_int16_t` |
| `uint32_t`, `int32_t` | `vector_uint32_t`, `vector_int32_t` |
| `uint64_t`, `int64_t` | `vector_uint64_t`, `vector_int64_t` |
| `half`, `bfloat16_t`, `float` | `vector_half`, `vector_bfloat16_t`, `vector_float` |
| `hifloat8_t` | `vector_hifloat8_t` |
| `fp8_e4m3fn_t`, `fp8_e5m2_t`, `fp8_e8m0_t` | Corresponding `vector_fp8_*` type |
| `int4x2_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t` | Corresponding `vector_int4x2_t` or `vector_fp4x2_*` type |

The tensor element type is `int4b_t`, and the corresponding register element type is `vector_int4x2_t`. `int4b_t` is not used as the register element type of `reg_tensor`.

## Constraints

- The default constructor does not initialize `reg` and `mask`. They must be initialized through move-in, computation, assignment, or the mask binding API before use.

## Member Functions

### with_mask

Binds the `reg` member of the input `reg_tensor<bool>` register `maskReg` to the `mask` member of the current `reg_tensor<DataType>`, without modifying the data member `reg`. It can be chained with the `load` API.

#### Prototype

```cpp
__simd_callee__ inline reg_tensor& with_mask(
    const reg_tensor<bool>& maskReg)
```

#### Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `maskReg` | Input | A register of the `reg_tensor<bool>` type. Its `reg` member must have been initialized. |

#### Return Value

Returns a reference to the current `reg_tensor`.

#### Constraints

`with_mask` only copies the mask register and does not verify whether the mask register and the data element width are consistent.

#### Example

```cpp
auto value = asc::te::experimental::load(src, asc::te::make_coord(0))
                .with_mask(asc::te::experimental::all_mask<float>());
asc::te::experimental::store(dst, asc::te::make_coord(0), value);
```

## Example

```cpp
__simd_vf__ inline void DeclareRegTensor()
{
    asc::te::experimental::reg_tensor<float> value;
    asc::te::experimental::reg_tensor<bool> predicate;
}
```
