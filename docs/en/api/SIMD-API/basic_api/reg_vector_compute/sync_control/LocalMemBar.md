# LocalMemBar<a name="ZH-CN_TOPIC_0000002222134221"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:10:22.095Z pushedAt=2026-10-26T07:41:44.608Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 Inference Product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/sync_control/LocalMemBar_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_membar_intf.h"`.

A synchronization instruction between different pipelines in a Reg vector compute macro function. This synchronization instruction specifies the **src** source pipeline and the **dst** destination pipeline. As shown in the following figure, the destination pipeline waits until all instructions on the source pipeline are complete before executing. In read/write scenarios, when the register used by a read instruction is the same as the register used by a write instruction, register ordering can be triggered. The instructions are then executed in code order without inserting a synchronization instruction. When different registers are used, a synchronization instruction must be inserted to ensure that the read and write instructions are executed in order. The same applies to write/write scenarios. **MemType** identifies different pipeline types. For its specific values and meanings, see [Table MemType value description](#table2_memtype).

**Figure 1**  Pipeline wait diagram<a name="fig2635167165614"></a>  
![](../../../../figures/pipeline_wait.png "Pipeline wait diagram")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <MemType src, MemType dst> 
__simd_callee__ inline void LocalMemBar()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter Name | Description |
| ------ | ------ |
| src | Source pipeline, of the MemType type. For details, see [Table 2 MemType value description](#table2_memtype). |
| dst | Destination pipeline, of the MemType type. For details, see [Table 2 MemType value description](#table2_memtype). |

**Table 2** MemType value description<a name="table2_memtype"></a>

| MemType Value | Meaning |
| ------ | ------ |
| VEC_STORE | Vector write UB pipeline in the SIMD_VF function.<br>Corresponds to register-to-UB copy instructions, such as StoreAlign, StoreUnAlign, and Store. |
| VEC_LOAD | Vector read UB pipeline in the SIMD_VF function.<br>Corresponds to UB-to-register copy instructions, such as LoadAlign, LoadUnAlign, and Load. |
| SCALAR_STORE | Scalar write UB pipeline in the SIMD_VF function.<br>Corresponds to instructions that write scalars to UB. |
| SCALAR_LOAD | Scalar read UB pipeline in the SIMD_VF function.<br>Corresponds to instructions that read scalars from UB. |
| VEC_ALL | All vector read/write UB pipelines in the SIMD_VF function. |
| SCALAR_ALL | All scalar read/write UB pipelines in the SIMD_VF function. |

**Table 3** src and dst combination value description

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

## Return Value Description<a name="section640mcpsimp"></a>

None

## Data Types

None

## Constraints<a name="section633mcpsimp"></a>

In read/write dependency scenarios, if the read instruction and the write instruction use the same register, register ordering is triggered, and the instructions are executed in code order without the need to insert an additional synchronization instruction.

## Example<a name="section642mcpsimp"></a>

In the following example, **dstPtr** and **src0Ptr** point to the same Unified Buffer (UB) address. In the second iteration of the for loop, the UB vector read and the UB vector write in the first iteration operate on the same UB address space. Therefore, the UB vector read in the second iteration must wait until the UB vector write in the first iteration completes before it can execute, which requires inserting synchronization for **VEC_LOAD** to wait for **VEC_STORE**.

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
