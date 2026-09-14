# asc_mem_bar

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:47:53.873Z pushedAt=2026-09-05T12:14:28.904Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

A synchronization instruction used between different pipelines in a Reg computation macro function. The instruction specifies a source pipeline and a destination pipeline. As shown in the following figure, the destination pipeline waits until all instructions on the source pipeline are complete before executing.
For read-after-write scenarios, if the read and write instructions use the same register, register ordering is automatically enforced and the instructions are executed in code order, so no synchronization instruction is required. If they use different registers, a synchronization instruction must be inserted to ensure the execution order of the read and write instructions. The same applies to write-after-write scenarios.

![Pipeline wait diagram within the SIMD_VF function](../../figures/SIMD_VF_function_pipeline_wait_diagram.png)

## Prototype

```cpp
__simd_callee__ inline void asc_mem_bar(MEM_TYPE mem_type)
```

## Parameters

Table 1 Parameter description
| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| mem_type | Input | Type of the synchronization pipeline. The type is MEM_TYPE. For the value range, see Table 2 mem_type value description. |

Table 2 mem_type value description (for the meanings of source/destination pipelines, see Table 3)
| Value  | Source Pipeline | Destination Pipeline |
| ------ | -------- | -------- |
| VV_ALL | VEC_ALL | VEC_ALL |
| VST_VLD | VEC_STORE | VEC_LOAD |
| VLD_VST | VEC_LOAD | VEC_STORE |
| VST_VST | VEC_STORE | VEC_STORE |
| VS_ALL | VEC_ALL | SCALAR_ALL |
| VST_LD | VEC_STORE | SCALAR_LOAD |
| VLD_ST | VEC_LOAD | SCALAR_STORE |
| VST_ST | VEC_STORE | SCALAR_STORE |
| SV_ALL | SCALAR_ALL | VEC_ALL |
| ST_VLD | SCALAR_STORE | VEC_LOAD |
| LD_VST | SCALAR_LOAD | VEC_STORE |
| ST_VST | SCALAR_STORE | VEC_STORE |
| SS_ALL | SCALAR_ALL | SCALAR_ALL |
| ST_LD | SCALAR_STORE | SCALAR_LOAD |
| LD_ST | SCALAR_LOAD | SCALAR_STORE |
| ST_ST | SCALAR_STORE | SCALAR_STORE |

Table 3 Reg computation pipeline description
| Pipeline | Meaning |
| ------ | -------- |
| VEC_STORE | Vector write-to-UB pipeline within the SIMD_VF function.<br>Corresponds to register-to-UB transfer instructions, for example, [asc_store](../reg_store/asc_store.md). |
| VEC_LOAD | Vector read-from-UB pipeline within the SIMD_VF function.<br>Corresponds to UB-to-register transfer instructions, for example, [asc_load](../reg_load/asc_load.md). |
| SCALAR_STORE | Scalar write-to-UB pipeline within the SIMD_VF function.<br>Corresponds to instructions that write scalars to UB, for example, [asc_duplicate_scalar](./asc_duplicate_scalar.md). |
| SCALAR_LOAD | Scalar read-from-UB pipeline within the SIMD_VF function.<br>Corresponds to instructions that read scalars from UB. |
| VEC_ALL | All vector read/write UB pipelines within the SIMD_VF function. |
| SCALAR_ALL | All scalar read/write UB pipelines within the SIMD_VF function. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

In the following example, the UB vector load in the current iteration of the for loop and the UB vector store in the previous iteration access the same UB address space. Therefore, the UB vector load in the current iteration must wait until the UB vector store in the previous iteration is complete. VST_VLD must be inserted to synchronize VEC_LOAD with VEC_STORE.

```cpp
// ub_addr is the externally input UB memory space address.
__ubuf__ half* ub_addr;

vector_half dst;
vector_half src0;
vector_half src1;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);

uint16_t repeatTime = 8;
uint16_t repeatSize = 64;
for (uint16_t i = 1; i < repeatTime; i++) {
    asc_mem_bar(VST_VLD);
    asc_loadalign(src0, ub_addr);
    asc_loadalign(src1, ub_addr + i * repeatSize);
    asc_add(dst, src0, src1, mask);
    asc_storealign(ub_addr, dst, mask);
}

```
