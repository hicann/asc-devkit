# Composite Compute

<!-- md-trans-meta sourceCommit=b6a18ceb669b3fdb6996232e81a739ccd0a507f2 translatedAt=2026-10-27T03:35:48.506Z pushedAt=2026-10-27T13:03:35.396Z nl=2 -->

This section describes register compute APIs that fuse multiple compute steps into a single call.

## API List

| API | Description |
| --- | --- |
| [axpy](axpy.md) | $dst_i = scalar \times src_i + dst_i$ |
| [abs_diff](abs_diff.md) | $dst_i = \lvert src0_i - src1_i \rvert$ |
| [exp_diff](exp_diff.md) | • When the input data type is `float`:<br>$dst_i = e^{src0_i - src1_i}$<br>• When the input data type is `half`:<br>$dst_i = e^{cast\_f16\_to\_f32(src0_i) - cast\_f16\_to\_f32(src1_i)}$ |
| [fma](fma.md) | $dst_i = src0_i \times src1_i + src2_i$ |
| [madd](madd.md) | $dst_i = dst_i \times src0_i + src1_i$ |
| [mula](mula.md) | $dst_i = src0_i \times src1_i + dst_i$ |
| [muls_cast](muls_cast.md) | $dst_i = cast\_round\_to\_f16(src_i \times scalar)$ |
