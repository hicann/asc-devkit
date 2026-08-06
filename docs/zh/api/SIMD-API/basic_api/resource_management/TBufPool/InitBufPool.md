# InitBufPool<a name="ZH-CN_TOPIC_0000001912802253"></a>

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
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_tpipe.h"`。

通过Tpipe::InitBufPool接口可划分出整块资源，整块TbufPool资源可以继续通过TBufPool::InitBufPool接口划分成小块资源。

资源分配的非共享模式与共享模式：
- 非共享模式：从Tpipe中划分出一块新的资源池。
- 共享模式：从Tpipe划分出的新资源池与其他资源池共享起始地址及长度。

## 函数原型<a name="section620mcpsimp"></a>

-   非共享模式

    ```cpp
    template <class T>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len)
    ```

-   共享模式

    ```cpp
    template <class T, class U>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a>bufPool<span>参数的类型。</span></p>
</td>
</tr>
<tr id="row758993611212"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p115897367213"><a name="p115897367213"></a><a name="p115897367213"></a>U</p>
</td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12589153615212"><a name="p12589153615212"></a><a name="p12589153615212"></a>shareBuf参数的类型。</p>
</td>
</tr>
</tbody>
</table>

**表2**  非共享模式接口参数说明

<a name="table1960411494236"></a>
<table><thead align="left"><tr id="row17605204922320"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p6605649122315"><a name="p6605649122315"></a><a name="p6605649122315"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p17605949182313"><a name="p17605949182313"></a><a name="p17605949182313"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p17605184915230"><a name="p17605184915230"></a><a name="p17605184915230"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row260544916231"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p104214518241"><a name="p104214518241"></a><a name="p104214518241"></a>bufPool</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1842145172415"><a name="p1842145172415"></a><a name="p1842145172415"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p164211556249"><a name="p164211556249"></a><a name="p164211556249"></a>新划分的资源池，类型为TBufPool。</p>
</td>
</tr>
<tr id="row03336319398"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1042110572411"><a name="p1042110572411"></a><a name="p1042110572411"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1342115582416"><a name="p1342115582416"></a><a name="p1342115582416"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p154210516247"><a name="p154210516247"></a><a name="p154210516247"></a>新划分资源池长度，单位为字节，非32字节对齐会自动向上补齐至32字节对齐。</p>
</td>
</tr>
</tbody>
</table>

**表3**  共享模式接口参数说明

<a name="table5376122715308"></a>
<table><thead align="left"><tr id="row1337716275309"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p1537762711305"><a name="p1537762711305"></a><a name="p1537762711305"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="12.370000000000001%" id="mcps1.2.4.1.2"><p id="p153771127123013"><a name="p153771127123013"></a><a name="p153771127123013"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="75.27000000000001%" id="mcps1.2.4.1.3"><p id="p17377162715303"><a name="p17377162715303"></a><a name="p17377162715303"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row19377627133012"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p55733283248"><a name="p55733283248"></a><a name="p55733283248"></a>bufPool</p>
</td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p1357372832410"><a name="p1357372832410"></a><a name="p1357372832410"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p1457322822419"><a name="p1457322822419"></a><a name="p1457322822419"></a>新划分的资源池，类型为TBufPool。</p>
</td>
</tr>
<tr id="row13377162793019"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1357312818241"><a name="p1357312818241"></a><a name="p1357312818241"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p857319288247"><a name="p857319288247"></a><a name="p857319288247"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p17573122810249"><a name="p17573122810249"></a><a name="p17573122810249"></a>新划分资源池长度，单位为字节，非32字节对齐会自动向上补齐至32字节对齐。</p>
</td>
</tr>
<tr id="row1371133216245"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1417710361241"><a name="p1417710361241"></a><a name="p1417710361241"></a>shareBuf</p>
</td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p15177113615246"><a name="p15177113615246"></a><a name="p15177113615246"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p11771236152417"><a name="p11771236152417"></a><a name="p11771236152417"></a>被复用资源池，类型为TBufPool，新划分资源池与被复用资源池共享起始地址及长度。</p>
</td>
</tr>
</tbody>
</table>

## 约束说明<a name="section633mcpsimp"></a>

- 新划分的资源池与被复用资源池的物理内存需要一致，两者共享起始地址及长度；
- 输入长度需要小于等于被复用资源池长度；
- 其他泛用约束参考[TBufPool](TBufPool.md)；

## 返回值说明<a name="section640mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

数据量较大且内存有限时，无法一次完成所有数据搬运，需要拆分成多个阶段计算，每次计算使用其中的一部分数据，可以通过TBufPool资源池进行内存地址复用。本例中，从Tpipe划分出资源池tbufPool0，tbufPool0为src0Gm分配空间后，继续分配了资源池tbufPool1，指定tbufPool1与tbufPool2复用并分别运用于第一、二轮计算，此时tbufPool1及tbufPool2共享起始地址及长度。

完整样例链接可以参考[tbufpool\_management样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/04_memory_management/tbufpool_management)。

```cpp
AscendC::TPipe* pipe;
AscendC::TBufPool<AscendC::TPosition::VECCALC> tbufPool0, tbufPool1, tbufPool2;
// srcQue0、srcQue1、srcQue2为VECIN上的TQue，dstQue0、dstQue1为VECOUT上的TQue
// 从Tpipe划分出资源池tbufPool0
pipe->InitBufPool(tbufPool0, bufSize * 4);
// 通过tbufPool0给tbufPool1分配空间，并指定tbufPool1与tbufPool2复用同一块空间
tbufPool0.InitBufPool(tbufPool1, bufSize * 2);
// 给srcQue0分配空间
tbufPool0.InitBuffer(srcQue0, 1, bufSize * 2); // Total src0
tbufPool0.InitBufPool(tbufPool2, bufSize * 2, tbufPool1);
// 通过tbufPool1给srcQue1、dstQue0分配空间
tbufPool1.InitBuffer(srcQue1, 1, bufSize);
tbufPool1.InitBuffer(dstQue0, 1, bufSize);
CopyIn();
Compute();
CopyOut();
// 切换资源池到tbufPool2，并通过tbufPool2给srcQue2、dstQue1分配空间
tbufPool1.Reset();
tbufPool2.InitBuffer(srcQue2, 1, bufSize);
tbufPool2.InitBuffer(dstQue1, 1, bufSize);
CopyIn1();
Compute1();
CopyOut1();
tbufPool2.Reset();
tbufPool0.Reset();
pipe->Reset();
```
