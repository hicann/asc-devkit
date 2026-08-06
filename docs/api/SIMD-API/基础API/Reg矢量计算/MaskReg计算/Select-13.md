# Select<a name="ZH-CN_TOPIC_0000001985457929"></a>

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

给定两个源操作数src0和src1，根据mask的比特位值选取元素，得到目的操作数dst。选择的规则为：当mask的比特位是1时，从src0中选取对应位置的数，比特位是0时从src1选取对应位置的数。

## 函数原型<a name="section620mcpsimp"></a>

```
__simd_callee__ inline void Select(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask);
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>dst</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>目的操作数。</p>
</td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>src0</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>源操作数。</p>
</td>
</tr>
<tr id="row2521428183011"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p58511219123111"><a name="p58511219123111"></a><a name="p58511219123111"></a>src1</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p175262810308"><a name="p175262810308"></a><a name="p175262810308"></a>源操作数。</p>
</td>
</tr>
<tr id="row849213245316"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p04921832195310"><a name="p04921832195310"></a><a name="p04921832195310"></a>mask</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p3926174212712"><a name="p3926174212712"></a><a name="p3926174212712"></a>指示选择src0或src1。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section932512912207"></a>

如下示例处理32个`float`类型的数据，通过`Select`组合两个不同范围的掩码。`maskFirstHalf`的前16位为1、后16位为0，因此生成的`selMask`在前16位使用`maskFirstQuarter`的对应位，在后16位使用`maskFullLength`的对应位。最后，`StoreAlign`仅搬出`selMask`中有效位对应的源数据。

```cpp
__simd_vf__ inline void SelectVF(__ubuf__ float* dstAddr, __ubuf__ float* srcAddr)
{
    AscendC::Reg::RegTensor<float> srcReg;
    AscendC::Reg::MaskReg maskFirstQuarter =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL8>();
    AscendC::Reg::MaskReg maskFirstHalf =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL16>();
    AscendC::Reg::MaskReg maskFullLength =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL32>();
    AscendC::Reg::MaskReg selMask;
    // 前16位从maskFirstQuarter取值，后16位从maskFullLength取值。
    AscendC::Reg::Select(selMask, maskFirstQuarter, maskFullLength, maskFirstHalf);
    // 仅搬入maskFullLength指示的前32个元素。
    AscendC::Reg::LoadAlign<float, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcReg, srcAddr, 1, maskFullLength);
    AscendC::Reg::StoreAlign(dstAddr, srcReg, selMask);
}
```

输入数据如下，`srcAddr`的前16个元素为`1.0`、后16个元素为`2.0`，`dstAddr`的32个元素均为`0.0`：

```text
srcAddr = [1.0, 1.0, ..., 1.0, 2.0, 2.0, ..., 2.0]
dstAddr = [0.0, 0.0, ..., 0.0]
```

`Select`执行后，`selMask`的前8位为1、第9位至第16位为0、后16位为1，其余位均为0。`selMask`是`MaskReg`类型，因此其中的值表示掩码比特：

```text
selMask = [1, 1, ..., 1, 0, 0, ..., 0, 1, 1, ..., 1]
```

`StoreAlign`执行后，`dstAddr`的前8个元素为`1.0`，第9个至第16个元素保持初始值`0.0`，后16个元素为`2.0`：

```text
dstAddr = [1.0, 1.0, ..., 1.0, 0.0, 0.0, ..., 0.0, 2.0, 2.0, ..., 2.0]
```
