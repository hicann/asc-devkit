# AllocTensor<a name="ZH-CN_TOPIC_0000002161196177"></a>

## 产品支持情况<a name="section1550532418810"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_tpipe.h"`。

从TSCM中分配tensor，tensor所占大小为InitBuffer时设置的每块内存长度。注意，分配的tensor内容并非全0，可能会是随机值。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline LocalTensor<T> AllocTensor()
```

## 参数说明<a name="section622mcpsimp"></a>

| 参数名称 | 说明 |
| -------- | ---- |
| T | 待获取Tensor的数据类型，支持的类型请见[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)相关描述。|

## 约束说明<a name="section633mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

LocalTensor对象。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
AscendC::TPipe pipe;
AscendC::TSCM<AscendC::TPosition::VECIN, 1> tscm;
int num = 4;
int len = 1024;
pipe.InitBuffer(tscm, num, len); // InitBuffer分配内存块数为4，每块大小为1024Bytes
AscendC::LocalTensor<half> tensor1 = tscm.AllocTensor<half>(); // AllocTensor分配Tensor长度为1024Bytes
tscm.EnQue(tensor1);
tensor1 = tscm.DeQue<half>();
tscm.FreeTensor(tensor1);
```
