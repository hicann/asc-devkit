---
okftype: reference.api
description: "Generates a no-operation instruction that performs no actual computation. Each call blocks one pipeline beat and can be used for debugging and pipeline alignment scenarios."
tag:
  - roles: [control]
dependencies: []
version: ">=9.2.0"
version_changes: []
---
# asc_nop

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.598472+00:00 pushedAt=2026-11-16T02:10:57.897Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"c_api/utils/sys_misc.h"`.

Generates a no-operation instruction that performs no actual computation. Each call blocks one pipeline stage.

## Prototype

```c
__aicore__ inline void asc_nop()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

To block exactly n pipeline stages, insert [`asc_sync`](../../sync/intra_core_sync/asc_sync.md) before and after n `asc_nop` API calls, for example:

```c
// Block 3 pipeline stages.
asc_sync();
asc_nop();
asc_nop();
asc_nop();
asc_sync();
```

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <iostream>
#include "acl/acl.h"
#include "kernel_operator.h"
#include "c_api/asc_simd.h"

namespace {
__global__ __vector__ void asc_nop_kernel()
{
    asc_init();
    asc_sync();
    asc_nop();
    asc_nop();
    asc_nop();
    asc_sync();
}
} // namespace

int main()
{
    aclInit(nullptr);
    aclrtSetDevice(0);
    asc_nop_kernel<<<1, 0>>>();
    const aclError ret = aclrtSynchronizeDevice();
    const bool passed = ret == ACL_SUCCESS;
    std::cout << (passed ? "[Success] asc_nop passed." : "[Failed] asc_nop failed.") << std::endl;
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
