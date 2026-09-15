# MakeMemPtr

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:02:46.942Z -->

> [!NOTE]Description
> This API is an experimental API. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers are advised to keep track of later version updates during use.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

## Description

The header file to be included is: `#include "tensor_api/tensor.h"`.

**MakeMemPtr** is used to construct the memory pointer adapter **HardwareMemPtr** with physical storage location markers and data type information. The supported physical locations are listed in the following table.

**MakeMemPtr** provides three calling forms:

1. Specify the physical location and data type, and construct a pointer based on the original address.
2. Specify the physical location, and construct a new pointer from an existing iterator or pointer adapter.
3. Automatically deduce the physical location from an existing iterator or pointer adapter.

## Prototype

- Specify the physical location and data type, and construct from the original address:

  ```cpp
  template <typename Hardware, typename DataType, typename Addr, EnableMakePtrByTrait<Hardware, Addr> Enable>
  __aicore__ inline constexpr auto MakeMemPtr(Addr addr)
  ```

- Specify the physical location and construct from an existing iterator:

  ```cpp
  template <typename Hardware, typename Iterator, EnableMakeHardwarePtr<Hardware, Iterator> Enable>
  __aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
  ```

- Automatically deduce the physical location from an existing iterator:

  ```cpp
  template <typename Iterator, EnableMakePtrByIter<Iterator> Enable>
  __aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
  ```

## Parameters

**Table 1** Supported physical locations

| Location | Description | Purpose |
| :--- | :--- | :--- |
| Location::GM | Global Memory | Global memory, the primary storage area for tensor inputs and outputs. |
| Location::L1 | L1 Buffer | Level-1 cache, the temporary data staging area before cube transfer and computation. |
| Location::L0A | L0A Buffer | Left input buffer for cube computation. |
| Location::L0B | L0B Buffer | Right input buffer for cube computation. |
| Location::L0ScaleA | L0 ScaleA Buffer | ScaleA buffer for MX cube computation. |
| Location::L0ScaleB | L0 ScaleB Buffer | ScaleB buffer for MX cube computation. |
| Location::L0C | L0C Buffer | Result buffer for cube computation. |
| Location::UB | Unified Buffer | Unified buffer, the data storage area for vector computation. |
| Location::BIAS | BiasTable Buffer | Bias table buffer, the bias storage area for cube computation with bias. |
| Location::FIXBUF | Fixpipe Buffer | Fixpipe output buffer, the transfer area from L0C to GM/UB. |

**Table 2** Template parameters

| Parameter | Type | Description |
| :--- | :---: | :--- |
| Hardware | Input | Physical storage location type. |
| DataType | Input | Data type. |
| Addr | Input | Type of the offset value. |
| Iterator | Input | Iterator type or existing pointer adapter type. |

**Table 3** Parameters

| Parameter | Type | Description |
| :--- | :---: | :--- |
| addr | Input | Address offset, that is, the offset relative to the base address. |
| iterator | Input | Existing hardware address iterator or pointer adapter object. |

## Return Value

Returns a pointer adapter object of the **HardwareMemPtr<PtrPattern, Pointer>** type, where:

- **PtrPattern** indicates the specified or deduced physical location;
- **Pointer** indicates the pointer type in the corresponding address space, for example, `__gm__ float*`, `__cbuf__ half*`, and so on.

## Constraints

- When **Hardware** is specified, it must be a supported physical location type.
- **Iterator** must satisfy the template constraints of the corresponding construction path.
- The automatic deduction form requires that the input object itself already carries recognizable hardware location information.

## Example

```cpp
using namespace AscendC::Te;
constexpr uint64_t gmOffset = 128;
constexpr uint64_t l1Offset = 128;
constexpr uint64_t l0aOffset = 128;
constexpr uint64_t ubOffset = 128;
// 1. Construct from the raw address: specify the physical location and data type.
auto gmPtr = MakeMemPtr<Location::GM, float>(gmOffset);
auto l1Ptr = MakeMemPtr<Location::L1, float>(l1Offset);
auto l0aPtr = MakeMemPtr<Location::L0A, half>(l0aOffset);
auto ubPtr = MakeMemPtr<Location::UB, bfloat16_t>(ubOffset);

// 2. Construct a tensor together with MakeTensor.
auto gmA = MakeTensor(gmPtr, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));
auto l1A = MakeTensor(l1Ptr, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));

// 3. Inherit the location from an existing pointer adapter (automatically deduced).
auto anotherPtr = MakeMemPtr(gmPtr);

// 4. Specify a new location from an existing iterator.
auto l1FromIter = MakeMemPtr<Location::L1>(someIterator);
```
