# make_mem_ptr

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T04:22:28.830Z pushedAt=2026-10-27T13:03:35.487Z nl=2 -->

## Applicable Products

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
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `tensor_api/tensor.h`.

make_mem_ptr is used to construct a memory pointer adapter `hardware_mem_ptr` that carries a physical storage location marker and data type information. The supported physical locations are listed in the following table.

make_mem_ptr provides three invocation forms:

1. Specify the physical location and data type, and construct a pointer from the original address.
2. Specify the physical location, and construct a new pointer from an existing iterator or pointer adapter.
3. Automatically deduce the physical location from an existing iterator or pointer adapter.

## Function Prototype

- Specify the physical location and data type, and construct from the original address:

  ```cpp
  template <typename PtrPattern, typename DataType, typename Addr>
  __aicore__ inline auto make_mem_ptr(Addr address)
  ```

- Specify the physical location, and construct from an existing iterator:

  ```cpp
  template <typename PtrPattern, typename Iterator>
  __aicore__ inline auto make_mem_ptr(Iterator iterator)
  ```

- Automatically deduce the physical location from an existing iterator:

  ```cpp
  template <typename Iterator>
  __aicore__ inline auto make_mem_ptr(Iterator iterator)
  ```

## Parameters

**Table 1**  Supported physical locations

| location | Description | Purpose |
| :--- | :--- | :--- |
| location::gm | Global Memory | Global memory, the primary storage area for tensor inputs and outputs. |
| location::l1 | L1 Buffer | Level-1 cache, the temporary data storage area before cube movement and computation. |
| location::l0a | L0A Buffer | Left input buffer for cube computation. |
| location::l0b | L0B Buffer | Right input buffer for cube computation. |
| location::l0scalea | L0 ScaleA Buffer | ScaleA buffer for MX cube computation. |
| location::l0scaleb | L0 ScaleB Buffer | ScaleB buffer for MX cube computation. |
| location::l0c | L0C Buffer | Result buffer for cube computation. |
| location::ub | Unified Buffer (UB) | Unified buffer, the data storage area for vector computation. |
| location::bias | BiasTable Buffer | Bias table buffer, the bias storage area for cube computation with bias. |
| location::fixbuf | Fixpipe Buffer | Fixpipe output buffer, the movement area from L0C to GM/UB. |

**Table 2** Template parameter description

| Parameter | Type | Description |
| :--- | :---: | :--- |
| PtrPattern | Input | Physical storage location type. |
| DataType | Input | Data type. |
| Addr | Input | Type of the offset value. |
| Iterator | Input | Iterator type or existing pointer adapter type. |

**Table 3**  Parameters

| Parameter | Type | Description |
| :--- | :---: | :--- |
| address | Input | Address offset, that is, the offset size relative to the base address. |
| iterator | Input | Existing hardware address iterator or pointer adapter object. |

## Return Value

Returns a pointer adapter object of the `hardware_mem_ptr<PtrPattern, Pointer>` type, where:

- PtrPattern indicates the specified or deduced physical location.
- Pointer indicates the pointer type in the corresponding address space, for example, __gm__ float* and \_\_cbuf_\_ half*.

## Constraints

- When PtrPattern is specified, it must be a supported physical location type.
- Iterator must satisfy the template constraints of the corresponding construction path.
- The automatic deduction form requires that the input object itself already carries recognizable hardware location information.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint64_t gm_offset = 128;
constexpr uint64_t l1_offset = 128;
constexpr uint64_t l0a_offset = 128;
constexpr uint64_t ub_offset = 128;
constexpr uint32_t m = 32;
constexpr uint32_t n = 32;
// 1. Construct from an original address: specify the physical location and data type.
// gm_offset, l1_offset, l0a_offset, and ub_offset are all address offsets of the uint64_t type.
auto gm_ptr = make_mem_ptr<location::gm, float>(gm_offset);
auto l1_ptr = make_mem_ptr<location::l1, float>(l1_offset);
auto l0a_ptr = make_mem_ptr<location::l0a, half>(l0a_offset);
auto ub_ptr = make_mem_ptr<location::ub, bfloat16_t>(ub_offset);

// 2. Construct a tensor together with make_tensor.
auto gm_a = make_tensor(gm_ptr, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(m, n));
auto l1_a = make_tensor(l1_ptr, make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n));

// 3. Inherit the location from an existing pointer adapter (automatically deduced).
// gm_ptr is a pointer adapter of the hardware_mem_ptr<location::gm, __gm__ float*> type.
auto another_ptr = make_mem_ptr(gm_ptr);

// 4. Specify a new location from an existing iterator.
// l1_iterator is an L1 Buffer address iterator derived from l1_ptr.
auto l1_iterator = l1_ptr + 1;
auto l1_from_iter = make_mem_ptr<location::l1>(l1_iterator);
```
