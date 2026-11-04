# Int8 Channel Merge

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-27T10:42:37.208Z pushedAt=2026-10-27T13:04:46.138Z nl=2 -->

Channel Merge is a hardware capability that automatically merges data during the process of moving cube computation results out of the cube unit. It takes effect only in output scenarios where the destination format is NZ, and users neither need to nor can configure it manually through parameters.

Int8 Channel Merge: When the destination data type is `int8_t` or `uint8_t`, this feature is automatically enabled for NZ output. The hardware expands the fractal cube from 16×16 to 16×32. If the number of output channels N is an even multiple of 16, every two adjacent 16×16 fractal cubes in the N direction are merged into one 16×32 fractal cube. If N is an odd multiple of 16, the first N - 16 channels participate in the merge, while the last 16 channels remain as an unchanged 16×16 fractal cube. For the Ascend 950PR&950DT products, the `hifloat8_t` and `fp8_e4m3fn_t` types also support this feature.

## Example

Int8 Channel Merge: When the target data type is `int8_t`, M is 32, and N is 48, the hardware merges the first two 16×16 fractal cubes in the N direction into one 16×32 fractal cube, while the remaining 16×16 fractal cube is kept unchanged and output.

**Figure 1**  Int8 Channel Merge diagram

![Int8 Channel Merge diagram](../../../../figures/Fixpipe_Int8_Channel_Merge.png)

## Constraints

- Channel Merge is automatically enabled by the hardware and cannot be configured through parameters.
- Int8 Channel Merge is automatically enabled when the output is in NZ format and the destination type is `int8_t` or `uint8_t`. On Ascend 950PR&950DT products, this feature is also supported for `hifloat8_t` and `fp8_e4m3fn_t` NZ output.
