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
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id7 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id4 -->
- Atlas 训练系列产品：支持
<!-- end id4 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetArchVersion_res.md#id1 -->


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
| coreVersion | 输出 | AI处理器架构版本。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

如下样例通过调用`GetArchVersion`接口获取AI处理器的架构版本号。

```cpp
uint32_t coreVersion = 0;
AscendC::GetArchVersion(coreVersion);
AscendC::printf("core version is %u", coreVersion);
```

不同型号服务器有不同的架构版本号取值，如下所示：
<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT：3510
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：2201
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：2201
<!-- end id10 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品：3002
<!-- end id13 -->
<!-- npu="310p" id12 -->
- Atlas 推理系列产品AI Core：2002
<!-- end id12 -->
<!-- npu="910" id11 -->
- Atlas 训练系列产品：1001
<!-- end id11 -->
