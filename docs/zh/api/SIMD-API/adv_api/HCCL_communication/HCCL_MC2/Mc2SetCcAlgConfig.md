# Mc2SetCcAlgConfig

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
- Atlas推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcAlgConfig_res.md#id1 -->

## 功能说明

设置MC2通信任务的通信算法。若不调用本接口或传入空字符串，接口内部会通过算法选择器自动选择算法。

## 函数原型

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcAlgConfig(void* ccArgs, const char* algConfig)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ccArgs | 输入 | [Mc2GetCcArgs](Mc2GetCcArgs.md)返回的MC2通信参数对象指针。 |
| algConfig | 输入 | 通信算法配置，以'\0'结尾的字符串，最大有效长度为127字节。支持的配置格式和取值请参考[SetAlgConfig](../HCCL_Tiling/SetAlgConfig.md)中`algConfig`参数的说明。 |

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败，例如参数为空指针或算法配置长度非法。

## 约束说明

-   算法配置长度必须小于128字节。
-   使用本接口设置的通信算法必须与指定的通信任务类型匹配，具体配置请参考[SetAlgConfig](../HCCL_Tiling/SetAlgConfig.md)中`algConfig`参数的说明。

## 调用示例

```cpp
char algConfig[] = "sole[mesh]";
Mc2Result ret = Mc2SetCcAlgConfig(ccArgs, algConfig);
```
