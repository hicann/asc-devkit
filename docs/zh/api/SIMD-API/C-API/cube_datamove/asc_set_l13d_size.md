# asc_set_l13d_size

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

设置[asc_copy_l12l0a](asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md)的3D格式搬运接口在L1 Buffer的边界值。

## 函数原型

```cpp
__aicore__ inline void asc_set_l13d_size(uint64_t value)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| value | 输入 | 用于设置asc_copy_l12l0a/asc_copy_l12l0b的3D格式搬运接口在L1 Buffer的边界值，单位是字节。非0场景的边界值不能小于1024。<br>如果value设置为0，则表示无边界，asc_copy_l12l0a/asc_copy_l12l0b接口可使用整个L1 Buffer。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 需配合asc_copy_l12l0a/asc_copy_l12l0b的3D格式搬运接口使用。
- 如果使用asc_set_l13d_size接口设置了边界值，asc_copy_l12l0a/asc_copy_l12l0b的3D格式搬运接口的L1 Buffer初始地址要在设置的边界内（value）。
- 如果asc_copy_l12l0a/asc_copy_l12l0b的3D格式搬运接口的源操作数在L1 Buffer上的地址超出设置的边界值（value），则会从L1 Buffer起始地址开始读取数据。
- 操作数地址对齐约束请参考[通用地址对齐约束](../通用说明和约束.md#通用地址对齐约束)。



## 调用示例

```cpp
uint64_t value = 1024;
asc_set_l13d_size(value);    // 设置L1 Buffer的边界值为1024（单位：字节）
```
