# GetHcclContext

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
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

获取指定Index通信域的context（消息区）地址。

## 函数原型

```
template <uint32_t index>
__aicore__ inline __gm__ uint8_t* __gm__ GetHcclContext(void)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| index | 模板参数，用来表示要设置的通信域ID，当前只支持2个通信域，index只能为0/1。 |

## 返回值说明

指定通信域的context（消息区）地址。

## 约束说明

当前最多只支持2个通信域。

## 调用示例

```
GM_ADDR contextGM = AscendC::GetHcclContext<0>();
```
