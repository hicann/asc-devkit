# Multi-Branch Strategy

<!-- md-trans-meta sourceCommit=fbd15d5df1f452aabb0f40f2b83dd25017ebaa8f translatedAt=2026-10-23T13:51:46.586Z pushedAt=2026-11-03T07:05:13.406Z nl=2 -->

This section is extended content. It describes how to use TilingKey or kernel function templates to organize multiple kernel function branches when the same operator needs to execute different computation flows based on dtype, shape, or attributes. TilingKey uses numeric identifiers to distinguish branches and is suitable for scenarios with a small number of branches and stable meanings. Kernel function templates use named parameters to describe branches and are suitable for scenarios where branches are determined by multiple dimensions together.

This section explains the reasons for using each approach, the correspondence between the Host side and the kernel function side, and the integration steps.

## Why Multiple Branches Are Needed

Ordinary runtime branching keeps all processing logic in the same kernel function:

```cpp
if (condition) {
    ProcessA();
} else {
    ProcessB();
}
```

Although a single execution enters only one of the branches, the code of both branches belongs to the same kernel function. When the branch code is large, it increases instruction cache pressure and the control overhead on the Scalar side.

After TilingKey is used, the branch condition can be turned into a compile-time constant:

```cpp
if (TILING_KEY_IS(1)) {
    ProcessA();
} else if (TILING_KEY_IS(2)) {
    ProcessB();
}
```

At compile time, the framework automatically identifies TilingKey and compiles multiple kernel function entry functions, performing **constant folding** on the condition check to eliminate the runtime branch judgment overhead.

Before adding branches, you also need to determine whether the differences truly require an independent kernel function implementation:

| Branch Dimension | Host-Side Judgment Basis | Typical Difference |
|---|---|---|
| dtype | `context->GetInputDesc()`, `context->GetOutputDesc()` | Computation type, transfer granularity, precision policy |
| shape | `context->GetInputShape()` | Direct computation for small shapes, block-based computation for large shapes, tail block processing |
| Attribute | `context->GetAttrs()` | Algorithm mode, dimension, switch |
| format | Input and output descriptions | Data organization and transfer method |

If different scenarios only affect runtime parameters such as the Tile size and the number of Blocks, while the kernel function computation flow remains unchanged, you only need to pass different parameters in TilingData and do not need to add kernel function branches. Only when the computation flow, data type instances, or code size actually differ do you need to use TilingKey or kernel function templates.

## Using TilingKey to Select a Branch

TilingKey uses a non-negative integer to identify a kernel function branch. The Host side selects the key for the current execution through `SetTilingKey`, and the kernel function side declares the branch corresponding to the key through `TILING_KEY_IS(key)`.

### Setting TilingKey on the Host Side

TilingKey is only responsible for selecting the kernel function computation branch and does not require each branch to use a different TilingData. When multiple branches require the same runtime parameter structure, they can share a single TilingData. The following selects different branches based on the input data volume and passes parameters through the same `TilingData`:

```cpp
namespace optiling {
constexpr uint32_t SHAPE_THRESHOLD = 1024;
constexpr uint32_t NUM_BLOCKS = 8;

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
    TilingData* tiling = context->GetTilingData<TilingData>();
    tiling->totalLength = totalLength;

    if (totalLength <= SHAPE_THRESHOLD) {
        context->SetTilingKey(1);
        tiling->tileNum = 1;
    } else {
        context->SetTilingKey(2);
        tiling->tileNum = NUM_BLOCKS * 8;
    }

    context->SetSimdNumBlocks(NUM_BLOCKS);
    size_t* currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

### Declaring Branches on the Kernel Function Side

In the kernel function, use `TILING_KEY_IS` to select the computation branch:

```cpp
extern "C" __global__ __aicore__ void my_op(
    GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    REGISTER_TILING_DEFAULT(optiling::TilingData);
    GET_TILING_DATA(tilingData, tiling);

    KernelMyOp op;
    op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
    if (TILING_KEY_IS(1)) {
        op.ProcessSmall();
    } else if (TILING_KEY_IS(2)) {
        op.ProcessLarge();
    }
}
```

### Using Different TilingData for a Specific Branch

Only when the field set or layout of a branch differs from the default structure do you need to register a separate TilingData type for that TilingKey. For example, key 1 uses the default `TilingData`, and key 2 uses `TilingData1`:

```cpp
REGISTER_TILING_DEFAULT(optiling::TilingData);
REGISTER_TILING_FOR_TILINGKEY("(TILING_KEY_VAR == 2)", optiling::TilingData1);

if (TILING_KEY_IS(1)) {
    GET_TILING_DATA(tilingData, tiling);
    // Use the default TilingData
} else if (TILING_KEY_IS(2)) {
    GET_TILING_DATA_WITH_STRUCT(optiling::TilingData1, tilingData, tiling);
    // Use TilingData1 corresponding to key 2
}
```

When using [REGISTER_TILING_FOR_TILINGKEY](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_FOR_TILINGKEY.md), you must also register the default structure through [REGISTER_TILING_DEFAULT](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_DEFAULT.md). On the Host side, you also need to call `GetTilingData<TilingData1>()` in the key 2 branch to write data, ensuring that the types and field layouts on both sides are consistent.

### Compiling Only the Specified TilingKey<a id="compile-selected-tiling-keys"></a>

During development and debugging, you can use `--tiling_key` to compile only the specified key, accelerating the compilation process:

```cmake
npu_op_kernel_options(ascendc_kernels MyOp
    COMPUTE_UNIT Ascendxxxyy
    OPTIONS --tiling_key=1)
```

When configuring by series, you can replace `COMPUTE_UNIT` with `SOC_SERIES`, for example, `SOC_SERIES Ascendxxx`. The two cannot be used together in the same invocation.

For a complete description of the compilation options, see [Basic Compilation and Deployment Process](../compilation_and_deployment/basic_process.md#custom-compile-options).

## Using Kernel Function Templates to Organize Branches<a name="using-kernel-function-templates-to-organize-branches"></a>

### Why Is a Kernel Function Template Needed

TilingKey uses numbers to identify branches. When there are many branches or when branches are affected by multiple conditions such as dtype, the number of tiles, and Boolean switches, it is difficult to directly tell the meaning of each branch from the numbers alone, and the mapping between the Host side and the kernel function side is also harder to maintain.

A kernel function template declares these branch dimensions as named template parameters, for example, `D_T_X`, `TILE_NUM`, and `IS_SPLIT`. The combinations of template parameters are uniformly constrained by the header file. The Host side selects a combination through `ASCENDC_TPL_SEL_PARAM`, and the kernel function entry directly receives the corresponding compile-time types and constants.

### Defining Template Parameters

Declare template parameters through `ASCENDC_TPL_ARGS_DECL`, and declare the allowed parameter combinations through `ASCENDC_TPL_SEL`.

The common macros for kernel function template programming are as follows. For details about the APIs, see [Template Parameter Definition](../../../../../api/Utils-API/Tiling_template_programming/template_params_definition.md):

| Macro Category | Macro Name | Description |
|---|---|---|
| Declaration entry | `ASCENDC_TPL_ARGS_DECL` | Defines the template parameter list and its order. |
| Data type parameter | `ASCENDC_TPL_DATATYPE_DECL` | Declares the data type template parameter and its optional values. |
| Integer parameter | `ASCENDC_TPL_UINT_DECL` | Declares the unsigned integer template parameter, encoding bit width, and value range. |
| Boolean parameter | `ASCENDC_TPL_BOOL_DECL` | Declares the boolean template parameter and its optional values. |
| Combination entry | `ASCENDC_TPL_SEL` | Defines the list of template parameter combinations allowed to be generated. |
| Single combination | `ASCENDC_TPL_ARGS_SEL` | Defines a group of template parameter combinations. |
| Combination constraint | `ASCENDC_TPL_DATATYPE_SEL`, `ASCENDC_TPL_UINT_SEL`, `ASCENDC_TPL_BOOL_SEL` | Constrains the value of each parameter in a combination. |
| TilingData structure | `ASCENDC_TPL_TILING_STRUCT_SEL` | Specifies the TilingData structure for the current `ASCENDC_TPL_ARGS_SEL` template parameter combination. It is not passed in as a kernel function template parameter. |

In the op_kernel directory of the operator project, add a header file to define the template parameters and template parameter combinations. In this example, the header file is named `tiling_key_add_custom_template.h`. The following uses dtype, the number of tiles, and the split switch as examples:

```cpp
#ifndef TILING_KEY_ADD_CUSTOM_TEMPLATE_H
#define TILING_KEY_ADD_CUSTOM_TEMPLATE_H
#include "ascendc/host_api/tiling/template_argument.h"

ASCENDC_TPL_ARGS_DECL(
    AddCustomTemplate,
    ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_FLOAT16, C_DT_FLOAT),
    ASCENDC_TPL_DATATYPE_DECL(D_T_Y, C_DT_FLOAT16, C_DT_FLOAT),
    ASCENDC_TPL_DATATYPE_DECL(D_T_Z, C_DT_FLOAT16, C_DT_FLOAT),
    ASCENDC_TPL_UINT_DECL(TILE_NUM, ASCENDC_TPL_8_BW,
                          ASCENDC_TPL_UI_LIST, 1, 8),
    ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1),
);

ASCENDC_TPL_SEL(
    ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ),
    ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ),
);
#endif // TILING_KEY_ADD_CUSTOM_TEMPLATE_H
```

`ASCENDC_TPL_ARGS_DECL` declares the template parameters and the value set of each parameter, `ASCENDC_TPL_SEL` declares the template parameter combinations, and `ASCENDC_TPL_ARGS_SEL` describes a group of instantiable kernel function template parameter combinations. The order of the template parameters in `ASCENDC_TPL_ARGS_DECL` must be consistent with the parameter passing order of `ASCENDC_TPL_ARGS_SEL` and the Host-side `ASCENDC_TPL_SEL_PARAM`, as well as the template parameter order of the kernel function.

### Step 2: Configuring the Host Side

The Host side obtains the specific values of the template parameters from the input description and shape, and then passes them to `ASCENDC_TPL_SEL_PARAM` in the declared order. Based on the passed template parameter values, this API selects a combination, automatically generates and configures the TilingKey.

| Declared Parameter | Value Passed on the Host Side | Kernel Function Template Parameter |
|---|---|---|
| `D_T_X` | `dtypeX` | `typename D_T_X` |
| `D_T_Y` | `dtypeY` | `typename D_T_Y` |
| `D_T_Z` | `dtypeZ` | `typename D_T_Z` |
| `TILE_NUM` | `tileNum` | `int TILE_NUM` |
| `IS_SPLIT` | `isSplit` | `int IS_SPLIT` |

```cpp
#include "register/op_def_registry.h"
#include "../op_kernel/add_custom_template_tiling.h"
#include "../op_kernel/tiling_key_add_custom_template.h"

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    uint32_t dtypeX = static_cast<uint32_t>(context->GetInputDesc(0)->GetDataType());
    uint32_t dtypeY = static_cast<uint32_t>(context->GetInputDesc(1)->GetDataType());
    uint32_t dtypeZ = static_cast<uint32_t>(context->GetOutputDesc(0)->GetDataType());
    uint32_t numBlocks = totalLength < 2048 ? 1 : 8;
    uint32_t tileNum = totalLength < 2048 ? 1 : 8;
    uint32_t isSplit = totalLength < 2048 ? 0 : 1;

    TilingDataTemplate* tiling = context->GetTilingData<TilingDataTemplate>();
    tiling->totalLength = totalLength;
    context->SetSimdNumBlocks(numBlocks);
    ASCENDC_TPL_SEL_PARAM(context, dtypeX, dtypeY, dtypeZ, tileNum, isSplit);

    size_t* currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;
    return ge::GRAPH_SUCCESS;
}
```

The non-split branch starts only one Block, which processes the complete Tensor. The split branch starts eight Blocks. In this way, the `numBlocks` set on the Host side stays consistent with the data range calculated based on `GetBlockNum()` in the kernel function, preventing multiple Blocks in the small-shape branch from repeatedly accessing the complete Tensor.

`ASCENDC_TPL_SEL_PARAM` matches the five passed values in the declared order of the template parameters. Each value must belong to the value set of the corresponding parameter, and the complete combination of the five values must also be covered by a certain `ASCENDC_TPL_ARGS_SEL`.

For example, when `(dtypeX, dtypeY, dtypeZ, tileNum, isSplit) = (C_DT_FLOAT16, C_DT_FLOAT16, C_DT_FLOAT16, 1, 0)` is passed, it matches the first `ASCENDC_TPL_ARGS_SEL` in the template parameter header file: all three dtypes are `C_DT_FLOAT16`, `tileNum` is `1`, and `isSplit` is `0`. If only `dtypeY` is changed to `C_DT_FLOAT`, the resulting mixed-dtype combination satisfies neither the "all `C_DT_FLOAT16`" condition of the first combination nor the "all `C_DT_FLOAT`" condition of the second combination, and therefore cannot match any combination.

### Step 3: Kernel Function Implementation

The kernel function implementation file must include the template parameter definition header file and declare the kernel function template parameters according to the types and order in `ASCENDC_TPL_ARGS_DECL`.

```cpp
#include "tiling_key_add_custom_template.h"

template <typename D_T_X, typename D_T_Y, typename D_T_Z,
          int TILE_NUM, int IS_SPLIT>
__global__ __aicore__ void add_custom_template(
    GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    // ... Other kernel function implementation code is omitted
    if constexpr (IS_SPLIT == 0) {
        // Use the unsplit implementation
    } else {
        // Use the split implementation
    }
}
```

The kernel function can use `if constexpr` to determine the template parameters and select the corresponding implementation. Branches that are not matched are not retained in the current kernel function instance. Apart from the template declaration and branch selection, the other kernel function implementation code can be written according to the actual computation logic.

### Compilation Acceleration

`--kernel-template-input` is used to filter the kernel function template instances to be compiled from the valid combinations declared by `ASCENDC_TPL_SEL`. The key names in the option are taken from the template parameters of the kernel function, such as `D_T_X` and `TILE_NUM`, not the `GM_ADDR` parameter name of the kernel function entry, nor the local variable names in the Host-side Tiling function.

Taking the template combination above as an example, `ASCENDC_TPL_SEL` is configured as follows:

```cpp
ASCENDC_TPL_SEL(
    ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ),
    ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ),
);
```

The first `ASCENDC_TPL_ARGS_SEL` fixes the three dtypes to `C_DT_FLOAT16` and combines the two values of `TILE_NUM` with the two values of `IS_SPLIT`, yielding four valid template parameter combinations; the second combination yields another four with `C_DT_FLOAT`. Therefore, when `--kernel-template-input` is not configured, each operator binary compilation task generates these eight kernel function template instances.

`--kernel-template-input` does not require listing all template parameters. Parameters that are not configured retain the valid values declared in `ASCENDC_TPL_SEL`. For example, add the following configuration to `op_kernel/CMakeLists.txt` to keep only the combination where `TILE_NUM=1` and `IS_SPLIT=0`:

```cmake
npu_op_kernel_options(ascendc_kernels AddCustomTemplate
    OPTIONS
    --kernel-template-input="TILE_NUM=1;IS_SPLIT=0"
)
```

Neither dtype combination is filtered out, so each operator binary compilation task generates the following two template instances (illustrative):

```cpp
add_custom_template<half, half, half, 1, 0>
add_custom_template<float, float, float, 1, 0>
```

When multiple values need to be retained for the same template parameter, separate them with commas. For example, configure `TILE_NUM` as `1,8` and keep `IS_SPLIT=0`:

```cmake
npu_op_kernel_options(ascendc_kernels AddCustomTemplate
    OPTIONS
    --kernel-template-input="TILE_NUM=1,8;IS_SPLIT=0"
)
```

In this case, each operator binary compilation task generates the following four template instances (illustrative):

```cpp
add_custom_template<half, half, half, 1, 0>
add_custom_template<half, half, half, 8, 0>
add_custom_template<float, float, float, 1, 0>
add_custom_template<float, float, float, 8, 0>
```

> [!NOTE]
> Different template parameters are separated by semicolons, and the configuration cannot contain spaces. For data type parameters, enter the C++ type name used by the kernel function template, for example, `C_DT_FLOAT16` corresponds to `half`, and `C_DT_FLOAT` corresponds to `float`. `--kernel-template-input` can only filter the combinations already declared by `ASCENDC_TPL_SEL` and cannot generate new combinations that are not declared. When configuring, you must retain all combinations that may be selected during the runtime phase; otherwise, no kernel function instance will be available for the corresponding scenario.

## How to Choose

**Decision criteria:**

1. Use the kernel function template when the kernel function code needs to be shared between the basic operator development mode (<<<>>>) and the aclnn engineering operator development scenario.
2. Use the kernel function template when there are many branches and the readability of branch rules needs to be improved.
3. In other scenarios, choose TilingKey or the kernel function template as needed.

## Related Documents

- [Operator Function Design](./operator_function_design.md): Determines whether the kernel function needs to be split by dtype, shape, or runtime configuration.
- [Host-Side Tiling Implementation](./host_tiling_implementation.md): Sets TilingData, TilingKey, and kernel function launch parameters.
- [Kernel Function Side Operator Implementation](./kernel_operator_implementation.md): Registers and parses different TilingData structures.
- [TILING_KEY_IS](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/TILING_KEY_IS.md): Kernel function side branch determination API.
- [ASCENDC_TPL_SEL_PARAM](../../../../../api/Utils-API/Tiling_template_programming/ASCENDC_TPL_SEL_PARAM.md): Host-side template parameter selection API.
