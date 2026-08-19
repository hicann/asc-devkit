# Init

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/Init_res.md#id1 -->

## 功能说明

Init主要用于对Conv3D对象中的Tiling数据进行初始化，根据Tiling参数进行资源划分，同时获取用户声明的Pipe对象，完成内存分配。Tiling参数的具体介绍请参考[Conv3D Tiling](../Conv3D_Tiling/Conv3d_Tiling.md)。

## 函数原型

```
__aicore__ inline void Init(const void* __restrict cubeTiling)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| cubeTiling | 输入 | Conv3D对象的Tiling参数，Tiling结构体定义请参见[TConv3DApiTiling结构体](../Conv3D_Tiling/TConv3DApiTiling_struct.md)。<br><br>Tiling参数可以通过Host侧[GetTiling](../Conv3D_Tiling/GetTiling.md)接口获取，并传递到核函数（Kernel）侧使用。 |

## 返回值说明

无

## 约束说明

-   调用Init接口前必须先初始化TPipe。
-   Init接口必须在IterateAll和End接口前调用，且只能调用一次Init接口，调用顺序如下。

    ```
    Init(...);
    ...
    IterateAll(...);
    End();
    ```

## 调用示例

```
TPipe pipe;
conv3dApi.Init(&tiling);
```

