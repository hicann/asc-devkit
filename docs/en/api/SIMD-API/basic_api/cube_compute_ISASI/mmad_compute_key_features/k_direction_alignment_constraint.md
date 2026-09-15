# K-Direction Alignment Constraint<a name="ZH-CN_TOPIC_0000002569070973"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:04:39.860Z -->

**Feature Description:**

Atlas A2 training products/Atlas A2 inference products/Atlas A3 training products/Atlas A3 inference products support enabling this feature for the Mmad API.

<!-- npu="950" id1 -->
On Ascend 950PR/Ascend 950DT, the L0A Buffer uses NZ layout instead of ZZ layout. In the scenario where the A cube is transposed with non-aligned float data types, the L0A Buffer is 8-aligned in the K direction, so this feature does not need to be enabled to ensure computation correctness.
<!-- end id1 -->

The core function of K-direction alignment is to control, through the kDirectionAlign parameter, the alignment of the L0A Buffer and L0B Buffer matrices in the K direction when using float data types (other data types do not require attention to this feature).

When `mmadParams.kDirectionAlign = true` is set, the matrices in the L0A Buffer/L0B Buffer align the K direction to `ceil(K/16)*16`.

![K-direction alignment schematic diagram](../../../../figures/mmad_kdirectionalign.png)

For example, when K=44 and K=36, both are aligned to 48. In this case, for K=44, all 12 fractal blocks in the L0A Buffer/L0B Buffer are fully read into the CUBE; whereas for K=36, although the aligned value is also 48, only 10 of the fractal blocks are read into the CUBE.

![K-direction alignment to 48 schematic diagram](../../../../figures/kdirectionalign_k_is36.png)

When `mmadParams.kDirectionAlign = false` is set, the matrices in the L0A Buffer/L0B Buffer align the K direction to `ceil(K/8)*8`.

For example, when K=44, it is aligned to 48, and when K=36, it is aligned to 40. In this case, for K=44, all 12 fractal blocks in the **L0A Buffer**/**L0B Buffer** are fully read into the **CUBE**; for K=36, all 10 fractal blocks in the **L0A Buffer**/**L0B Buffer** are read into the **CUBE**.

![Schematic diagram of K direction alignment to 40](../../../../figures/kdirectionalign_k_is36_false.png)

1. In the scenario where cube A is not transposed, there is no need to enable **kDirectionAlign**.

2. In the scenario where cube A is transposed, **kDirectionAlign** needs to be enabled.

When the input requires a transpose operation, for example, the original cube A has dimensions K×M, after being moved into the **L1 Buffer**, its storage form changes to NZ layout (essentially still K×M). In this case, the **LoadData** API with transpose capability should be called. The internal implementation of this API is shown in the following figure: alignment to 16 is performed in the K direction (as shown in the left figure). After the transpose is completed, the data appears in the ZZ fractal form shown in the right figure and is stored in the **L0A Buffer**. To avoid reading invalid data, you can enable the **kDirectionAlign** parameter to read only the valid fractal data.

![Schematic diagram of K direction alignment transpose](../../../../figures/kdirectionalign_demo.png)

**Example:**

```cpp
AscendC::MmadParams mmadParams;
mmadParams.m = m;
mmadParams.n = n;
mmadParams.k = k;
// Set the K direction alignment to ceil(K/16)*16 for the float data type.
mmadParams.kDirectionAlign = true;
AscendC::Mmad(c, a, b, mmadParams);
```
