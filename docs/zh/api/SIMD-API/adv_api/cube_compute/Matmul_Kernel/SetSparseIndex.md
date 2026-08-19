# SetSparseIndex

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetSparseIndex_res.md#id1 -->

## 功能说明

设置稀疏矩阵[稠密化过程](../../../basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md#功能说明)生成的索引矩阵。

索引矩阵在稠密化中的作用请参考[MmadWithSparse](../../../basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md#section618mcpsimp)。

## 函数原型

```
__aicore__ inline void SetSparseIndex(const GlobalTensor<uint8_t>& indexGlobal)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| indexGlobal | 输入 | 索引矩阵在Global Memory上的首地址，类型为GlobalTensor。<br><br>索引矩阵的数据类型为uint2，需要由用户拼成uint8的数据类型，再传入本接口。索引矩阵的Format格式只支持NZ格式。 |

## 返回值说明

无

## 约束说明

-   索引矩阵的Format格式要求为NZ格式。
-   本接口仅支持在纯Cube模式（只有矩阵计算）且[MDL模板](MatmulConfig.md#mdl-template)的场景使用。

## 调用示例

```
#define ASCENDC_CUBE_ONLY // 开启纯Cube模式(只有矩阵计算)
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetSparseIndex(gm_index); //设置索引矩阵
mm.SetBias(gm_bias);
mm.IterateAll(gm_c);
```
