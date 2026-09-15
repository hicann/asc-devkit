# Int8 Channel Merge

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:09:03.093Z -->

Channel Merge is a hardware capability that automatically merges data during the process of moving cube computation results out. It takes effect only in output scenarios where the destination format is NZ, and users neither need to nor can configure it manually through parameters.

Int8 Channel Merge: When the target data type is `int8_t` or `uint8_t`, this feature is automatically enabled for NZ output. The hardware expands the fractal cube from 16×16 to 16×32. If the number of output channels N is an even multiple of 16, every two adjacent 16×16 fractal matrices in the N direction are merged into one 16×32 fractal cube. If N is an odd multiple of 16, the first N - 16 channels participate in the merge, while the last 16 channels remain as an unchanged 16×16 fractal cube. For Ascend 950PR/Ascend 950DT, the `hifloat8_t` and `fp8_e4m3fn_t` types also support this feature.

## Example

Int8 Channel Merge: When the target data type is `int8_t`, M is 32, and N is 48, the hardware merges the first two 16×16 fractal matrices in the N direction into one 16×32 fractal cube, while the remaining 16×16 fractal cube is output unchanged.

**Figure 1**  Int8 Channel Merge diagram

![Int8 Channel Merge diagram](../../../../figures/Fixpipe_Int8_Channel_Merge.png)

## Constraints

- Channel Merge is enabled automatically by hardware and cannot be configured through parameters.
- Int8 Channel Merge is automatically enabled when the output is in NZ format and the destination type is `int8_t` or `uint8_t`. On Ascend 950PR/Ascend 950DT, this feature is also supported for NZ output of `hifloat8_t` and `fp8_e4m3fn_t`.
