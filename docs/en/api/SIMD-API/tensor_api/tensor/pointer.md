# pointer

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T04:22:43.925Z pushedAt=2026-10-27T13:03:35.486Z nl=2 -->

## Description

The Tensor API constructs a pointer adapter carrying physical storage location information through `make_mem_ptr`. The pointer adapter retains the access capability of the original pointer or iterator and marks physical locations such as Global Memory, L1 Buffer, and L0A Buffer through `ptr_pattern`. When using it, you only need to call the external API `make_mem_ptr`; you do not need to directly construct the internal pointer adapter type.

## Constraints

- The original pointer must point to a valid memory space.
- The specified `location` must be consistent with the actual physical storage location.
- The data type of the pointer adapter must be consistent with the actual data type.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr int32_t size = 256;
__ubuf__ float ub_data[size];
__cbuf__ float l1_data[size];
__ca__ float l0a_data[size];

// Create a Unified Buffer (UB) pointer.
auto ub_ptr = asc::te::make_mem_ptr(ub_data);

// Create an L1 Buffer pointer.
auto l1_ptr = asc::te::make_mem_ptr(l1_data);

// Create an L0A Buffer pointer.
auto ca_ptr = asc::te::make_mem_ptr(l0a_data);
```
