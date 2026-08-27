# Init

## 产品支持情况

### Tiling参数传入栈地址的接口

<!-- npu="950" id5 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas 训练系列产品：不支持
<!-- end id10 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Init_res.md#id2 -->

### Tiling参数传入GM地址的接口

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：不支持
<!-- end id16 -->
<!-- npu="x90" id3 -->
- Kirin X90：支持
<!-- end id3 -->
<!-- npu="9030" id4 -->
- Kirin 9030：支持
<!-- end id4 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Init_res.md#id1 -->

## 功能说明

Init主要用于对Matmul对象中的Tiling数据进行初始化，根据Tiling参数进行资源划分，Tiling参数的具体介绍请参考[Matmul Tiling侧接口](../Matmul_Tiling/Matmul_Tiling.md)。

开发者可以先通过[REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md)不传入Tiling参数对单个Matmul对象进行初始化，后续通过Init接口单独传入Tiling参数，对Matmul对象中的Tiling数据进行调整。比如，Tiling参数可变的场景下，可以通过多次调用Init来重新设置Tiling参数。

不需要Tiling变更的场景下，推荐使用[REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md)传入Tiling参数进行初始化。

## 函数原型

-   Tiling参数传入栈地址

    ```
    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    ```

-   Tiling参数传入GM地址

    ```
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe = nullptr)
    ```

## 参数说明

**表1**  Tiling参数传入栈地址接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| cubeTiling | 输入 | Matmul Tiling参数，TCubeTiling结构体定义请参见[表1 TCubeTiling结构说明](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct)。<br><br>Tiling参数可以通过host侧[GetTiling](../Matmul_Tiling/GetTiling.md)接口获取，并传递到kernel侧使用。在kernel侧调用[GET_TILING_DATA](../../../basic_api/Kernel-Tiling/GET_TILING_DATA.md)实现将Tiling参数搬运到AI Core内的栈空间中，本接口传入Tiling参数中TCubeTiling结构体的栈地址。 |
| tpipe | 输入 | Tpipe对象。 |

**表2**  Tiling参数传入GM地址接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| gmCubeTiling | 输入 | Matmul Tiling参数，该参数指向gm上的一块内存地址，其中的数据类型是TCubeTiling结构体，TCubeTiling结构体定义请参见[表1 TCubeTiling结构说明](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct)。<br><br>Tiling参数可以通过host侧[GetTiling](../Matmul_Tiling/GetTiling.md)接口获取，并传递到kernel侧使用。在kernel侧调用[GET_TILING_DATA_PTR_WITH_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_PTR_WITH_STRUCT.md)获取gm上Tiling参数的指针，本接口传入Tiling参数中TCubeTiling结构体的GM地址。 |
| tpipe | 输入 | Tpipe对象。 |

## 返回值说明

无

## 约束说明

-   Tiling参数传入栈地址的接口：

    无

-   Tiling参数传入GM地址的接口：
    -   仅支持Matmul Tiling参数的[部分常量化](GetMatmulApiTiling.md#功能说明)场景。
    -   不支持CPU域调试。

## 调用示例

-   Tiling参数传入栈地址

    ```
    GET_TILING_DATA(tilingData, tiling);
    // ...
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
    mm.Init(&(tiling.cubeTilingData));
    ```

-   Tiling参数传入GM地址
    -   纯Cube模式

        ```
        #define ASCENDC_CUBE_ONLY

        GET_TILING_DATA_PTR_WITH_STRUCT(MatmulCustomTilingData, tilingDataPtr, tiling);
        KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIC_ONLY);
        // ...
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
        mm.Init(&(tilingDataPtr->cubeTilingData));
        ```

    -   MIX模式

        ```
        GET_TILING_DATA_PTR_WITH_STRUCT(MatmulCustomTilingData, tilingDataPtr, tiling);
        KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_AIC_1_2);
        // ...
        // MIX模式下，只调用REGIST_MATMUL_OBJ接口，传入Tiling参数的GM地址，不需调用Init接口
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &(tilingDataPtr->cubeTilingData));
        ```
