# LocalMemBar<a name="ZH-CN_TOPIC_0000002222134221"></a>

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_membar_intf.h"`。

Reg矢量计算宏函数内不同流水线之间的同步指令。该同步指令指定src源流水线和dst目的流水线，如下图所示，目的流水线将等待源流水线上所有指令完成才进行执行。读写场景下，当读指令使用的寄存器和写指令使用的寄存器相同时，可以触发寄存器保序，指令将会按照代码顺序执行，不需要插入同步指令，而当使用的寄存器不同时，如果要确保读写指令顺序执行，则需要插入同步指令，写写场景同理。MemType用于标识不同的流水线类型，其具体取值与含义详见[表 MemType取值说明](#table2_memtype)。

**图1**  流水线等待示意图<a name="fig2635167165614"></a>  
![](../../../../figures/流水线等待示意图.png "流水线等待示意图")

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <MemType src, MemType dst> 
__simd_callee__ inline void LocalMemBar()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| src | 源流水线，类型为MemType，具体参见[表2 MemType取值说明](#table2_memtype)。 |
| dst | 目的流水线，类型为MemType，具体参见[表2 MemType取值说明](#table2_memtype)。 |

**表 2**  MemType取值说明<a name="table2_memtype"></a>

| MemType取值 | 含义 |
| ------ | ------ |
| VEC_STORE | SIMD_VF函数内矢量写UB流水线。<br>对应寄存器到UB的搬运指令，如StoreAlign、StoreUnAlign、Store。 |
| VEC_LOAD | SIMD_VF函数内矢量读UB流水线。<br>对应UB到寄存器的搬运指令，如LoadAlign、LoadUnAlign、Load。 |
| SCALAR_STORE | SIMD_VF函数内标量写UB流水线。<br>对应标量写入UB的指令。 |
| SCALAR_LOAD | SIMD_VF函数内标量读UB流水线。<br>对应UB读取标量的指令。 |
| VEC_ALL | SIMD_VF函数内所有矢量读写UB流水线。 |
| SCALAR_ALL | SIMD_VF函数内所有标量读写UB流水线。 |

**表3**  src和dst组合取值说明

| src | dst |
| ------ | ------ |
| MemType::VEC_STORE | MemType::VEC_LOAD |
| MemType::VEC_LOAD | MemType::VEC_STORE |
| MemType::VEC_STORE | MemType::VEC_STORE |
| MemType::VEC_STORE | MemType::SCALAR_LOAD |
| MemType::VEC_STORE | MemType::SCALAR_STORE |
| MemType::VEC_LOAD | MemType::SCALAR_STORE |
| MemType::SCALAR_STORE | MemType::VEC_LOAD |
| MemType::SCALAR_STORE | MemType::VEC_STORE |
| MemType::SCALAR_LOAD | MemType::VEC_STORE |
| MemType::VEC_ALL | MemType::VEC_ALL |
| MemType::VEC_ALL | MemType::SCALAR_ALL |
| MemType::SCALAR_ALL | MemType::VEC_ALL |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

无

## 约束说明<a name="section633mcpsimp"></a>

读写依赖的场景下，如果读指令和写指令使用的寄存器相同，会触发寄存器保序，指令将会按照代码顺序执行，无需额外插入同步指令。

## 调用示例<a name="section642mcpsimp"></a>

如下示例中dstPtr和src0Ptr为同一个UB地址，for循环的第二次迭代中读UB矢量搬运和第一次迭代中写UB矢量搬运操作了同一块UB地址空间，因此第二次循环中读UB矢量搬运需要等待第一次循环中写UB矢量搬运执行完成后才能执行，需要插入VEC_LOAD等待VEC_STORE的同步。

```cpp
template<typename T>
__simd_vf__ inline void AddVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;   
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LocalMemBar<AscendC::Reg::MemType::VEC_STORE, AscendC::Reg::MemType::VEC_LOAD>();
        AscendC::Reg::LoadAlign(srcReg0, src0Addr);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr, dstReg, mask);
    }
}
```
