# InitV2

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/InitV2_res.md#id1 -->

## 功能说明

HCCL客户端初始化接口。该接口默认在所有核上工作，用户也可以在调用前通过[GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md)指定其在某一个核上运行。

## 函数原型

```
__aicore__ inline void InitV2(GM_ADDR context, const void* initTiling)
```

## 参数说明

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| context | 输入 | 通信上下文，包含rankDim，rankID等相关信息。通过框架提供的获取通信上下文的接口[GetHcclContext](../HCCL-Context/GetHcclContext.md)获取context。 |
| initTiling | 输入 | 通信域初始化[Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588)的地址。[Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588)在Host侧计算得出，具体请参考[表1 Mc2InitTiling参数说明](../HCCL_Tiling/TilingData_struct.md#table4835205712588)，由框架传递到核函数（Kernel）中使用。 |

## 返回值说明

无

## 约束说明

-   本接口必须与[SetCcTilingV2](SetCcTilingV2.md)接口配合使用。
-   调用本接口时，必须使用标准C++语法定义TilingData结构体的开发方式。
-   调用本接口传入的initTiling参数，不能使用Global Memory地址，建议通过[GET\_TILING\_DATA\_WITH\_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md)接口获取TilingData的栈地址。
-   本接口不支持使用相同的context初始化多个HCCL对象。

## 调用示例

用户自定义TilingData结构体：

```
class UserCustomTilingData {
    AscendC::tiling::Mc2InitTiling initTiling;
    AscendC::tiling::Mc2CcTiling tiling;
    CustomTiling param;
};
```

在所有核上创建HCCL对象，并调用InitV2接口初始化：

```
extern "C" __global__ __aicore__ void userKernel(GM_ADDR aGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
{
    REGISTER_TILING_DEFAULT(UserCustomTilingData);
    GET_TILING_DATA_WITH_STRUCT(UserCustomTilingData, tilingData, tilingGM);

    GM_ADDR contextGM = AscendC::GetHcclContext<0>();
    Hccl hccl;
    hccl.InitV2(contextGM, &tilingData);
    hccl.SetCcTilingV2(offsetof(UserCustomTilingData, tiling));

    // 调用HCCL的Prepare、Commit、Wait、Finalize接口
}
```
