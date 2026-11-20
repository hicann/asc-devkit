# Int8 Channel Merge<a name="ZH-CN_TOPIC_0000002538071280"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T04:24:32.765Z pushedAt=2026-11-18T01:25:06.787Z nl=2 -->

## Description

During the move-out process of matrix computation, when the target data type is converted to int8\_t or uint8\_t, this feature is automatically enabled for NZ output. The hardware expands the fractal matrix from 16×16 to 16×32. The specific rules are as follows: If the number of output channels N is an even multiple of 16, then in the N direction, every two adjacent 16×16 fractal matrices are merged into a single 16×32 fractal matrix. If N is an odd multiple of 16, channels 1 through channel (N–16) are merged, while the last 16 channels remain unchanged and are not merged.

## Constraints

It is enabled automatically by the hardware and cannot be configured.

## Example

For example, when the target data type is s8, M is 32, and N is 48, the hardware first merges the first two 16×16 fractal matrices along the N direction into a 16×32 matrix, and then directly moves the remaining 16×16 fractal matrix to the L1 Buffer.

**Figure 1** Int8 Channel Merge diagram<a name="zh-cn_topic_0000002547260781_fig1935632714360"></a>  

![](../../../../figures/Fixpipe_Int8_Channel_Merge.png)
