# VacantInQue<a name="ZH-CN_TOPIC_0000001811034068"></a>

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

头文件路径为：`"basic_api/kernel_tpipe.h"`。

查询队列是否已满。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline bool VacantInQue()
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

该接口不支持[Tensor原地操作](../../../../../guide/编程指南/附录/常用操作/如何使用Tensor原地操作提升算子性能.md)，即TQue的depth设置为0的场景。

## 返回值说明<a name="section640mcpsimp"></a>

-   true - 表示Queue未满，可以继续Enque操作
-   false - 表示Queue已满，不可以继续入队

## 调用示例<a name="section642mcpsimp"></a>

```cpp
// 根据VacantInQue判断当前que是否已满，设置当前队列深度为4
AscendC::TPipe pipe;
AscendC::TQueBind<AscendC::TPosition::VECOUT, AscendC::TPosition::GM, 4> que;
int num = 10;
int len = 1024;
pipe.InitBuffer(que, num, len);
bool ret = que.VacantInQue(); // 返回为true 
AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor2 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor3 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor4 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor5 = que.AllocTensor<half>();
que.EnQue(tensor1);// 将tensor1加入VECOUT的Queue中
que.EnQue(tensor2);// 将tensor2加入VECOUT的Queue中
que.EnQue(tensor3);// 将tensor3加入VECOUT的Queue中
que.EnQue(tensor4);// 将tensor4加入VECOUT的Queue中
ret = que.VacantInQue(); // 返回为false,继续入队操作（Enque）将报错
```
