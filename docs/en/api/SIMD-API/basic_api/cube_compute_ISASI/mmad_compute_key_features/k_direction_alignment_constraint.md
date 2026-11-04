# K Direction Alignment Constraints<a name="ZH-CN_TOPIC_0000002569070973"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:33:44.881Z pushedAt=2026-10-26T07:41:44.200Z nl=2 -->

**Description:**

The Mmad API supports this feature on Atlas A2 products/Atlas A3 products.

<!-- npu="950" id1 -->
On the Ascend 950PR&950DT products, the L0A Buffer uses NZ layout instead of ZZ layout. In the scenario where the A matrix is transposed and the float data type is non-aligned, the L0A Buffer is aligned to 8 in the K direction, so this feature does not need to be enabled to ensure computation correctness.
<!-- end id1 -->

The core function of K direction alignment is to control, through the **kDirectionAlign** parameter, the alignment of the L0A Buffer and L0B Buffer matrices in the K direction when the float data type is used (other data types do not require attention to this feature).

When **mmadParams.kDirectionAlign** is set to **true**, the matrices in the L0A Buffer/L0B Buffer align the K direction to \`ceil\(K/16\)\*16\`.

![Schematic diagram of K direction alignment](../../../../figures/mmad_kdirectionalign.png)

For example, when K=44 and K=36, both are aligned to 48. In this case, for K=44, all 12 fractal blocks in the L0A Buffer/L0B Buffer are fully read into the CUBE; for K=36, although the aligned value is also 48, only 10 of the fractal blocks are read into the CUBE.

![Schematic diagram of K direction alignment to 48](../../../../figures/kdirectionalign_k_is36.png)

When \`mmadParams.kDirectionAlign is set to false\`, the matrices in the L0A Buffer/L0B Buffer align the K direction to \`ceil\(K/8\)\*8\`.

For example, when K=44, it is aligned to 48, while when K=36, it is aligned to 40. In this case, for K=44, all 12 fractal blocks in the L0A Buffer/L0B Buffer are completely read into the CUBE; for K=36, all 10 fractal blocks in the L0A Buffer/L0B Buffer are read into the CUBE.

![Schematic diagram of K direction alignment to 40](../../../../figures/kdirectionalign_k_is36_false.png)

1. In the scenario where matrix A is not transposed, there is no need to enable **kDirectionAlign**.

2. In the scenario where matrix A is transposed, **kDirectionAlign** needs to be enabled.

When the input requires a transpose operation, for example, the original matrix A has dimensions K×M. After being moved into the L1 Buffer, its storage format changes to NZ layout (which is still essentially K×M). In this case, the **LoadData** API with transpose capability should be called. The internal implementation of this API is shown in the following figure: 16-byte alignment is performed in the K direction (as shown in the left figure). After the transpose is completed, the data appears in the ZZ fractal form shown in the right figure and is stored in the L0A Buffer. To avoid reading invalid data, you can enable the **kDirectionAlign** parameter to read only the valid fractal data.

![Schematic diagram of K direction alignment transpose](../../../../figures/kdirectionalign_demo.png)

**Example:**

```cpp
AscendC::MmadParams mmadParams;
mmadParams.m = m;
mmadParams.n = n;
mmadParams.k = k;
// Align the K direction to ceil(K/16)*16 for the float data type.
mmadParams.kDirectionAlign = true;
AscendC::Mmad(c, a, b, mmadParams);
```
