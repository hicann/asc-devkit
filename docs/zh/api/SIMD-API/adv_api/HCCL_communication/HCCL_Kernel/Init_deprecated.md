# Init（废弃）

> [!NOTE]说明
>该接口废弃，并将在后续版本移除，请不要使用该接口。请使用[InitV2](InitV2.md)接口进行初始化。

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/Init_deprecated_res.md#id1 -->

## 功能说明

HCCL客户端初始化接口。该接口默认在所有核上工作，用户也可以在调用前通过[GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md)指定其在某一个核上运行。

## 函数原型

```
__aicore__ inline void Init(GM_ADDR context, __gm__ void* initTiling = nullptr)
```

## 参数说明

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| context | 输入 | 通信上下文，包含rankDim，rankID等相关信息。 |
| initTiling | 输入 | 可选参数，通信域初始化[Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588)的地址。[Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588)在Host侧计算得出，具体请参考[表1 Mc2InitTiling参数说明](../HCCL_Tiling/TilingData_struct.md#table4835205712588)，由框架传递到核函数（Kernel）中使用，完整示例请参考[8.13.1.2-调用示例](HCCL_template_params.md#调用示例)。 |

## 返回值说明

无

## 约束说明

-   若调用本接口时传入initTiling参数，则必须与[SetCcTiling](SetCcTiling_deprecated.md)接口配合使用。
-   同一个程序中不能同时调用传入可选参数initTiling的接口和不传入可选参数initTiling的接口，推荐使用传入initTiling参数的调用方式。
-   若调用本接口时传入initTiling参数，必须使用标准C++语法定义TilingData结构体的开发方式。
-   本接口不支持使用相同的context初始化多个HCCL对象。每个HCCL对象都应获取其自身的通信上下文。
