# 多分支策略

本文属于扩展内容，介绍同一个算子需要根据dtype、shape或属性执行不同计算流程时，如何使用TilingKey或核函数（Kernel）模板组织多个核函数（Kernel）分支。TilingKey使用数字标识分支，适合分支数量较少、含义稳定的场景；核函数（Kernel）模板使用具名参数描述分支，适合由多个维度共同决定分支的场景。

本文分别说明两种方式的使用原因、Host与核函数（Kernel）侧的对应关系以及接入步骤。

## 为什么需要多分支

普通的运行时分支会将所有处理逻辑保留在同一个核函数（Kernel）中：

```cpp
if (condition) {
    ProcessA();
} else {
    ProcessB();
}
```

单次执行虽然只会进入其中一个分支，但两个分支的代码都属于同一个核函数（Kernel）。分支代码较大时，会增加指令缓存压力和Scalar侧的控制开销。

使用TilingKey后，可以将分支条件变为编译期常量：

```cpp
if (TILING_KEY_IS(1)) {
    ProcessA();
} else if (TILING_KEY_IS(2)) {
    ProcessB();
}
```

编译时框架会自动识别TilingKey并编译多个核函数（Kernel）入口函数，将条件判断进行**常量折叠**，消除运行时的分支判断开销。

增加分支前，还需要判断差异是否确实需要独立核函数（Kernel）实现：

| 分支维度 | Host侧判断依据 | 典型差异 |
|---|---|---|
| dtype | `context->GetInputDesc()`、`context->GetOutputDesc()` | 计算类型、搬运粒度、精度策略 |
| shape | `context->GetInputShape()` | 小shape直算、大shape分块、尾块处理 |
| 属性 | `context->GetAttrs()` | 算法模式、维度、开关 |
| format | 输入输出描述 | 数据组织和搬运方式 |

如果不同场景只影响Tile大小、Block数量等运行参数，而核函数（Kernel）计算流程不变，只需要在TilingData中传递不同参数，不必增加核函数（Kernel）分支。只有计算流程、数据类型实例或代码体积确实不同，才需要使用TilingKey或核函数（Kernel）模板。

## 使用TilingKey选择分支

TilingKey使用非负整数标识核函数（Kernel）分支。Host侧通过`SetTilingKey`选择本次执行的key，核函数（Kernel）侧通过`TILING_KEY_IS(key)`声明与该key对应的分支。

### Host侧设置TilingKey

TilingKey只负责选择核函数（Kernel）计算分支，并不要求每个分支使用不同的TilingData。多个分支需要的运行参数结构相同时，可以共用一个TilingData。下面根据输入数据量选择不同分支，并通过同一个`TilingData`传递参数：

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

### 核函数（Kernel）侧声明分支

在核函数（Kernel）中通过`TILING_KEY_IS`选择计算分支：

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

### 为特定分支使用不同TilingData

只有某个分支的字段集合或布局与默认结构体不同时，才需要为该TilingKey注册单独的TilingData类型。例如，key 1使用默认的`TilingData`，key 2使用`TilingData1`：

```cpp
REGISTER_TILING_DEFAULT(optiling::TilingData);
REGISTER_TILING_FOR_TILINGKEY("(TILING_KEY_VAR == 2)", optiling::TilingData1);

if (TILING_KEY_IS(1)) {
    GET_TILING_DATA(tilingData, tiling);
    // 使用默认的TilingData
} else if (TILING_KEY_IS(2)) {
    GET_TILING_DATA_WITH_STRUCT(optiling::TilingData1, tilingData, tiling);
    // 使用key 2对应的TilingData1
}
```

使用[REGISTER_TILING_FOR_TILINGKEY](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_FOR_TILINGKEY.md)时，必须同时通过[REGISTER_TILING_DEFAULT](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_DEFAULT.md)注册默认结构体。Host侧也需要在key 2分支中调用`GetTilingData<TilingData1>()`写入数据，保证两侧类型和字段布局一致。

### 只编译指定TilingKey<a id="compile-selected-tiling-keys"></a>

开发调试时，可以通过`--tiling_key`仅编译指定key，加速编译过程：

```cmake
npu_op_kernel_options(ascendc_kernels MyOp
    COMPUTE_UNIT Ascendxxxyy
    OPTIONS --tiling_key=1)
```

按系列配置时可将`COMPUTE_UNIT`替换为`SOC_SERIES`，例如`SOC_SERIES Ascendxxx`。同一次调用中两者不能同时使用。

关于编译选项的完整说明见[编译与部署基本流程](../compilation_and_deployment/basic_process.md#custom-compile-options)。

## 使用核函数（Kernel）模板组织分支<a name="使用kernel模板组织分支"></a>

### 为什么需要核函数（Kernel）模板

TilingKey使用数字标识分支。分支数量较多或同时受dtype、Tile数量、布尔开关等多个条件影响时，仅通过数字难以直接看出每个分支的含义，Host与核函数（Kernel）侧的映射关系也更难维护。

核函数（Kernel）模板将这些分支维度声明为具名模板参数，例如`D_T_X`、`TILE_NUM`和`IS_SPLIT`。模板参数组合由头文件统一约束，Host侧通过`ASCENDC_TPL_SEL_PARAM`选择组合，核函数（Kernel）入口直接接收对应的编译期类型和常量。

### 步骤一：定义模板参数

通过`ASCENDC_TPL_ARGS_DECL`声明模板参数，通过`ASCENDC_TPL_SEL`声明允许生成的参数组合。

核函数（Kernel）模板编程常用宏如下，具体API参考见[模板参数定义](../../../../../api/Utils-API/Tiling_template_programming/template_params_definition.md)：

| 宏类别 | 宏名称 | 作用 |
|---|---|---|
| 声明入口 | `ASCENDC_TPL_ARGS_DECL` | 定义模板参数列表及顺序。 |
| 数据类型参数 | `ASCENDC_TPL_DATATYPE_DECL` | 声明数据类型模板参数及可选值。 |
| 整数参数 | `ASCENDC_TPL_UINT_DECL` | 声明无符号整数模板参数、编码位宽和取值范围。 |
| 布尔参数 | `ASCENDC_TPL_BOOL_DECL` | 声明布尔模板参数及可选值。 |
| 组合入口 | `ASCENDC_TPL_SEL` | 定义允许生成的模板参数组合列表。 |
| 单组组合 | `ASCENDC_TPL_ARGS_SEL` | 定义一组模板参数组合。 |
| 组合约束 | `ASCENDC_TPL_DATATYPE_SEL`、`ASCENDC_TPL_UINT_SEL`、`ASCENDC_TPL_BOOL_SEL` | 约束一组组合中各参数的取值。 |
| TilingData结构体 | `ASCENDC_TPL_TILING_STRUCT_SEL` | 为当前`ASCENDC_TPL_ARGS_SEL`模板参数组合指定TilingData结构体，不作为核函数（Kernel）模板参数传入。 |

在算子工程的op_kernel目录下，新增头文件定义模板参数和模板参数组合。本示例中头文件命名为`tiling_key_add_custom_template.h`。下面以dtype、Tile数量和切分开关为例：

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

`ASCENDC_TPL_ARGS_DECL`声明模板参数及每个参数的取值集合，`ASCENDC_TPL_SEL`声明模板参数组合，`ASCENDC_TPL_ARGS_SEL`都描述一组可实例化的核函数（Kernel）模板参数组合。`ASCENDC_TPL_ARGS_DECL`中各模板参数的排列顺序，需要与`ASCENDC_TPL_ARGS_SEL`、Host侧`ASCENDC_TPL_SEL_PARAM`的传参顺序以及核函数（Kernel）的模板参数顺序保持一致。

### 步骤二：Host侧配置

Host侧从输入描述和shape得到模板参数的具体值，然后按声明顺序传给`ASCENDC_TPL_SEL_PARAM`。该接口根据传入的模板参数值选择组合自动生成并配置TilingKey。

| 声明参数 | Host侧传入值 | 核函数（Kernel）模板参数 |
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

未切分分支只启动1个Block，由该Block处理完整Tensor；切分分支启动8个Block。这样，Host侧设置的`numBlocks`与核函数（Kernel）中基于`GetBlockNum()`计算的数据范围保持一致，避免小Shape分支的多个Block重复访问完整Tensor。

`ASCENDC_TPL_SEL_PARAM`会按照模板参数的声明顺序，同时匹配传入的五个值。每个值都需要属于对应参数的取值集合，五个值组成的完整组合还必须被某个`ASCENDC_TPL_ARGS_SEL`覆盖。

例如，传入`(dtypeX, dtypeY, dtypeZ, tileNum, isSplit) = (C_DT_FLOAT16, C_DT_FLOAT16, C_DT_FLOAT16, 1, 0)`时，会命中模板参数头文件中的第一个`ASCENDC_TPL_ARGS_SEL`：三个dtype均为`C_DT_FLOAT16`，`tileNum`取值为`1`，`isSplit`取值为`0`。如果只将`dtypeY`改为`C_DT_FLOAT`，得到的混合dtype组合既不满足第一个组合的“全部为`C_DT_FLOAT16`”，也不满足第二个组合的“全部为`C_DT_FLOAT`”，因此无法命中任何组合。

### 步骤三：核函数（Kernel）侧实现

核函数（Kernel）实现文件需要包含模板参数定义头文件，并按照`ASCENDC_TPL_ARGS_DECL`中的类型和顺序声明核函数（Kernel）模板参数。

```cpp
#include "tiling_key_add_custom_template.h"

template <typename D_T_X, typename D_T_Y, typename D_T_Z,
          int TILE_NUM, int IS_SPLIT>
__global__ __aicore__ void add_custom_template(
    GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    // ... 省略其他核函数（Kernel）实现代码
    if constexpr (IS_SPLIT == 0) {
        // 使用未切分实现
    } else {
        // 使用切分实现
    }
}
```

核函数（Kernel）可以通过`if constexpr`判断模板参数并选择对应实现，未命中的分支不会保留在当前核函数（Kernel）实例中。除模板声明和分支选择外，其他核函数（Kernel）实现代码按实际计算逻辑编写即可。

### 编译加速

`--kernel-template-input`用于从`ASCENDC_TPL_SEL`声明的合法组合中筛选需要编译的核函数（Kernel）模板实例。选项中的键名取自核函数（Kernel）的模板参数，例如`D_T_X`、`TILE_NUM`，不是核函数（Kernel）入口的`GM_ADDR`参数名，也不是Host侧Tiling函数中的局部变量名。

以上文的模板组合为例，`ASCENDC_TPL_SEL`配置如下：

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

第一个`ASCENDC_TPL_ARGS_SEL`固定三个dtype为`C_DT_FLOAT16`，并将`TILE_NUM`的两个取值与`IS_SPLIT`的两个取值组合，得到4个合法模板参数组合；第二个组合以`C_DT_FLOAT`得到另外4个。因此，不配置`--kernel-template-input`时，每个算子二进制编译任务会生成这8个核函数（Kernel）模板实例。

`--kernel-template-input`不要求列出所有模板参数。未配置的参数会保留`ASCENDC_TPL_SEL`中声明的合法取值。例如，在`op_kernel/CMakeLists.txt`中增加以下配置，只保留`TILE_NUM=1`且`IS_SPLIT=0`的组合：

```cmake
npu_op_kernel_options(ascendc_kernels AddCustomTemplate
    OPTIONS
    --kernel-template-input="TILE_NUM=1;IS_SPLIT=0"
)
```

两个dtype组合均未被过滤，因此每个算子二进制编译任务会生成以下两个模板实例（示意）：

```cpp
add_custom_template<half, half, half, 1, 0>
add_custom_template<float, float, float, 1, 0>
```

同一模板参数需要保留多个值时，使用英文逗号分隔。例如，将`TILE_NUM`配置为`1,8`，并保持`IS_SPLIT=0`：

```cmake
npu_op_kernel_options(ascendc_kernels AddCustomTemplate
    OPTIONS
    --kernel-template-input="TILE_NUM=1,8;IS_SPLIT=0"
)
```

此时每个算子二进制编译任务会生成以下4个模板实例（示意）：

```cpp
add_custom_template<half, half, half, 1, 0>
add_custom_template<half, half, half, 8, 0>
add_custom_template<float, float, float, 1, 0>
add_custom_template<float, float, float, 8, 0>
```

> [!NOTE]
> 不同模板参数之间使用英文分号分隔，配置中不能包含空格。数据类型参数填写核函数（Kernel）模板使用的C++类型名，例如`C_DT_FLOAT16`对应`half`，`C_DT_FLOAT`对应`float`。`--kernel-template-input`只能筛选`ASCENDC_TPL_SEL`已经声明的组合，不能生成未声明的新组合。配置时必须保留运行阶段可能选中的全部组合，否则对应场景将没有可用的核函数（Kernel）实例。

## 如何选择

**决策准则：**

1. 核函数（Kernel）代码需要在基础算子开发方式（<<<>>>）和aclnn工程化算子开发场景共用时，使用核函数（Kernel）模板方式。
2. 分支较多、需要提升分支规则可读性时，使用核函数（Kernel）模板方式。
3. 其他场景，按需选择TilingKey或核函数（Kernel）模板。

## 相关文档

- [算子功能设计](./operator_function_design.md)：判断是否需要按dtype、shape或运行配置拆分核函数（Kernel）。
- [Host侧Tiling实现](./host_tiling_implementation.md)：设置TilingData、TilingKey和核函数（Kernel）启动参数。
- [核函数（Kernel）侧算子实现](./kernel_operator_implementation.md)：注册和解析不同TilingData结构体。
- [TILING_KEY_IS](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/TILING_KEY_IS.md)：核函数（Kernel）侧分支判断接口。
- [ASCENDC_TPL_SEL_PARAM](../../../../../api/Utils-API/Tiling_template_programming/ASCENDC_TPL_SEL_PARAM.md)：Host侧模板参数选择接口。
