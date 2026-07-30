# asc_create_mask

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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

根据入参生成相应的掩码寄存器。生成的掩码用于约束RegTensor的计算范围，将掩码作为参数传入其他函数，则只有掩码为True所对应的元素才会参与计算。

## 函数原型

  ```cpp
  asc_create_mask_b8(pat_mode)
  asc_create_mask_b16(pat_mode)
  asc_create_mask_b32(pat_mode)
  ```

## 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| pat_mode  | 输入 | 掩码寄存器的创建模式，pat_mode表示选取元素的范围，定义如下：<pre><code>PAT_ALL, // 所有元素都参与计算<br>PAT_VL1, // 最低位的1个元素参与计算<br>PAT_VL2, // 最低位的2个元素参与计算<br>PAT_VL3, // 最低位的3个元素参与计算<br>PAT_VL4, // 最低位的4个元素参与计算<br>PAT_VL8, // 最低位的8个元素参与计算<br>PAT_VL16, // 最低位的16个元素参与计算<br>PAT_VL32, // 最低位的32个元素参与计算<br>PAT_VL64, // 最低位的64个元素参与计算<br>PAT_VL128, // 最低位的128个元素参与计算<br>PAT_M3, // 下标为3的倍数的元素参与计算<br>PAT_M4, // 下标为4的倍数的元素参与计算<br>PAT_H, // 低一半的元素参与计算<br>PAT_Q, // 低四分之一的元素参与计算<br>PAT_ALLF = 15 // 任何元素都不参与计算</pre> |

## 返回值说明

根据上述提供的模式生成相应的掩码寄存器，数据类型为vector_bool，其位宽固定为VL/8。如果是调用 `asc_create_mask_b8`生成的掩码，返回值中一个bit位置对应一个元素；如果生成的是非b8类型的掩码，则返回值中N（N=数据位宽/8）个bit对应一个元素，只有低比特位有效。

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

见[掩码寄存器调用示例](../reg_data_types/data_type_definition.md#调用示例-2)。
