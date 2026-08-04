# GetArchVersion

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取当前AI处理器架构版本号。

## 函数原型

```cpp
__aicore__ inline void GetArchVersion(uint32_t& coreVersion)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| coreVersion | 输出 | AI处理器架构版本 |

## 返回值说明

无

## 约束说明

架构版本号为十六进制编码，建议使用十六进制格式（如`%x`）输出查看，或者自行转换成十六进制数。

## 调用示例

如下样例通过调用`GetArchVersion`接口获取AI处理器的架构版本号。

```cpp
uint32_t coreVersion = 0;
AscendC::GetArchVersion(coreVersion);
AscendC::printf("core version is %x", coreVersion); //需用%x将其打印成十六进制数
```
