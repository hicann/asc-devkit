# DestroyWithoutPipeAll<a name="ZH-CN_TOPIC_0000002532992898"></a>

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


## 功能说明<a name="section618mcpsimp"></a>

释放资源。

## 函数原型<a name="section620mcpsimp"></a>

```
__aicore__ inline void DestroyWithoutPipeAll()
```

## 约束说明<a name="section633mcpsimp"></a>

用于重复申请释放TPipe，创建TPipe对象后，可调用DestroyWithoutPipeAll手动释放资源。

该接口内部未设置PIPE\_ALL同步，需要开发者自行保证同步流水正确。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

```
AscendC::TPipe pipe; // Pipe内存管理对象
AscendC::TQue<AscendC::TPosition::VECOUT, 2> que; //输出数据Queue队列管理对象，TPosition为VECOUT
uint8_t num = 2;
uint32_t len = 128;
pipe.InitBuffer(que, num, len);
pipe.DestroyWithoutPipeAll();
```

