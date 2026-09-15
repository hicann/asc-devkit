# In-line Quantization

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:10:14.521Z -->

In-line quantization is a data conversion capability applied when cube computation results are moved from the **L0C Buffer** to **Global Memory** or the **Unified Buffer**. During the transfer from the **L0C Buffer** to **Global Memory**/**Unified Buffer**, the system automatically selects the no-quantization, direct cast, scalar quantization, or tensor quantization mode based on whether the `quant` parameter is passed.

## Invocation Forms

When the `quant` parameter is not passed, the API selects non-quantized output or direct cast output based on the source type and destination type:

```cpp
auto atom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(atom, dst, src);
```

When a `quant` parameter of the `uint64_t` type is passed, it indicates scalar quantization. The entire **C** cube shares the same quantization parameter:

```cpp
uint64_t quant = 1;
auto atom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(atom, dst, src, quant);
```

When a Tensor API tensor is passed as the `quant` parameter, it indicates tensor quantization. The quantization parameters are usually located in the **L1 Buffer**, with an element type of `uint64_t`, and each channel dimension corresponds to one quantization parameter:

```cpp
auto quant = MakeTensor(MakeMemPtr(quantAddr),
    MakeFrameLayout<NDExtLayoutPtn>(1, n));
auto atom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(atom, dst, src, quant);
```

## Quantization Parameter

The element type of both the scalar quantization parameter and the tensor quantization parameter is `uint64_t`.

## Constraints

- The tensor quantization parameter tensor must be located in the L1 Buffer, with an element type of `uint64_t` and a 32-byte aligned address.
- Quantization parameters cannot be `inf`, `nan`, or denormalized numbers. This is a hardware constraint, and the API does not check the parameter values separately.
- `RoundMode::HYBRID` is supported only in quantization output scenarios where the source type is `float` and the destination type is `hifloat8_t`.
- When `quant` is not passed, only non-quantized output or direct cast output from `float` to `half`/`bfloat16_t` is supported.
