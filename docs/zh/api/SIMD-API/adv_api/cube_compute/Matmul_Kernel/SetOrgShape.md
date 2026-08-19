# SetOrgShape

## 产品支持情况

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetOrgShape_res.md#id1 -->

## 功能说明

设置Matmul计算原始完整的形状M、N、K，单位为元素个数。用于运行时修改shape，比如复用同一个Matmul对象，从不同的矩阵块取数据计算。

## 函数原型

```
__aicore__ inline void SetOrgShape(int orgM, int orgN, int orgK)
```

```
__aicore__ inline void SetOrgShape(int orgM, int orgN, int orgKa, int orgKb, int orgKc = 0)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| orgM | 输入 | 设置原始完整的形状M大小，单位为元素。<!-- npu="950" id8 --><br><br>对于Ascend 950PR/Ascend 950DT，在使用[MDL模板](MatmulConfig.md)创建的Matmul对象调用本接口时，该参数用于设置GM或L1上完整的形状M大小，单位为元素。<!-- end id8 --> |
| orgN | 输入 | 设置原始完整的形状N大小，单位为元素。<!-- npu="950" id10 --><br><br>对于Ascend 950PR/Ascend 950DT，在使用[MDL模板](MatmulConfig.md)创建的Matmul对象调用本接口时，该参数用于设置GM或L1上完整的形状N大小，单位为元素。<!-- end id10 --> |
| orgK | 输入 | 设置原始完整的形状K大小，单位为元素。原始完整形状Ka=Kb时可设置。<!-- npu="950" id11 --><br><br>对于Ascend 950PR/Ascend 950DT，在使用[MDL模板](MatmulConfig.md)创建的Matmul对象调用本接口时，该参数用于设置GM或L1上完整的形状K大小，单位为元素。原始完整形状Ka=Kb时可设置。<!-- end id11 --> |
| orgKa | 输入 | 设置矩阵A原始完整的形状Ka大小，单位为元素。<!-- npu="950" id12 --><br><br>对于Ascend 950PR/Ascend 950DT，在使用[MDL模板](MatmulConfig.md)创建的Matmul对象调用本接口时，该参数用于设置GM或L1上完整的形状Ka大小，单位为元素。<!-- end id12 --> |
| orgKb | 输入 | 设置矩阵B原始完整的形状Kb大小，单位为元素。<!-- npu="950" id13 --><br><br>对于Ascend 950PR/Ascend 950DT，在使用[MDL模板](MatmulConfig.md)创建的Matmul对象调用本接口时，该参数用于设置GM或L1上完整的形状Kb大小，单位为元素。<!-- end id13 --> |
| orgKc | 输入 | 设置输出C矩阵的N，单位为元素。需要输入B矩阵的N和输出C矩阵的N不一样时可设置，默认为0（即使用B矩阵的N，不进行修改）。 |

<!-- npu="950" id14 -->
> [!CAUTION]注意
>-   对于Ascend 950PR/Ascend 950DT上使用[MDL模板](MatmulConfig.md)创建的Matmul对象，L1上数据的形状与Tiling侧接口[SetOrgShape](../Matmul_Tiling/SetOrgShape.md)中的orgMIn/orgNIn/orgKIn/orgKaIn/orgKbIn一致时，不必须调用此接口。
>-   对于Ascend 950PR/Ascend 950DT上使用[MDL模板](MatmulConfig.md)创建的Matmul对象，L1上数据的形状与Tiling侧接口[SetOrgShape](../Matmul_Tiling/SetOrgShape.md)中的orgMIn/orgNIn/orgKIn/orgKaIn/orgKbIn不一致时，必须调用本接口指定GM/L1上的orgM/orgN/orgK/orgKa/orgKb。
>    例如，使用[MDL模板](MatmulConfig.md)时，输入矩阵A在L1、输入矩阵B在GM的场景，L1上A的形状大小与Tiling侧原始的orgMIn/orgKIn/orgKaIn不一致时，调用SetOrgShape\(orgM, orgN, orgK\)/SetOrgShape\(orgM, orgN, orgKa, orgKb\)接口指定L1上A矩阵相关参数orgM/orgK/orgKa。
<!-- end id14 -->

## 返回值说明

无

## 约束说明

本接口需要在SetTensorA接口、SetTensorB接口、SetBias接口及SetSingleShape接口前调用。

## 调用示例

-   设置矩阵原始完整的形状

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gm_a);
    mm.SetTensorB(gm_b);
    mm.SetBias(gm_bias);
    mm.IterateAll(gm_c);
    //  复用mm对象
    mm.SetOrgShape(orgM, orgN, orgK);
    mm.SetTensorA(gm_a1);
    mm.SetTensorB(gm_b1);
    mm.SetBias(gm_bias1);
    mm.IterateAll(gm_c1);
    ```

<!-- npu="950" id9 -->
-   对于Ascend 950PR/Ascend 950DT上使用[MDL模板](MatmulConfig.md)的Matmul对象，设置GM或L1上完整的形状

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    for (int m = 0; m < mIter_; m++) {
         for (int n = 0; n < nIter_; n++) {
              for (int k = 0; k < kIter_; k++) {
                   // 复用mm，指定A在L1和B在GM上的shape
                   mm.SetOrgShape(alignedSingleM, tiling.N, alignedSingleK, tiling.Kb, tiling.N);
                   mm.SetSingleShape(curBaseM, curBaseN, curBaseK);
                   mm.SetTensorA(tscm_a[offset_a]); // Set aMatrix tscm input
                   mm.SetTensorB(gm_b[offset_b]);
                   mm.SetBias(gm_bias[offset_bias]);
                   mm.Iterate(k != 0);
                }
                matmulObj.GetTensorC(gm_c[offset_c]);
         }
    }
    ```
<!-- end id9 -->
