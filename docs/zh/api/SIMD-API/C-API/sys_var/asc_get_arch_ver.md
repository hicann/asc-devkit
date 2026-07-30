# asc_get_arch_ver

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

获取当前AI处理器架构版本号。

## 函数原型

```cpp
__aicore__ inline void asc_get_arch_ver(uint32_t& core_version)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| core_version | 输出 | AI处理器架构版本，数据类型uint32_t。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

在调用asc_get_arch_ver接口前，需先定义core_version,调用asc_get_arch_ver接口后core_version会变成相对应架构版本号的值。
由于硬件约束，在查看转换后的AI处理器架构版本号时需要将其打印成十六进制的数或者自行转换成十六进制的数。

## 调用示例

```cpp
uint32_t core_version = 0;// 定义AI处理器版本
asc_get_arch_ver(core_version);
printf("core version is %x", core_version);// 需用%x将其打印成十六进制的数
```