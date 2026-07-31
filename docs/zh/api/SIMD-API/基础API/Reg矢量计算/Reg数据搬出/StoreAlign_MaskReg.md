# MaskReg搬出（StoreAlign）<a name="ZH-CN_TOPIC_0000001958347433"></a>

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`。

StoreAlign能够实现数据从[MaskReg](../寄存器数据类型/MaskReg.md)搬运至Unified Buffer（UB）。有三类搬运接口：

- 普通搬运接口：完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。
- PostUpdate扩展搬运接口：
    - POST_MODE_NORMAL模式：不支持。
    - POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。

- 使用[AddrReg](../寄存器数据类型/AddrReg.md)寄存器存储偏移量场景：在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。

## 函数原型<a name="section620mcpsimp"></a>

- 普通搬运接口

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask)
    ```

- PostUpdate扩展搬运接口

    ```cpp
    template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, MaskReg& mask, int32_t offset)
    ```

- 使用[AddrReg](../寄存器数据类型/AddrReg.md)存储偏移量接口，需要配合CreateAddrReg接口使用

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask, AddrReg offset)
    ```
    
## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| postMode | 用于控制是否使能UB地址会自动更新功能（post update），PostLiteral类型。具体参数说明请参考[PostLiteral](../辅助数据类型/PostLiteral.md)。 |
| dist | 搬运模式，MaskDist类型。具体参数说明请参考[表3 MaskDist参数说明](#表3-MaskDist参数说明)。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstAddr | 输入/输出 | 目的操作数，UB起始地址，需要32字节对齐。 |
| mask | 输入 | 源操作数，类型为[MaskReg](../寄存器数据类型/MaskReg.md)。 |
| offset | 输入 | &bull; 当数据类型为AddrReg，为地址偏移寄存器，需要在搬运前调用CreateAddrReg设定地址偏移量，实际搬运UB地址为dstAddr + 寄存器中存储的偏移量。AddrReg寄存器中存储的偏移量 * sizeof(T)的对齐约束请参考[表3 MaskDist参数说明](#表3-MaskDist参数说明)。<br>&bull; 当数据类型为int32_t，为地址更新步长，单位：字节。offset的对齐约束请参考[表3 MaskDist参数说明](#表3-MaskDist参数说明)。根据postMode的取值有两种情况：<br>&nbsp;&nbsp;&bull; POST_MODE_NORMAL：不支持。<br>&nbsp;&nbsp;&bull; POST_MODE_UPDATE：实际搬运UB起始地址为dstAddr，搬运后执行地址更新 dstAddr += offset。 |

<a id="表3-MaskDist参数说明"></a>
**表 3**  MaskDist参数说明

| MaskDist取值 | 含义 | 搬运对齐约束（Byte） |
|-----|-----|-----|
| DIST_NORM | 正常模式，搬运VL/8数据。 | VL/8 |
| DIST_PACK | 压缩模式，每间隔1bit舍弃数据，将VL/8的数据压缩为VL/16搬出。 | VL/16 |

## 数据类型

目的操作数支持的数据类型为：b8、b16、b32、b64。

## 返回值说明<a name="section1575141714439"></a>

无

## 约束说明<a name="section11585101304320"></a>

- 位于Unified Buffer的地址约束、offset的对齐约束、AddrReg寄存器中存储的偏移量 \* sizeof(T)的对齐约束与MaskDist有关，具体地址约束请参考：[表3 MaskDist参数说明](#表3-MaskDist参数说明)。
- MaskReg模板参数regTrait只支持RegTraitNumOne。

## 调用示例<a name="section642mcpsimp"></a>

- 普通搬运接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode01(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign(mask, srcAddr + i * oneRepeatSize);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, mask);
        }
    }
    ```

- PostUpdate扩展搬运接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void StoreAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(mask, srcAddr, oneRepeatSize);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, mask, oneRepeatSize);
        }
    }
    ```

- 使用AddrReg存储偏移量接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode03(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::MaskReg mask;
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadAlign(mask, srcAddr, aReg);
            AscendC::Reg::StoreAlign(dstAddr, mask, aReg);
        }
    }
    ```

