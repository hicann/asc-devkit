# Histogram Computation

<!-- md-trans-meta sourceCommit=b6a18ceb669b3fdb6996232e81a739ccd0a507f2 translatedAt=2026-10-27T03:45:45.147Z pushedAt=2026-10-27T13:03:35.418Z nl=2 -->

This section describes the register histogram statistics APIs.

## API List

| API | Function |
| --- | --- |
| [histograms](histograms.md) | • Frequency statistics: <br>$dst_i = dst_i + \sum_{j:\,src.mask_j=1}\mathbf{1}(src_j=h+i)$<br>• Cumulative statistics: <br>$dst_i = dst_i + \sum_{j:\,src.mask_j=1}\mathbf{1}(src_j\le h+i)$<br>where $h=0$ in low-bit mode and $h=128$ in high-bit mode. |

The valid range of the input data is determined by `src.mask`, and the returned object inherits `dst.mask`.
