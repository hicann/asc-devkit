# Reg Vector Computation Programming<a name="ZH-CN_TOPIC_0000002509863892"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-20T11:17:54.873Z pushedAt=2026-08-26T03:06:40.100Z -->

This document describes how to use the basic API (C++ template API) to write Reg vector computation code. The basic API and [Reg Vector Computation Programming (Language Extension C API)](../c_pointer_programming/reg_vector_compute_programming.md) describe the same set of Reg vector hardware capabilities with consistent functional semantics. The main differences lie in API naming, type expressions, and the way configurable parameters are expressed.

For content unrelated to API naming, such as the hardware principles of Reg vector programming, memory hierarchy, VF function execution domain, pipeline synchronization, hardware loop, and VF fusion, see [Reg Vector Computation Programming (Language Extension C API)](../c_pointer_programming/reg_vector_compute_programming.md). This document only describes the interface positioning, template parameters, data type system, API classification, and typical calling methods of the basic API.

## Basic API Positioning<a name="section_part1"></a>

The basic Reg vector computation APIs are located in the `AscendC::Reg` namespace and are C++ template-based wrappers of the language extension C APIs. The basic API does not change the Reg vector computation programming model: you still need to enter the `__simd_vf__` function through `asc_vf_call` in the `__aicore__` domain, and complete `Load` -> `Compute` -> `Store` within the VF function.

The main features of the basic API are as follows:

- Use C++ types to express register objects, such as `RegTensor<T>`, `MaskReg`, `AddrReg`, `UnalignRegForLoad`, and `UnalignRegForStore`. For the mapping, see [Data Objects and Register Types](#section_part4).

- Use template parameters to express configurations such as data type, movement mode, and PostUpdate mode.

- Most APIs can automatically deduce the register type from parameters.

- The movement APIs can connect to `LocalTensor`. They can support obtaining the UB physical address from `LocalTensor::GetPhyAddr()` through the outer `__aicore__` code without operating on the UB address, and then pass it to the Reg movement API in the VF function.

- Some computation APIs provide the `MaskMergeMode` template parameter to control how elements not selected by the mask are written to the destination register.

> 📌 **Note**:
>
> 1. The basic APIs and C APIs provide the same functionality. If you need to understand model-level content such as "why Reg vector programming is needed", "the relationship between registers and UB/GM", and "which scenarios require synchronization", read the C API version of the document directly.
>
> 2. The movement APIs support the `LocalTensor` parameter, which will be provided in a later version and is not yet supported.

## Correspondence with C APIs<a name="section_part2"></a>

The core difference between the basic APIs and C APIs lies in their expression forms.

| Dimension | Basic API (C++ Template) | C API (Language Extension) |
| --- | --- | --- |
| Namespace/prefix | `AscendC::Reg::*` | `asc_*` |
| Naming style | `LoadAlign`, `StoreUnAlignPost` | `asc_loadalign`, `asc_storeunalign_post` |
| Vector data register | `RegTensor<T>` | `vector_float`, `vector_half`, `vector_int32_t`, etc. |
| Mask register | `MaskReg` | `vector_bool` |
| Address register | `AddrReg` | `iter_reg` |
| Unaligned copy-in/copy-out register | `UnalignRegForLoad` / `UnalignRegForStore` | `vector_load_unalign` / `vector_store_unalign` |
| Data type selection | Template parameter `T`, usually deducible from parameters | Type name or API suffix, such as `_b8`, `_b16`, `_b32` |
| PostUpdate | `PostLiteral` template parameter | API name suffix, such as `_postupdate` |
| Mask merge mode | `MaskMergeMode` template parameter | Not supported yet |

Common API mappings are as follows:

| Function | Basic API | C API |
| --- | --- | --- |
| Create a full mask | `CreateMask<T, MaskPattern::ALL>()` | `asc_create_mask_b*()` |
| Update the tail block mask | `UpdateMask<T>(count)` | `asc_update_mask_b*()` |
| Create an address register | `CreateAddrReg<T>(...)` | `asc_create_iter_reg_b*()` |
| General copy-in/copy-out | `Load` / `Store` | `asc_load` / `asc_store` |
| Aligned copy-in/copy-out | `LoadAlign` / `StoreAlign` | `asc_loadalign` / `asc_storealign` |
| Unaligned copy-in | `LoadUnAlignPre` + `LoadUnAlign` | `asc_loadunalign_pre` + `asc_loadunalign` |
| Unaligned copy-out | `StoreUnAlign` + `StoreUnAlignPost` | `asc_storeunalign` + `asc_storeunalign_post` |
| Pipeline synchronization | `LocalMemBar<src, dst>()` | `asc_mem_bar(...)` |

## Template Parameter System<a name="section_part3"></a>

The basic APIs consolidate the capabilities expressed by "suffixes, enums, and special API names" in the C APIs into template parameters. Common template parameters are as follows.

| Template Parameter | Purpose | Typical Value/Type | Common API |
| --- | --- | --- | --- |
| `T` | Data type | `float`, `half`, `int32_t`, etc. | `RegTensor<T>`, `LoadAlign<T>`, `Add<T>` |
| `PostLiteral` | Whether to update the UB address after movement | `POST_MODE_NORMAL`, `POST_MODE_UPDATE` | `LoadAlign`, `StoreAlign`, `LoadUnAlign`, `StoreUnAlign` |
| `LoadDist` | Copy-in mode | `DIST_NORM`, broadcast-type values, etc. | `LoadAlign` |
| `StoreDist` | Copy-out mode | Normal copy-out, first-element copy-out, pack-type values, etc. | `StoreAlign` |
| `MaskMergeMode` | Handling of elements not selected by the mask | `ZEROING`, `MERGING` | Some computation APIs, for example, Add |
| Computation mode enum | Specifies the algorithm or computation variant | For example, `ReduceType`, `PairReduce`, etc. | Reduction, compound computation, etc. |

### Data Type Template Parameter

The basic APIs use `RegTensor<T>` to express a vector data register. `T` determines how the elements in the register are interpreted, while the register width is still determined by the hardware VL. For example, when VL is 256B, `RegTensor<float>` can represent 64 `float` elements, and `RegTensor<half>` can represent 128 `half` elements.

```cpp
AscendC::Reg::RegTensor<float> srcReg;
AscendC::Reg::RegTensor<float> dstReg;

AscendC::Reg::LoadAlign(srcReg, srcAddr);
AscendC::Reg::Add(dstReg, srcReg, srcReg, mask);
```

When an API parameter already contains `RegTensor<T>`, the template parameter `T` can usually be deduced by the compiler. When a specific mode or conversion API is used, or when ambiguity needs to be eliminated, it can be specified explicitly.

### Movement Mode Template Parameters

The basic APIs use template parameters such as `LoadDist`, `StoreDist`, and `PostLiteral` to express the movement mode.

```cpp
// Aligned copy-in, using the LoadDist template parameter.
AscendC::Reg::LoadAlign<float, AscendC::Reg::LoadDist::DIST_BRC_B32>(srcReg, srcAddr);

// PostUpdate aligned copy-in. The API updates srcAddr after execution.
AscendC::Reg::LoadAlign<float, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, stride);

// Aligned copy-out, using the StoreDist template parameter.
AscendC::Reg::StoreAlign<float, AscendC::Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(dstAddr, dstReg, mask);
```

### MaskMergeMode Template Parameter

Some Reg computation APIs provide the `MaskMergeMode` template parameter, which describes how elements not selected by the mask are handled in the destination register:

- `MaskMergeMode::ZEROING`: Elements not selected by the mask are set to zero in `dstReg`.

- `MaskMergeMode::MERGING`: Elements not selected by the mask retain their original values in `dstReg`.

```cpp
AscendC::Reg::Add<T, AscendC::Reg::MaskMergeMode::ZEROING>(dstReg, src0Reg, src1Reg, mask);

AscendC::Reg::Add<T, AscendC::Reg::MaskMergeMode::MERGING>(dstReg, src0Reg, src1Reg, mask);
```

> 📌 **Note**: `MaskMergeMode` is supported only by some API forms and is implemented by software simulation. In actual use, refer to the function prototype and parameter description in the specific API document. For scenarios where the old values of unselected elements do not need to be retained, use the default `ZEROING` semantics.

## Data Objects and Register Types<a name="section_part4"></a>

The basic APIs use C++ types to express the register resources in Reg vector programming.

| Register Category | Basic API Type | Purpose |
| --- | --- | --- |
| Vector data register | `RegTensor<T, regTrait>` | Stores VL- or multi-VL-length vector data, and serves as the main source/destination operand of computation APIs. |
| Mask register | `MaskReg` | Controls which elements participate in computation, and can also carry comparison, logical, and other results. |
| Address register | `AddrReg` | Stores the UB access offset, suitable for movement scenarios such as fixed stride and multi-dimensional indexing. |
| Unaligned copy-in register | `UnalignRegForLoad` | Temporary buffer for contiguous unaligned copy-in. |
| Unaligned copy-out register | `UnalignRegForStore` | Temporary buffer for contiguous unaligned copy-out. |

Register objects such as `RegTensor` and `MaskReg` can only be used as a whole, and cannot be accessed element by element as in a C++ array. For the related hardware meanings, widths, and mask bit correspondence, see "Register Types and Allocation" in [Reg Vector Computation Programming (Language Extension C API)](../c_pointer_programming/reg_vector_compute_programming.md).

### Connecting to LocalTensor

The Reg vector movement APIs actually access UB addresses. In tensor-based Cpp programming, the outer layer typically uses `LocalTensor` to manage UB data.

```cpp
template <typename T>
__aicore__ inline void Compute()
{
    AscendC::LocalTensor<T> dst = ubAllocator.Alloc<T, TILE_LENGTH>();
    // Parameter calculation, etc.
    ......
    asc_vf_call<AddVF<T>>(dst, src0, src1, oneRepeatSize, repeatTimes);
    // Tail processing
    ......
}
```

This approach allows the basic API to connect with the `LocalTensor` flow in `__aicore__`.

> 📌 **Note**: When using LocalTensor as a VF function parameter, you cannot use a reference LocalTensor &. This is because `__aicore__` and `__simd_vf__` are two mutually isolated execution domains, and access by reference across execution domains is not possible.

## Features<a name="section_part5"></a>

### Register Construction

#### Creating a Mask

[`CreateMask`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/MaskReg.md) is used to create a `MaskReg` with a fixed pattern, and [`UpdateMask`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/MaskReg.md) is used to generate a tail block mask based on the remaining element count.

```cpp
AscendC::Reg::MaskReg fullMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();

AscendC::Reg::MaskReg mask = AscendC::Reg::UpdateMask<T>(remainCount);
```

`UpdateMask<T>(remainCount)` is suitable for tail block processing in a VF loop. Its semantics are consistent with those of `asc_update_mask_b*` in the C API.

#### Creating an Address Register

[`CreateAddrReg`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/AddrReg.md) is used to create an `AddrReg` that stores the UB offset, supporting up to four levels of loop jumps.

```cpp
AscendC::Reg::AddrReg aReg;
for(uint16_t i = 0;i < extent1; i++){
    for(uint16_t j = 0;j < extent2; j++){
        for(uint16_t k = 0;k < extent3; k++){
            for(uint16_t m = 0;m < extent4; m++){
                aReg = AscendC::Reg::CreateAddrReg(i, const1, j, const2, k, const3, m, const4);
                AscendC::Reg::LoadAlign(srcReg, srcAddr, aReg);
            }
        }
    }
}
```

### Data Movement

#### General Movement

`Load`/`Store` do not require users to distinguish whether the UB address is 32-byte-aligned, making them suitable for scenarios where aligned and unaligned addresses need to be handled uniformly.

```cpp
AscendC::Reg::Load(srcReg, srcAddr);
AscendC::Reg::Store(dstAddr, dstReg);
AscendC::Reg::Store(dstAddr, dstReg, count);
```

General load/store is a software wrapper. When performance is critical and the address meets the alignment condition, use aligned load/store first.

#### Aligned Movement

[`LoadAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadAlign_continuous.md)/[`StoreAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_continuous.md) are suitable for contiguous copy-in/copy-out when the UB address meets the alignment requirement, and are the most common movement APIs in Reg vector programming. During movement, operations such as data layout can be performed in-line based on the built-in functions. For details, see the corresponding API sections.

```cpp
AscendC::Reg::LoadAlign(srcReg, srcAddr);
AscendC::Reg::StoreAlign(dstAddr, dstReg, mask);
```

#### Unaligned Movement

Contiguous unaligned copy-in is implemented by combining [`LoadUnAlignPre`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md) and [`LoadUnAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md); contiguous unaligned copy-out is implemented by combining [`StoreUnAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md) and [`StoreUnAlignPost`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md).

```cpp
AscendC::Reg::UnalignRegForLoad loadUreg;
AscendC::Reg::UnalignRegForStore storeUreg;

AscendC::Reg::LoadUnAlignPre(loadUreg, srcAddr);
for (uint16_t i = 0; i < repeatTimes; ++i) {
    AscendC::Reg::LoadUnAlign(srcReg, loadUreg, srcAddr, postUpdateStride);
    // Reg computation
    ...
    AscendC::Reg::StoreUnAlign(dstAddr, dstReg, storeUreg, postUpdateStride);
}
AscendC::Reg::StoreUnAlignPost(dstAddr, storeUreg, 0);
```

### Vector Computation

Vector computation APIs directly operate on `RegTensor<T>` or `MaskReg`, and usually carry a `MaskReg mask` to control the valid elements.

#### `RegTensor<T>` and `RegTensor<T>` Computation

The basic arithmetic APIs perform element-wise computation between registers, and the scalar arithmetic APIs perform element-wise computation between a register and a scalar.

```cpp
AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
AscendC::Reg::Mul(dstReg, src0Reg, src1Reg, mask);
AscendC::Reg::Adds(dstReg, srcReg, scalarValue, mask);
AscendC::Reg::Muls(dstReg, srcReg, scalarValue, mask);
```

#### `MaskReg` Computation

Logical APIs can operate on `RegTensor` or `MaskReg`; comparison APIs usually output the comparison result, and selection APIs choose between two source operands based on the mask.

```cpp
AscendC::Reg::And(dstMaskReg, src0MaskReg, src1MaskReg, mask);
AscendC::Reg::Compare(cmpMaskReg, src0Reg, src1Reg, mask);
AscendC::Reg::Select(dstReg, src0Reg, src1Reg, cmpMaskReg, mask);
```

These APIs often have more template parameters or restrictions. It is recommended that you refer to the corresponding API page when using them.

### Synchronization Control

The basic API uses [`LocalMemBar`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/sync_control/LocalMemBar.md) to express the synchronization constraint on UB access pipelines within the VF.

```cpp
AscendC::Reg::Store(dstAddr, dstReg);
AscendC::Reg::LocalMemBar<AscendC::Reg::MemType::VEC_STORE,
                          AscendC::Reg::MemType::VEC_LOAD>();
AscendC::Reg::Load(srcReg, dstAddr);
```

The time when synchronization is required is unrelated to the API name. For the judgment rules, see "Pipeline Synchronization" in [Reg Vector Computation Programming (Language Extension C API)](../c_pointer_programming/reg_vector_compute_programming.md).

## Programming Example<a name="section_part6"></a>

The following example shows the typical calling method of the basic APIs. The outer `__aicore__` function is still responsible for `LocalTensor` management, and the VF function uses the Reg basic APIs to complete data copy-in, computation, and data copy-out.

```cpp
template <typename T>
__simd_vf__ inline void AddVFv1(AscendC::LocalTensor<T> dst,
                              AscendC::LocalTensor<T> src0,
                              AscendC::LocalTensor<T> src1,
                              uint32_t oneRepeatSize,
                              uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> src0Reg;
    AscendC::Reg::RegTensor<T> src1Reg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask =
        AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        uint32_t offset = i * oneRepeatSize;
        AscendC::Reg::LoadAlign(src0Reg, src0 + offset);
        AscendC::Reg::LoadAlign(src1Reg, src1 + offset);
        AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
        AscendC::Reg::StoreAlign(dst + offset, dstReg, mask);
    }
}

template <typename T>
__simd_vf__ inline void AddVFv2(__ubuf__ float* dstAddr,
                                __ubuf__ float* src0Addr,
                                __ubuf__ float* src1Addr,
                                uint32_t oneRepeatSize,
                                uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> src0Reg;
    AscendC::Reg::RegTensor<T> src1Reg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask =
        AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        uint32_t offset = i * oneRepeatSize;
        AscendC::Reg::LoadAlign(src0Reg, src0Addr + offset);
        AscendC::Reg::LoadAlign(src1Reg, src1Addr + offset);
        AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
        AscendC::Reg::StoreAlign(dstAddr + offset, dstReg, mask);
    }
}

template <typename T>
__aicore__ inline void Compute()
{
    AscendC::LocalTensor<T> dst = ubAllocator.Alloc<T, TILE_LENGTH>();

    constexpr uint32_t oneRepeatSize = 256 / sizeof(T);
    uint16_t repeatTimes = TILE_LENGTH / oneRepeatSize;
	// Use LocalTensor as the parameter.
    asc_vf_call<AddVFv1<T>>(dst, src0, src1, oneRepeatSize, repeatTimes);
    
    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* src0Addr = (__ubuf__ T*)src0.GetPhyAddr();
    __ubuf__ T* src1Addr = (__ubuf__ T*)src1.GetPhyAddr();
    // Use the UB address as the parameter.
    asc_vf_call<AddVFv2<T>>(dstAddr, src0Addr, src1Addr, oneRepeatSize, repeatTimes);
}
```

If you need to process the tail block, you can use `UpdateMask<T>` in the VF loop:

```cpp
uint32_t remainCount = count;
for (uint16_t i = 0; i < repeatTimes; ++i) {
    AscendC::Reg::MaskReg mask = AscendC::Reg::UpdateMask<T>(remainCount);
    // Load / Compute / Store
}
```

## Summary<a name="section_part7"></a>

The basic APIs and C APIs describe the same set of Reg vector computation capabilities, and their programming models and hardware semantics remain consistent. The basic APIs differ mainly in the API expression: they organize APIs through the `AscendC::Reg::*` namespace, use C++ register types to describe the data objects inside the VF, and carry configurations such as data type, movement mode, and computation mode through template parameters. When interfacing with `LocalTensor`, you can directly pass `LocalTensor` as a parameter to a VF function in an `__aicore__` function, or obtain the UB address through `LocalTensor::GetPhyAddr()` and pass it to the VF function, and then use the Reg movement APIs inside the VF function to complete the access.