# UnalignRegForLoad & UnalignRegForStore<a name="ZH-CN_TOPIC_0000001956862301"></a>

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

为提升对不规则内存地址的处理能力，Reg矢量计算支持在数据搬运过程中对非32字节对齐的地址进行访问，适用于从UB向RegTensor非对齐搬运，或从RegTensor向UB非对齐搬运的场景。RegBase引入非对齐寄存器缓存机制，降低非对齐访问带来的性能开销，该机制利用非对齐寄存器（32B）UnalignRegForLoad和UnalignRegForStore作为临时缓存区，用于暂存非32B对齐的数据，从而实现高效的连续非对齐数据传输。

- UnalignRegForLoad为非对齐搬入寄存器，在读非对齐地址时，先通过LoadUnAlignPre进行初始化（将非对齐地址数据暂存进UnalignRegForLoad），再调用LoadUnAlign进行搬运。

- UnalignRegForStore为非对齐搬出寄存器，在写非对齐地址时，先调用StoreUnAlign，再使用StoreUnAlignPost进行后处理（将UnalignRegForStore中的数据写入非对齐UB地址）。

UnalignRegForLoad和UnalignRegForStore的具体使用方式请参考：[连续非对齐搬入（LoadUnAlign）](../Reg数据搬入/LoadUnAlign_continuous.md)、[连续非对齐搬出（StoreUnAlign）](../Reg数据搬出/StoreUnAlign_continuous.md)。

## 支持的型号<a name="section156721693504"></a>

Ascend 950PR/Ascend 950DT

## 约束说明<a name="section11585101304320"></a>

- UnalignRegForLoad寄存器数量上限为4。

- UnalignRegForStore寄存器数量上限为4。
