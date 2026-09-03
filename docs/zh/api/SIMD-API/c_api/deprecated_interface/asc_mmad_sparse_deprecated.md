# asc_mmad_sparse（废弃）

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/cube_compute/cube_compute.h"`。

**入参`unit_flag`类型为`uint8_t`的`asc_mmad_sparse`接口和`asc_mmad_sparse_sync`接口已废弃。请使用入参`unit_flag_mode`类型为`asc_unit_flag_mode`的[asc_mmad_sparse](../cube_compute/asc_mmad_sparse.md)接口替代；原`asc_mmad_sparse_sync`接口的同步功能请通过新`asc_mmad_sparse`接口和`asc_sync()`实现，具体请参见[asc_sync](../sync/asc_sync.md)。**

本接口完成稀疏矩阵乘加操作。传入的左矩阵A为稀疏矩阵，右矩阵B为稠密矩阵；矩阵A在计算时完成稠密化，矩阵B需在输入数据准备阶段完成稠密化。

本接口为矩阵计算接口，仅在AIC上生效。

## 函数原型

```c
__aicore__ inline void asc_mmad_sparse(__cc__ int32_t* c,
                                       __ca__ int8_t* a,
                                       __cb__ int8_t* b,
                                       uint16_t m,
                                       uint16_t k,
                                       uint16_t n,
                                       uint8_t unit_flag,
                                       bool c_matrix_source,
                                       bool c_matrix_init_val)

__aicore__ inline void asc_mmad_sparse_sync(__cc__ int32_t* c,
                                            __ca__ int8_t* a,
                                            __cb__ int8_t* b,
                                            uint16_t m,
                                            uint16_t k,
                                            uint16_t n,
                                            uint8_t unit_flag,
                                            bool c_matrix_source,
                                            bool c_matrix_init_val)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| c | 输出 | 结果矩阵C在L0C Buffer中的起始地址，需按照1024字节对齐。 |
| a | 输入 | 左矩阵A在L0A Buffer中的起始地址，需按照512字节对齐。 |
| b | 输入 | 右矩阵B在L0B Buffer中的起始地址，需按照512字节对齐。 |
| m | 输入 | 左矩阵A和结果矩阵C的M维大小，取值范围为[0, 4095]。 |
| k | 输入 | 左矩阵A和右矩阵B的K维大小，取值范围为[0, 4095]。 |
| n | 输入 | 右矩阵B和结果矩阵C的N维大小，取值范围为[0, 4095]。 |
| unit_flag | 输入 | UnitFlag控制参数。0表示关闭；2表示开启且执行后保持单元标志位；3表示开启且执行后更新单元标志位。 |
| c_matrix_source | 输入 | 当`c_matrix_init_val`为false时，配置矩阵C的初始值是否来自BiasTable Buffer。 |
| c_matrix_init_val | 输入 | 配置是否将矩阵C的初始值设置为0。 |

## 返回值说明

无

## 流水类型

PIPE_M

## 约束说明

- 本接口仅在AIC上生效，在AIV上调用将直接返回。
- 原始稀疏矩阵B每4个元素中最多包含2个非零元素；如果存在3个或更多非零元素，则仅使用前2个非零元素。
- `m`、`k`、`n`中的任意一个值为0时，接口将被视为NOP（空操作）。
- 稀疏矩阵的数据准备、稠密算法及其他约束请参见[asc_mmad_sparse](../cube_compute/asc_mmad_sparse.md)。
