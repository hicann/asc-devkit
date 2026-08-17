# Debug, Dump, And Advanced Fallbacks

Use this file when the task mentions `printf`, `DumpTensor`, dump-data parsing, unsupported buffer positions, or custom debug paths.

## Recommended Selection Order

1. Use `AscendC::printf` for scalar state, block index, boolean flags, small values, and quick path confirmation.
2. Use `DumpTensor` or compatible `asc_dump_*` helpers when the target buffer position is supported and a tensor dump is enough.
3. Use `show_kernel_debug_data` when the task needs offline parsing of dumped debug bins into readable files.
4. Use custom debug-bus or explicit memory-path instrumentation only when stock debug APIs cannot cover the required storage position or architecture.

## Practical Rules

- `DumpTensor` is a standard SIMD-side debug path, but support still depends on storage position, architecture, and active dump mode.
- From repository docs, stock `DumpTensor` is not a safe default for every L1-related requirement.
- For Ascend 950PR and Ascend 950DT, check `docs/zh/api/context/DumpTensor.md` before claiming L1 Buffer support.
- For ordinary AICore `printf` missing-output or large-print wraparound issues, use the [AICore printf ring-buffer review notes](#aicore-printf-ring-buffer-review-notes) in this file.
- For requests involving L1, L0A, or L0B, first determine whether the requirement is:
  - a supported dump through existing APIs;
  - a memory movement problem;
  - an unsupported-storage debug problem that needs custom handling.
- Do not default to debug-bus unless the task explicitly needs unsupported-buffer visibility or asks for that path.

## L1/L0A/L0B DumpTensor Review Notes

When synchronizing requirements/design docs or reviewing code for `DumpTensor` L1/L0A/L0B support, check the live asc-devkit source first rather than relying on older requirement drafts.

Reusable review points:

- User-facing `DumpTensor` APIs may remain unchanged even when the internal ring-buffer dump path changes.
- Check whether the runtime block-info structure has an explicit debug-bus address field before documenting how local-buffer data reaches GM. Do not describe reserved fields as a debug-bus address unless the current code proves it.
- Do not claim a new `.ascend.meta` TLV exists unless the current compiler code emits it.
- Architecture-specific debug helpers usually live under `impl/utils/debug/npu_arch_<arch>/`; review the target architecture file instead of copying behavior from another architecture.
- Address stepping and local-address interpretation can differ between L1 and L0A/L0B. Verify the current helper before writing docs or tests.
- If a dump-data copy helper returns status, check more than the final write-info update. Header writing, ring-buffer wrap/skip handling, timeout propagation, and reader-visible write offsets must remain consistent.
- For L1 dumps with stale or duplicated first chunks, inspect both producer synchronization before dumping and the debug-bus ready/read sequence before blaming queue semantics.
- For queue-based MMAD smoke cases, `TQue::DeQue` after `DataCopy(...); EnQue(...)` is normally evidence of queue producer/consumer synchronization. If a dump at the DeQue line is wrong, distinguish unsupported dump path/mode from data readiness, and compare L1/A1 data in its actual layout rather than raw GM order.
- Prefer fail-closed defaults in dump helpers: unsupported `Hardware` enum values should not be reported as successful copies.
- Empty compatibility stubs in another architecture directory do not prove real L0A/L0B dump support.

## AICore printf Ring-Buffer Review Notes

When reviewing ordinary AICore-scope `printf_impl` under `impl/utils/debug`, model the external FIFO layout explicitly: `fifospace -> blockInfo -> readInfo -> ringbuf -> writeInfo`, with each block selected by core index and `blockInfo` pointing at the ring buffer payload. Focus on the protocol between the device writer-owned `writeInfo` and the external reader-owned `readInfo`.

Protocol assumptions to confirm against current code:

- If the tail remaining space is smaller than `SkipTlv`, the writer may directly wrap `writeInfo->bufOffset` to 0; the reader is expected to start from the head in that case.
- `SkipTlv` is only for the tail-space case where the tail can hold a skip marker but cannot hold the next full TLV; it tells the reader to skip to the head.
- In the wrapped state `writeOffset < readOffset`, do not propose writing a `SkipTlv` in the middle. The writer should check whether the next TLV would reach or cover `readOffset`; if so, wait for the reader to advance and drop or return on timeout according to the accepted debug behavior.
- A common pitfall is checking tail overflow before checking wrapped-state collision. Prefer handling wrapped collision before tail skip/wrap handling.
- Do not flag timeout-drop as a bug when the accepted design is lossy debug output; confirm whether timeout should drop or overwrite before recommending changes.

## `show_kernel_debug_data`

Resolved tool locations:

- `${CANN_PATH}/tools/show_kernel_debug_data`
- `${CANN_PATH}/python/site-packages/show_kernel_debug_data`

Repository reference doc:

- `${DEVKIT_PATH}/docs/zh/guide/programming_guide/appendix/show_kernel_debug_data_tool.md`

Basic usage:

```bash
show_kernel_debug_data <bin_file_path> [<output_path>]
```

Python usage:

```python
from show_kernel_debug_data import show_kernel_debug_data
show_kernel_debug_data("./input/dump_workspace.bin")
```

Use it when the kernel already emits dump data and the task is offline parsing or inspection.

## Curated Repository References

- `printf` sample:
  `${DEVKIT_PATH}/examples/01_simd_cpp_api/01_utilities/00_printf/printf.asc`
- dump sample:
  `${DEVKIT_PATH}/examples/01_simd_cpp_api/01_utilities/02_dump/mmad_custom_dump.asc`
- A1/L0 path sample:
  `${DEVKIT_PATH}/examples/01_simd_cpp_api/05_compatibility_guide/data_copy_l1togm_950/data_copy_l1togm.asc`

## Advanced Fallback Rule

If the task explicitly needs unsupported storage such as L1, L0A, or L0B and the normal dump path does not cover it:

- keep the ordinary solution on the normal AscendC path first;
- add a custom debug path only for the unsupported visibility gap;
- document clearly that the custom path is architecture-sensitive;
- keep the custom path isolated so normal kernels do not depend on it.
