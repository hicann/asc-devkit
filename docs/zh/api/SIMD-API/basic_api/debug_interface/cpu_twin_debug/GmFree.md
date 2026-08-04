# GmFree<a name="ZH-CN_TOPIC_0000002080803569"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section259105813316"></a>

头文件路径为：`"tools/cpudebug/include/stub_def.h"`。

进行核函数的CPU侧运行验证时，用于释放通过[GmAlloc](GmAlloc.md)申请的共享内存。

## 函数原型<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section2067518173415"></a>

```cpp
void GmFree(void *ptr)
```

## 参数说明<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section158061867342"></a>

**表1**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| ptr | 输入 | 需要释放的共享内存的指针。|

## 返回值说明<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section640mcpsimp"></a>

无

## 约束说明<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section794123819592"></a>

- 为了保留接口兼容，推荐[<<<>>>](../../../../../guide/编程指南/调试调优/功能调试/CPU域孪生调试.md)编译使用。
- 传入的指针必须是之前通过GmAlloc申请过的共享内存的指针。

## 调用示例<a name="zh-cn_topic_0000002000199397_zh-cn_topic_0000001592243993_section82241477610"></a>

```cpp
AscendC::GmFree((void*)x);
```
