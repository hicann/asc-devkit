# 使用高阶API时配套的Tiling实现

本文属于扩展内容，介绍使用高阶API时配套的Host侧Tiling实现。Matmul、Conv等高阶API通常提供配套的Host侧Tiling类，用于根据shape、dtype计算切分参数。开发者需要把配套Tiling类的计算结果传到Kernel侧，并使用同一份数据初始化高阶API对象。

本文构造一个简化的aclnn工程化算子MatmulCustom作为示例。该算子只计算`C = A * B`，用于说明`MultiCoreMatmulTiling`和`TCubeTiling`在算子工程中的接入流程，不包含Bias、激活函数或多分支逻辑。

## 高阶API Tiling的数据流

配套流程如下：

```mermaid
flowchart LR
    A["输入shape、dtype"] --> B["Host侧调用配套Tiling类计算切分参数"]
    B --> C["写入算子TilingData"]
    C --> D["传递到Kernel侧"]
    D --> E["初始化高阶API对象并计算"]
```

配套Tiling类负责计算高阶API内部使用的多核切分和核内切分参数，算子TilingData负责把计算结果传到Kernel侧。

## 定义包含TCubeTiling的TilingData

高阶API的Tiling结构体可以通过标准C++方式或宏定义方式组织。本例为了展示`TILING_DATA_FIELD_DEF_STRUCT`的使用方法，选择宏定义方式嵌套`TCubeTiling`：

```cpp
#ifndef MATMUL_CUSTOM_TILING_H
#define MATMUL_CUSTOM_TILING_H
#include "register/tilingdata_base.h"
#include "tiling/tiling_api.h"

namespace optiling {
BEGIN_TILING_DATA_DEF(MatmulCustomTilingData)
    TILING_DATA_FIELD_DEF_STRUCT(TCubeTiling, cubeTilingData);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(MatmulCustom, MatmulCustomTilingData)
} // namespace optiling
#endif // MATMUL_CUSTOM_TILING_H
```

`TILING_DATA_FIELD_DEF_STRUCT`只能嵌套由同类宏定义的Tiling结构体，因此选择该宏时，外层`MatmulCustomTilingData`也需要使用宏定义方式。使用标准C++方式时，可以在普通结构体中直接声明`AscendC::tiling::TCubeTiling`字段，不使用上述宏。

宏定义方式会生成`SaveToBuffer()`和`GetDataSize()`等接口，Host侧计算完成后需要显式序列化TilingData。

## 在TilingFunc中调用配套Tiling类

本例固定A为`[512, 128]`、B为`[128, 512]`，输出C为`[512, 512]`。A、B使用`float16`，C使用`float`，矩阵均为ND格式。

```cpp
#include "../op_kernel/matmul_custom_tiling.h"
#include "register/op_def_registry.h"
#include "tiling/tiling_api.h"

namespace optiling {
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    constexpr int32_t M = 512;
    constexpr int32_t N = 512;
    constexpr int32_t K = 128;
    constexpr uint32_t NUM_BLOCKS = 2;

    // 使用当前芯片的平台信息初始化配套Tiling类。
    auto platform =
        platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MultiCoreMatmulTiling matmulTiling(platform);

    // 指定可使用的核数。A、B位于GM，使用ND格式和float16，
    // SetAType和SetBType的最后一个参数false表示矩阵不转置。
    matmulTiling.SetDim(NUM_BLOCKS);
    matmulTiling.SetAType(
        matmul_tiling::TPosition::GM,
        matmul_tiling::CubeFormat::ND,
        matmul_tiling::DataType::DT_FLOAT16,
        false);
    matmulTiling.SetBType(
        matmul_tiling::TPosition::GM,
        matmul_tiling::CubeFormat::ND,
        matmul_tiling::DataType::DT_FLOAT16,
        false);
    matmulTiling.SetCType(
        matmul_tiling::TPosition::GM,
        matmul_tiling::CubeFormat::ND,
        matmul_tiling::DataType::DT_FLOAT);
    // 配置原始shape、本次计算shape、Bias开关和buffer空间。
    matmulTiling.SetOrgShape(M, N, K);
    matmulTiling.SetShape(M, N, K);
    matmulTiling.EnableBias(false);
    matmulTiling.SetBufferSpace(-1, -1, -1);

    // 计算Matmul高阶API所需的多核切分和核内切分参数。
    MatmulCustomTilingData tiling;
    if (matmulTiling.GetTiling(tiling.cubeTilingData) == -1) {
        return ge::GRAPH_FAILED;
    }
    int32_t usedCoreNum = tiling.cubeTilingData.get_usedCoreNum();
    if (usedCoreNum <= 0) {
        return ge::GRAPH_FAILED;
    }

    // 将计算得到的TCubeTiling序列化到框架提供的TilingData buffer。
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(),
                        context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());

    // Kernel启动核数需要与Tiling结果中的实际使用核数一致。
    context->SetSimdNumBlocks(static_cast<uint32_t>(usedCoreNum));

    // 系统workspace供高阶API内部计算使用，并与算子自定义workspace相加。
    size_t userWorkspaceSize = 0;
    size_t systemWorkspaceSize =
        static_cast<size_t>(platform.GetLibApiWorkSpaceSize());
    size_t* currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = systemWorkspaceSize + userWorkspaceSize;
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

示例使用固定shape以突出配套Tiling类的接口。支持动态shape时，应从`TilingContext`读取`M`、`N`、`K`，并在调用`GetTiling`前校验rank、K维相等关系以及整数转换范围。

### 设置workspace

Matmul高阶API可能需要系统workspace。Host侧通过`GetLibApiWorkSpaceSize()`获取系统workspace大小，并与算子自定义workspace相加：

```cpp
size_t userWorkspaceSize = 0;
size_t systemWorkspaceSize =
    static_cast<size_t>(platform.GetLibApiWorkSpaceSize());
size_t* currentWorkspace = context->GetWorkspaceSizes(1);
currentWorkspace[0] = systemWorkspaceSize + userWorkspaceSize;
```

即使算子没有自定义临时数据，也需要检查高阶API是否使用系统workspace，不能直接把workspace设为0。

### 校验输入条件

实际算子还需要在调用`GetTiling`前校验：

- A、B是否为二维矩阵。
- A的K维与B的K维是否相等。
- 输入dtype和format是否与`SetAType`、`SetBType`配置一致。
- 输出shape、dtype和format是否与`SetCType`配置一致。
- `M`、`N`、`K`转换到`int32_t`前是否超出范围。

校验失败时返回`ge::GRAPH_FAILED`，不要继续使用无效shape计算Tiling。

## Kernel侧使用TCubeTiling

Kernel入口解析TilingData，并用其中的`TCubeTiling`注册Matmul对象：

```cpp
#include "kernel_operator.h"
#define ASCENDC_CUBE_ONLY
#include "lib/matmul_intf.h"

using namespace matmul;

extern "C" __global__ __aicore__ void matmul_custom(
    GM_ADDR a, GM_ADDR b, GM_ADDR c,
    GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);

    AscendC::TPipe pipe;
    AscendC::GlobalTensor<half> aGlobal;
    AscendC::GlobalTensor<half> bGlobal;
    AscendC::GlobalTensor<float> cGlobal;
    aGlobal.SetGlobalBuffer(
        reinterpret_cast<__gm__ half*>(a),
        tilingData.cubeTilingData.M * tilingData.cubeTilingData.Ka);
    bGlobal.SetGlobalBuffer(
        reinterpret_cast<__gm__ half*>(b),
        tilingData.cubeTilingData.Kb * tilingData.cubeTilingData.N);
    cGlobal.SetGlobalBuffer(
        reinterpret_cast<__gm__ float*>(c),
        tilingData.cubeTilingData.M * tilingData.cubeTilingData.N);

    // 模板参数中的位置、格式和数据类型需要与Host侧Tiling配置一致。
    Matmul<
        MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half>,
        MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half>,
        MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>> mm;

    // 使用系统workspace和TCubeTiling注册Matmul对象。
    REGIST_MATMUL_OBJ(
        &pipe, GetSysWorkSpacePtr(), mm,
        &tilingData.cubeTilingData);

    // 设置原始矩阵的M、N、Ka和Kb。
    mm.SetOrgShape(
        tilingData.cubeTilingData.M,
        tilingData.cubeTilingData.N,
        tilingData.cubeTilingData.Ka,
        tilingData.cubeTilingData.Kb);

    uint64_t blockIdx = static_cast<uint64_t>(AscendC::GetBlockIdx());
    uint64_t aOffset =
        blockIdx * tilingData.cubeTilingData.singleCoreM *
        tilingData.cubeTilingData.Ka;
    uint64_t cOffset =
        blockIdx * tilingData.cubeTilingData.singleCoreM *
        tilingData.cubeTilingData.N;

    mm.SetTensorA(aGlobal[aOffset], false);
    mm.SetTensorB(bGlobal[0], false);
    mm.IterateAll(cGlobal[cOffset]);
    mm.End();
}
```

Host侧`SetAType`、`SetBType`和`SetCType`配置的位置、format、dtype，需要与Kernel侧的`MatmulType`模板参数保持一致。本例按M轴分核：不同Block读取A的不同行，复用完整B矩阵，并把结果写入C的对应行。

## 相关文档

- [Host侧Tiling实现](./基本流程.md)：普通TilingData和Tiling函数的基本流程。
- [Matmul-Tiling类](../../../../../../api/SIMD-API/高阶API/矩阵计算/Matmul-Tiling类/Matmul-Tiling类.md)：配套Tiling类接口说明。
