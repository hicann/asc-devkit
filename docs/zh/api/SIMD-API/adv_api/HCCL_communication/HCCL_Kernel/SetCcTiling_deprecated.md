# SetCcTiling（废弃）

> [!NOTE]说明
>该接口废弃，并将在后续版本移除，请不要使用该接口。请使用[SetCcTilingV2](SetCcTilingV2.md)接口设置通信算法的Tiling地址。

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/SetCcTiling_deprecated_res.md#id1 -->

## 功能说明

用于设置HCCL客户端通信算法的Tiling地址。

## 函数原型

```
__aicore__ inline int32_t SetCcTiling(__gm__ void* ccOpTilingData)
```

## 参数说明

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ccOpTilingData | 输入 | 通信算法的[Mc2CcTiling](../HCCL_Tiling/TilingData_struct.md#table678914014562)参数的地址。Mc2CcTiling在Host侧计算得出，具体请参考[表2 Mc2CcTiling参数说明](../HCCL_Tiling/TilingData_struct.md#table678914014562)，由框架传递到核函数（Kernel）中使用。 |

## 返回值说明

-   HCCL\_SUCCESS，表示成功。
-   HCCL\_FAILED，表示失败。

## 约束说明

-   参数相同的同一种通信算法在调用Prepare接口前只需要调用一次本接口，否则需要多次调用本接口。
-   同一种通信算法只支持设置一个ccOpTilingData地址；对于同一种通信算法，重复调用本接口会覆盖该通信算法的ccOpTilingData地址。
-   若调用本接口，必须与传initTiling地址的[Init](Init_deprecated.md)接口配合使用，且Init接口在本接口前被调用。
-   若调用本接口，必须使用标准C++语法定义TilingData结构体的开发方式。
