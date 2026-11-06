# Requirement Type Routing

AscendC requirements handled by this skill fall into two major families. Classify the `TASK_GOAL` before writing a design or editing code, because the expected files, validation strategy, and coding rules differ.

## 1. Basic And High-Level API Requirements

Use this route when the task asks to add, extend, optimize, or fix AscendC basic APIs, high-level APIs, operator helper APIs, or API behavior exposed to kernel authors.

Typical wording:

- "实现一个 AscendC API"
- "给某 API 增加 dtype 支持"
- "扩展 basic API / high-level API"
- "修改 Dumptensor / Sign / Abs / Cast / Reduce / DataCopy 等 API 行为"
- "在 autofuse 或 kernel API 框架下补能力"

Primary devkit areas:

- `${DEVKIT_PATH}/include`
- `${DEVKIT_PATH}/impl`

Useful files to inspect in the user's devkit:

- `${DEVKIT_PATH}/include/basic_api/kernel_operator_vec_*.h`
- `${DEVKIT_PATH}/include/basic_api/kernel_operator_*.h`
- matching API implementation files under `${DEVKIT_PATH}/impl`
- existing APIs with similar type, mask, repeat, tmp-buffer, or pipeline behavior

Design expectations:

- Start from the public API declaration and existing implementation style in the devkit repository.
- Do not assume a fixed function signature. Match the API level and overload style already used nearby.
- Distinguish native API layers:
  - Level 0 with mask array, `repeatTime`, and repeat params.
  - Level 0 with scalar mask.
  - Simplified count-based API, often the best model for extension APIs.
  - mixed input/output type APIs such as cast-style APIs.
  - reduce APIs that require `sharedTmpBuffer`.
- Keep API code in `AscendC`-style device-side form: `__aicore__ inline`, `LocalTensor<T>`, repeat params, explicit tmp-buffer use, and pipeline synchronization.
- Use compile-time dispatch with `if constexpr` and type constraints with `static_assert` or local type-trait helpers.
- For unsupported dtypes, prefer explicit compile-time rejection or clearly documented fallback conversion through supported dtypes.
- For in-place support, compare physical addresses such as `dst.GetPhyAddr() == src.GetPhyAddr()` and preserve source data when required.
- For temporary buffers, split `tmp_buf` with 32-byte alignment and validate available size before use.
- For large counts, use repeat-aware batching:
  - max-repeat chunks capped at 255 repeats.
  - medium chunks aligned to one repeat.
  - tail chunks below one repeat.

Synchronization expectations:

- Use `PipeBarrier<PIPE_V>()` between dependent vector operations on the same pipeline.
- Use `SetFlag` / `WaitFlag` for cross-pipeline dependencies such as MTE to Vector when the queue model does not already handle synchronization.
- Do not duplicate manual SetFlag/WaitFlag around `TQue` `EnQue` / `DeQue` paths that already manage synchronization.
- Use cross-core sync only for AIC/AIV split or explicitly cross-core requirements.
- Use memory/system barriers only when the requirement involves memory visibility or ordering beyond normal vector execution.

## 2. AscendC Compile-Engineering Requirements

Use this route when the task asks to change build, compile, metadata, packaging, scripts, generated files, CMake, Python tooling, or C++ host-side compiler support around AscendC.

Typical wording:

- "修改 AscendC 编译流程"
- "生成/解析 .ascend.meta"
- "增加 TLV / DFX / compiler marker"
- "修改构建脚本 / CMake / Python 编译工具"
- "调整 op compile pipeline"
- "修复 devkit tools 下的编译问题"

Primary devkit areas:

- `${DEVKIT_PATH}/tools`
- `${DEVKIT_PATH}/scripts`
- `${DEVKIT_PATH}/cmake`

Useful files to inspect in the user's devkit:

- compiler entry points under `${DEVKIT_PATH}/tools`
- Python build and compile scripts
- CMake modules and toolchain files
- metadata/DFX generation code
- non-test sample build invocations near the changed tool

Design expectations:

- Treat these tasks mostly as ordinary C++, Python, shell, or CMake engineering unless they directly touch device-side AscendC code.
- Preserve existing command-line interfaces, generated file formats, and metadata compatibility unless the task explicitly changes them.
- For `.ascend.meta`, TLV, DFX, or compile marker work, derive the layout from the repository compiler code; do not invent metadata formats from memory.
- Keep path handling environment-neutral. Use supplied `DEVKIT_PATH`, supplied or resolved `CANN_PATH`, and command arguments instead of fixed local paths.
- Update scripts and CMake with the repository's existing style, variable names, and error-handling conventions.
- Add focused validation commands or tests that match the touched toolchain surface.

## Routing Rule

When a requirement mixes both families, split the plan:

- API/high-level API changes first: `include` and `impl`.
- Compile-engineering support second: `tools`, `scripts`, and `cmake`.
- Shared validation last: compile smoke test, generated metadata check, API behavior check, and regression path.
