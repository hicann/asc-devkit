# ICachePreLoad\(ISASI\)<a name="ZH-CN_TOPIC_0000001834752537"></a>

## 产品支持情况<a name="section1550532418810"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_cache_intf.h"`。

开发者手动调用ICachePreLoad接口，能够从指令所在GM地址预加载指令到ICache中。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ICachePreLoad(const int64_t preFetchLen)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| preFetchLen | 输入 | 预取长度。|


<!-- npu="950,A3,910b" id8 -->
针对如下型号：
<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id11 -->
preFetchLen参数单位为2K Byte，取值满足如下要求：

$$
\text{preFetchLen}< \dfrac{\text{ICache的大小}}{2K}
$$
 
其中，AIC和AIV的ICache大小分别为32KB和16KB。
<!-- end id8 -->
<!-- npu="310p" id110 -->
针对Atlas 推理系列产品AI Core，传入该参数无效，预取长度均为128Byte。
<!-- end id110 --> 

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section837496171220"></a>

```cpp
int64_t preFetchLen = 2; // 预取指令长度。
AscendC::ICachePreLoad(preFetchLen);
```
