# 连续非对齐搬入（LoadUnAlign）<a name="ZH-CN_TOPIC_0000001955990981"></a>

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

为提升对不规则内存地址的处理能力，Reg矢量计算支持在数据搬运过程中对非32字节对齐的地址进行访问，降低非对齐访问带来的性能开销。LoadUnAlign能够实现数据从非对齐的Unified Buffer（UB）连续搬运至[RegTensor](../寄存器数据类型/RegTensor.md)，利用非对齐寄存器[UnalignRegForLoad](../寄存器数据类型/UnalignRegForLoad-UnalignRegForStore.md)作为临时缓存区，暂存跨对齐边界的数据，从而实现高效的连续非对齐数据传输。

非对齐搬运有三类接口：普通搬运接口、PostUpdate扩展搬运接口、使用[AddrReg](../寄存器数据类型/AddrReg.md)寄存器存储偏移量接口。

RegTensor的模板参数regTrait支持RegTraitNumOne及RegTraitNumTwo，具体支持情况如下：

| RegTensor模板参数regTrait | 普通搬运接口 | PostUpdate扩展搬运接口 | 使用AddrReg寄存器存储偏移量接口 |
|-----|-----|-----|-----|
| RegTraitNumOne | 完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。 | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。 |
| RegTraitNumTwo | 不支持 | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 不支持 |

非对齐搬入原理请参考[关键特性说明](#关键特性说明)。

## 函数原型<a name="section620mcpsimp"></a>

非对齐寄存器UnalignRegForLoad作为临时缓存区，用于暂存跨对齐边界的数据，从而实现高效的连续非对齐数据传输。在读非对齐地址前，应该先通过LoadUnAlignPre进行初始化，保存非32字节对齐的数据，然后再调用LoadUnAlign进行数据搬入。

- 普通搬运接口：

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)

    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)
    ```

- PostUpdate扩展搬运接口：

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)

    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, uint32_t postUpdateStride)
    ```

- 使用[AddrReg](../寄存器数据类型/AddrReg.md)存储偏移量接口（需要配合[CreateAddrReg](../寄存器数据类型/AddrReg.md)接口使用）：

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr, AddrReg& areg)

    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, AddrReg& areg, uint32_t inc)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| postMode | 用于控制是否使能UB地址会自动更新功能（post update），PostLiteral类型。具体参数说明请参考[PostLiteral](../辅助数据类型/PostLiteral.md)。 |
| U | 目的操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstReg | 输出 | 目的操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| ureg | 输入/输出 | 非对齐寄存器，UnalignRegForLoad类型，用于存储非32字节的数据，寄存器大小为32字节。 |
| srcAddr | 输入/输出 | 源操作数，UB起始地址，不需要32字节对齐。 |
| postUpdateStride | 输入 | 地址更新步长，uint32_t类型，单位：元素个数。根据postMode的取值有两种情况：<br>&bull; POST_MODE_NORMAL：不支持。<br>&bull; POST_MODE_UPDATE：实际搬运UB起始地址为srcAddr，搬运后执行地址更新 srcAddr += postUpdateStride。 |
| areg | 输入 | 地址偏移寄存器，AddrReg类型，需要在搬运前调用CreateAddrReg设定地址偏移量，实际搬运UB地址为srcAddr + 寄存器中存储的偏移量。 |
| inc | 输入 | 未使用，设置为0即可。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。
- 当RegTensor模板参数regTrait为RegTraitNumOne时，支持的数据类型为：b8、b16、b32、b64。
- 当RegTensor模板参数regTrait为RegTraitNumTwo时，支持的数据类型为：complex32、b64。

## 返回值说明

无

## 约束说明<a name="section1575141714439"></a>

- LoadUnAlignPre与LoadUnAlign接口需要组合使用。
- PostUpdate扩展搬运接口只支持POST_MODE_UPDATE模式。
- 当RegTensor模板参数RegTrait为RegTraitNumOne和RegTraitNumTwo时，支持情况如下：

    | RegTensor模板参数RegTrait取值 | 支持的接口 | 支持的数据类型 |
    |-----|-----|-----|
    | RegTraitNumOne | 所有接口 | b8、b16、b32、b64 |
    | RegTraitNumTwo | PostUpdate扩展搬运接口的POST_MODE_UPDATE模式 | complex32、b64 |

## 关键特性说明

**1、非对齐数据搬入原理**

**图 1**  非对齐搬入示例<a id="fig-loadunalign-1"></a>  
![](../../../../figures/reg_loadunalign.png)

如[图1 非对齐搬入示例](#fig-loadunalign-1)所示，从UB地址srcAddr ~ 304读取数据，并将其搬运至目标寄存器dstReg（256B）。处理流程如下：

① 调用**LoadUnAlignPre**进行非对齐搬入初始化。非对齐寄存器ureg缓存UB地址32 ~ 64的有效数据，作为后续非对齐访问的前置数据缓存。

② 调用**LoadUnAlign**，硬件指令将UB地址64 ~ 320的对齐数据搬入临时寄存器tmpReg，并将ureg中srcAddr ~ 64对应的数据与tmpReg中地址64 ~ 304对应的数据拼接在一起，将结果写入dstReg。此外，UB地址288 ~ 320的数据会被写入ureg。

**2、连续非对齐搬入搬出示例**

**图 2**  连续非对齐搬入搬出示例（数据类型uint32_t）<a id="fig-loadunalign-2"></a>  
![](../../../../figures/reg_loadstoreunalign.png)

**连续非对齐搬入时，LoadUnAlign会将后续未对齐的数据缓存至ureg，所以下一次搬入不需要再次调用LoadUnAlignPre，只需在迭代开始前调用一次LoadUnAlignPre，从而实现非对齐搬入的性能优化。**

**连续非对齐搬出时，下次迭代的StoreUnAlign会将本次迭代StoreUnAlign缓存至ureg中的数据写入UB，所以本次迭代不需要调用StoreUnAlignPost将ureg数据写入UB，只需在迭代结束后调用一次StoreUnAlignPost，从而实现非对齐搬出的性能优化。**

如[图2 连续非对齐搬入搬出示例](#fig-loadunalign-2)，将UB地址48 ~ 560的uint32_t数据[1, 2, 3, ..., 128]搬入至dstReg，再搬回UB，需要两次搬入搬出操作，即for循环执行两次，初始化和后处理移至for循环外。postUpdateStride = 256B / sizeof(T)  （即每次地址偏移256B），repeatTimes = dataSize / 256B（即迭代次数=总数据量/VL）。

PostUpdate扩展搬运接口POST_MODE_UPDATE模式示例片段如下：

```cpp
template <typename T>
__simd_vf__ inline void LoadUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::UnalignRegForLoad ureg1;
    AscendC::Reg::UnalignRegForStore ureg2;
    // 非对齐搬入初始化，只需在迭代开始前调用一次。
    AscendC::Reg::LoadUnAlignPre(ureg1, srcAddr);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadUnAlign(dstReg, ureg1, srcAddr, postUpdateStride);
        AscendC::Reg::StoreUnAlign(dstAddr, dstReg, ureg2, postUpdateStride);
    }
    // 非对齐搬出后处理，只需在迭代结束后调用一次。
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg2, 0);
}
```

具体的搬运步骤如下：

1.  非对齐搬入初始化：更新ureg1 = [1, 2, 3, 4]。
2.  非对齐搬入：tmpReg = [5, 6, 7, ..., 68]，tmpReg部分数据和ureg1数据写入dstReg = [1, 2, 3, ..., 64]，更新ureg1 = [61, 62, 63, ..., 68]。
3.  非对齐搬出：dstReg部分数据[1, 2, 3, ..., 60]写入UB地址48 ~ 288，更新ureg2 = [61, 62, 63, 64]。
4.  非对齐搬入：tmpReg = [69, 70, 71, ...,128]，tmpReg数据和ureg1部分数据写入dstReg = [65, 66, 67, ..., 128]。
5.  非对齐搬出：ureg2数据[61, 62, 63, 64]和dstReg部分数据[65, 66, 67, ..., 124]写入UB地址288 ~ 544，更新ureg2 = [125, 126, 127, 128]。
6.  非对齐搬出后处理：将ureg2中缓存的数据[125, 126, 127, 128]写入UB地址544 ~ 560。

## 调用示例<a name="section15860211204820"></a>

- 普通搬运接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void LoadUnAlignVF1(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr);
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr + i * postUpdateStride);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
    }
    ```

- PostUpdate扩展搬运接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void LoadUnAlignVF2(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr);
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, postUpdateStride);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
    }
    ```

- 使用AddrReg寄存器存储偏移量接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void LoadUnAlignVF3(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr, aReg);
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, aReg, 0);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, aReg);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, aReg);
    }
    ```

