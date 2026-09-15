# Int8 Channel Merge<a name="ZH-CN_TOPIC_0000002538071280"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:15:53.604Z -->

## Description

During the cube move-out process, when the target data type is converted to `int8_t` or `uint8_t`, this feature is automatically enabled for NZ output. The hardware expands the fractal cube from 16×16 to 16×32. The specific rules are as follows: if the number of output channels N is an even multiple of 16, every two adjacent 16×16 fractal matrices in the N direction are merged into a single 16×32 fractal cube. If N is an odd multiple of 16, channels 1 through channel (N–16) are merged, while the last 16 channels remain unchanged and are not merged.

## Constraints

The hardware enables this feature automatically and it cannot be configured.

## Example

For example, when the target data type is **s8**, **M** is 32, and **N** is 48, the hardware first merges the first two 16×16 fractal matrices in the **N** direction into a 16×32 cube, and then directly moves the remaining 16×16 fractal cube to the L1 Buffer.

**Figure 1** Int8 Channel Merge diagram<a name="zh-cn_topic_0000002547260781_fig1935632714360"></a>  

![](../../../../figures/Fixpipe_Int8_Channel_Merge.png)
