# GetCurAddr<a name="ZH-CN_TOPIC_0000002303777434"></a>

## 产品支持情况<a name="section73648168211"></a>

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
- Atlas 推理系列产品Vector Core：支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->


## 功能说明<a name="section618mcpsimp"></a>

返回当前物理位置空闲的起始地址。

## 函数原型<a name="section620mcpsimp"></a>

```
template <Hardware hard>
__aicore__ inline uint32_t LocalMemAllocator<hard>::GetCurAddr() const
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

当前物理位置空闲的起始地址，范围为\[0，物理内存最大值\)。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

```
LocalMemAllocator allocator;
// 默认的物理位置为UB，由于从0地址开始分配，下面的打印结果为0
AscendC::printf("current addr is %u\n", allocator.GetCurAddr());
```

