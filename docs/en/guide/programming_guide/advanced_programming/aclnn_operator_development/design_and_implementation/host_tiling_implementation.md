# Basic Process

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:47:24.188Z pushedAt=2026-11-19T06:32:19.863Z nl=2 -->

This document covers the basics and describes the basic process of writing host-side tiling in the aclnn engineering-based operator development approach. Host-side tiling takes the tiling design conclusions from [Operator Function Design](./operator_function_design.md), and converts the input shape, attributes, and running scenarios into the parameters required before the kernel function is launched.

## Tiling Concept Review

Tiling is the process of determining the data partitioning method and runtime configuration based on the data scale, platform resources, and kernel function implementation before the kernel function is launched. Tiling itself does not perform the tensor computation of the operator; instead, it prepares the parameters required for the kernel function to run.

For common data-parallel operators, data is typically partitioned at the following two levels:

- Block: the logical parallel unit of the kernel function. The overall data is distributed to multiple blocks for parallel processing, and the number of blocks is specified by `numBlocks`.
- Tile: the data unit processed in batches within a single block. When the data assigned to a block cannot fit into on-chip storage at once (for example, the Unified Buffer (UB)), the kernel function loops over tiles to complete data loading, computation, and write-back.

TilingFunc runs on the host side. The framework calls TilingFunc before the kernel function is launched and provides the input/output shapes and attribute information through `TilingContext`. TilingFunc reads and validates this information, computes the partitioning parameters, selects the kernel function branch, and finally writes back two types of results:

- `TilingData`: the parameters that the kernel function needs to read during execution, such as the total number of elements, the amount of data processed at a time, the number of loop iterations, the tail block length, and branch parameters.
- Kernel function launch configuration: for example, `numBlocks`, the workspace size, `TilingKey`, and the scheduling mode.

The inputs and outputs of host-side tiling are shown in the following figure:

<img src="../../../../figures/tiling_io.png" alt="Input and output of Tiling implementation" width="300px" height="407px">

In the figure, `TilingContext` is the data carrier between TilingFunc and the framework: TilingFunc reads the input and output shapes and attribute information from it, and writes back the results such as `TilingData`, `numBlocks`, `TilingKey`, and the workspace size.

## Core Concepts

The outputs of host-side tiling mainly include `TilingData`, `numBlocks`, workspace size, and `TilingKey`. Different runtime scenarios may also involve kernel function launch configurations such as the schedule mode. Understanding these outputs before writing code makes the subsequent process easier to follow.

### TilingData (Required)

`TilingData` carries the tiling results passed from the host side to the kernel function side. You only need to define the fields actually used by the kernel function. Common fields include the total number of elements, the amount of data processed at a time, the number of loops, the tail block length, and branch parameters. The specific fields are determined by the kernel function implementation.

The AddCustom basic sample defines only `totalLength` and `tileNum`. `totalLength` indicates the total number of elements, and `tileNum` indicates the number of blocks into which the total data is divided in the sample. The kernel function computes the amount of data processed at a time through `totalLength / tileNum`, and together with `numBlocks` determines the number of loops for each Block.

AscendC recommends using **standard C++ syntax** to define the tiling structure. This approach makes it easy to organize fixed-size, directly copyable fields, and allows different operators to use structures with the same name. For constraints on field types, array syntax, and host/kernel function side consistency, see [Usage Constraints](#tilingdata-constraints).

> [!NOTE] When to Use the Macro Definition Approach
> When a specific API has special requirements on how the tiling structure is defined, follow the corresponding API documentation. When maintaining legacy projects developed with `BEGIN_TILING_DATA_DEF`, you can also continue to use the macro definition approach.

This document uses standard C++ syntax as the main line to introduce the basic process of tiling implementation.

### numBlocks (Required)<a id="numblocks"></a>

`numBlocks` specifies the number of SIMD blocks launched by the kernel function. It should be determined based on the data scale, tiling strategy, and hardware resources. The configurable range is also subject to the AI processor model, the kernel function execution type, and runtime resource constraints. For details, see [Kernel Function Configuration](../../../language_extension/simd_builtin_keywords.md#section97005415463).

### Workspace Size (Required)

The workspace is temporary memory in device-side Global Memory, used as auxiliary storage during kernel function execution. The host-side tiling function computes and sets the required size, based on which the framework or caller allocates the memory, and the kernel function accesses the workspace through the entry parameter workspace. The workspace memory consists of two parts: system workspace and user workspace:

- System workspace: workspace memory that needs to be reserved for Ascend C APIs

    During computation, APIs require some workspace memory as cache. Therefore, the operator needs to reserve workspace memory for the APIs. The size of the reserved memory is obtained through the [GetLibApiWorkSpaceSize](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md) API.
- User workspace: workspace memory used by the operator implementation (as needed)

    User workspace needs to be allocated only when the operator requires additional device memory for data exchange or caching. Allocate it as needed. The usage scenarios are as follows:

    - When the space on the Unified Buffer (UB) and L1 Buffer is required but insufficient, data can be temporarily stored in the workspace.
    - When calling APIs such as [SyncAll](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll.md), the workspace is required as an input parameter.
    - Other scenarios where memory space in the Global Memory is required.

The overall workspace memory is the sum of the system workspace and the user workspace, and is set through the `GetWorkspaceSizes` API in the tiling function. The specific usage is as follows:

1. In the tiling function, first call the **GetWorkspaceSizes** API to obtain the storage location of the workspace size, and then set the workspace size. The framework allocates the corresponding size of device-side Global Memory, which can be used in the kernel-side implementation of the operator. When using advanced APIs that require system workspace, such as the [Matmul Kernel Function API](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_Kernel.md), the workspace size to be set is the sum of the system workspace and the user workspace.

    ```cpp
    // User-defined tiling function
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        AddApiTiling tiling;
        ...
        size_t usrSize = 256; // Set the workspace size required by the user to 256 bytes.
        // Call GetLibApiWorkSpaceSize to obtain the system workspace size.
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
        if (sysWorkspaceSize == std::numeric_limits<uint32_t>::max()) {
            return ge::GRAPH_FAILED;
        }
        size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input of GetWorkspaceSizes is the number of workspace blocks required.
        currentWorkspace[0] = usrSize + sysWorkspaceSize;
        ...
    }
    ```

2. The workspace at the device-side kernel function entry is the user workspace pointer:

    ```cpp
    // The kernel function written by the user must include the GM_ADDR workspace input parameter, which must be placed before tiling.
    extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        ...

    }
    ```

> [!NOTE] Workspace Application in Different Execution Scenarios
>
> In the single-operator API execution scenario, the developer obtains the workspace size through the first-stage API and allocates Global Memory of the corresponding size. In the graph scenario, the framework automatically allocate the memory based on the size set by the host-side tiling.

### TilingKey (Optional)

`TilingKey` is used to distinguish different kernel function implementation branches of the same operator. Host-side tiling selects a numeric identifier based on shape, dtype, attributes, or the running scenario, and writes it into the context through `context->SetTilingKey(...)`. The kernel side and the compilation toolchain select the corresponding implementation based on this number. For more information, see [Multi-Branch Strategy](./multi_branch_strategy.md).

## Tiling Implementation Process

The host-side tiling implementation of AddCustom can be understood in three steps:

1. Define the `TilingData` structure to determine the fields that the host side needs to pass to the kernel function side.
2. Write `TilingFunc` to read the shape and write `TilingData`, `numBlocks`, and the workspace size.
3. Associate the tiling function with the operator prototype through `AICore().SetTiling(...)`.

### Defining the TilingData Structure<a id="define-tilingdata-structure"></a>

The `TilingData` header file in the AddCustom sample is located at:

```text
op_kernel/add_custom_tiling.h
```

The code is as follows:

```cpp
#ifndef ADD_CUSTOM_TILING_H
#define ADD_CUSTOM_TILING_H
#include <cstdint>

struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;
};

#endif // ADD_CUSTOM_TILING_H
```

The fields are described as follows:

| Field | Meaning | Purpose on the Kernel Function Side |
|---|---|---|
| `totalLength` | Total number of elements to be computed this time | Computes the data range processed by each block |
| `tileNum` | Number of tiles into which the total data is divided in the sample | The kernel computes the amount of data processed at a time through `totalLength / tileNum`, and determines the number of loops for each block together with `numBlocks` |

The structure header file is placed in the `op_kernel` directory so that both the host-side tiling function and the kernel-side implementation can reference this definition.

### Writing TilingFunc

The host-side implementation file of AddCustom is located at:

```text
op_host/add_custom.cpp
```

This file defines `optiling::TilingFunc`. After the operator prototype associates this function through `SetTiling(optiling::TilingFunc)`, the framework calls it before the operator is executed and passes in `gert::TilingContext* context`.

The AddCustom sample code is as follows:

```cpp
#include "../op_kernel/add_custom_tiling.h"
#include "register/op_def_registry.h"

namespace optiling {
const uint32_t NUM_BLOCKS = 8;
const uint32_t TILE_NUM = 8;

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddCustomTilingData* tiling = context->GetTilingData<AddCustomTilingData>();
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    context->SetSimdNumBlocks(NUM_BLOCKS);
    tiling->totalLength = totalLength;
    tiling->tileNum = TILE_NUM;
    size_t* currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

> [!NOTE] Tiling Premise of the AddCustom Sample
> The sample input shape is `[8, 2048]`, with a total of 16384 elements. The host side sets both `numBlocks` and `tileNum` to 8. The kernel function computes that each Block processes 2048 elements through `totalLength / GetBlockNum()`, and computes that a single pass also processes 2048 elements through `totalLength / tileNum`. Therefore, each block loops only once.

This code completes tiling through the following steps:

1. Obtain the `TilingData` pointer.
2. Read the shape of input `x` and compute the total number of elements.
3. Set `numBlocks`, and write `totalLength` and `tileNum`.
4. Set the workspace size to 0.
5. Return `ge::GRAPH_SUCCESS`.

In the standard C++ structure mode, the host side obtains the structure pointer through `GetTilingData<T>()` and directly writes fields, and the framework handles subsequent data transfer. The way the kernel function side reads the data is described in [Kernel-Side Operator Implementation](./kernel_operator_implementation.md).

<!-- npu="950" id1 -->
> [!NOTE] Dynamic UB Configuration in SIMD and SIMT Hybrid Scenarios
>
> When SIMD and SIMT hybrid programming is enabled, at least 32 KB of Data Cache must be reserved for SIMT. The host-side tiling must determine the dynamic UB size based on the static UB used by the kernel function, the reserved space, and the Data Cache size, and call [`SetDynUBufSize`](https://gitcode.com/cann/metadef/blob/master/docs/en/api/gert_namespace/tilingcontext/SetDynUBufSize.md) to set it. You can call [`GetDynUBufSize`](https://gitcode.com/cann/metadef/blob/master/docs/en/api/gert_namespace/tilingcontext/GetDynUBufSize.md) to query the dynamic UB size that has been set.
>
> Assume that TilingFunc has computed `dynUBufSize` based on the actual requirements of the kernel function. The call example is as follows:
>
> ```cpp
> ge::graphStatus ret = context->SetDynUBufSize(dynUBufSize);
> if (ret != ge::GRAPH_SUCCESS) {
>     return ret;
> }
> ```
>
> For details about the dynamic application method, size limit, and Data Cache layout of shared memory, see [Shared Memory](../../../programming_model/ai_core_simt_programming/memory_hierarchy.md#section66329146410).
<!-- end id1 -->

### Associating the Operator Prototype

After the tiling function is written, you need to associate it in the operator prototype definition through `AICore().SetTiling(...)`. The prototype code in the AddCustom sample is as follows:

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc);
```

> [!NOTE] Note
>
> An operator can register only one host-side Tiling function. If different AI processors require different tiling configurations, distinguish the AI processors and set the corresponding parameters within the same tiling function. Do not register multiple tiling functions for different AI processors.

`SetTiling` establishes the call relationship between the operator prototype and the host-side tiling function. This document only describes how to prepare the kernel function running parameters inside `TilingFunc`. For the complete prototype configuration, see [Operator Prototype Definition](./operator_prototype_definition.md).

## Constraints<a id="tilingdata-constraints"></a>

When defining the `TilingData` structure using standard C++ syntax, you can inspect it by code form. The following types of writing are most likely to affect data transfer between the host side and the kernel function side.

### Avoiding Member Function Definitions

`TilingData` is used by both the host side and the kernel side. Member functions are easily affected by the difference in the `__aicore__` modifier.

```cpp
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;

    __aicore__ void Init() { } // Host-side compilation is affected by the __aicore__ modifier.
    void Reset() { }           // The __aicore__ modifier is missing when used on the kernel function side.
};
```

### Avoiding Pointer and Reference Fields

When the host side writes `TilingData`, the written content must be serializable and transferable to the kernel function side. Pointers and references express object relationships on the host side, which cannot be resolved according to the original object relationships after being transferred to the kernel function side.

```cpp
struct AddCustomTilingData {
    uint32_t* totalLength; // Avoid using pointer fields.
    uint32_t& tileNum;     // Avoid using reference fields
};
```

### Keeping Field Types Simple

`TilingData` fields should use types whose size is determined at compile time and that can be copied directly, such as `uint32_t`, `int64_t`, `float`, `bool`, and fixed-length arrays. Pointers, references, dynamic containers, and complex objects containing virtual functions or virtual inheritance are not suitable as `TilingData` fields.

```cpp
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;
    bool enableExtraPath;
    uint32_t tileSizes[8];
};
```

Complex objects and dynamic memory-related information are computed in the host-side tiling function, and then the results required by the kernel function are written as simple fields.

### Explicitly Assigning Values to Fields

The fields of the structure obtained by `GetTilingData<T>()` have no default business meaning. Any field that the kernel will subsequently read must be explicitly assigned in the host-side tiling function.

```cpp
AddCustomTilingData* tiling = context->GetTilingData<AddCustomTilingData>();
uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();

tiling->totalLength = totalLength;
tiling->tileNum = TILE_NUM;
```

### Using a using Alias for Multiple Template Parameters

If `TilingData` uses multiple template parameters, the registration macro on the kernel function side will treat the commas in the template parameters as macro argument separators. In such scenarios, first define an alias with `using`, and then pass the alias to the registration macro.

```cpp
template<int32_t sizeA, int32_t sizeB>
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t dataArray[sizeA][sizeB];
};

using AddCustomTiling = AddCustomTilingData<3, 5>;
REGISTER_TILING_DEFAULT(AddCustomTiling);
```

## Related Documents

- [Operator Prototype Definition](./operator_prototype_definition.md): Learn how to associate the tiling function with the operator prototype through `AICore().SetTiling(...)`.
- [Passing Attribute Information Through TilingData](./tiling_data_attributes.md): Learn how to read operator attributes and pass them to the kernel side through TilingData.
- [Tiling Implementation for Advanced APIs](./tiling_with_advanced_api.md): Learn how to compute and pass the corresponding tiling parameters when using advanced APIs.
- [Kernel-Side Operator Implementation](./kernel_operator_implementation.md): Learn how the kernel side reads `TilingData` and uses the runtime parameters set on the host side.
- [Multi-Branch Strategy](./multi_branch_strategy.md): Learn how to select different kernel implementation branches through `TilingKey`.
