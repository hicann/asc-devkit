# Extract

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
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/Extract_res.md#id1 -->

## 功能说明

处理Sort的结果数据，输出排序后的value和index。

## 函数原型

```
template <typename T>
__aicore__ inline void Extract(const LocalTensor<T>& dstValue, const LocalTensor<uint32_t>& dstIndex, const LocalTensor<T>& sorted, const int32_t repeatTime)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 含义 |
| --- | --- | --- |
| dstValue | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要32字节对齐。 |
| dstIndex | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要32字节对齐。<br><br>此源操作数固定为uint32_t数据类型。 |
| sorted | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要32字节对齐。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| repeatTime | 输入 | 重复迭代次数，int32_t类型。<!-- npu="950" id7 --><br><br>Ascend 950PR/Ascend 950DT，每次迭代处理64个float类型或128个half类型元素。<!-- end id7 --><!-- npu="A3" id8 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，每次迭代处理64个float类型或128个half类型元素。<!-- end id8 --><!-- npu="910b" id9 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，每次迭代处理64个float类型或128个half类型元素。<!-- end id9 --><!-- npu="310p" id10 --><br><br>Atlas 推理系列产品AI Core，每次迭代完成16个Region Proposals的元素抽取并排布到16个元素里，下次迭代跳至相邻的下一组16个Region Proposals和下一组16个元素。<!-- end id10 --><br><br>取值范围：repeatTime∈[0,255]。 |

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。

## 调用示例

请参见[MrgSort](MrgSort.md)的调用示例。
