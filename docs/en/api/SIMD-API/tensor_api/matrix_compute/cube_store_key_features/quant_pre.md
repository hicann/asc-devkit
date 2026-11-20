# In-Transit Quantization

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T03:07:12.710Z pushedAt=2026-10-27T13:03:35.347Z nl=2 -->

In-transit quantization is a data conversion capability applied when cube computation results are moved from the L0C Buffer to Global Memory or the Unified Buffer (UB). Depending on whether the `quant` parameter is passed, the movement from the L0C Buffer to Global Memory/UB automatically selects the no-quantization, direct cast, scalar quantization, or tensor quantization mode.

## Call Form

When the `quant` parameter is not passed, the API selects either non-quantized output or direct cast output based on the source type and destination type:

```cpp
auto atom = make_copy(copy_l0c_to_gm{}).with(fixpipe_params);
copy(atom, dst, src);
```

When a `quant` parameter of the `uint64_t` type is passed, it indicates scalar quantization. The entire C cube shares the same quantization parameter:

```cpp
uint64_t quant = 1;
auto atom = make_copy(copy_l0c_to_gm{}).with(fixpipe_params);
copy(atom, dst, src, quant);
```

When a Tensor API tensor is passed as the `quant` parameter, it indicates tensor quantization. The quantization parameter is usually located in the L1 Buffer, with an element type of `uint64_t`, and each channel dimension corresponds to one quantization parameter:

```cpp
auto quant = make_tensor(make_mem_ptr(quant_addr),
    make_frame_layout<nd_ext_layout_ptn>(1, n));
auto atom = make_copy(copy_l0c_to_gm{}).with(fixpipe_params);
copy(atom, dst, src, quant);
```

## Quantization Parameters

The element type of both scalar quantization parameters and tensor quantization parameters is `uint64_t`.

## Constraints

- The tensor quantization parameter tensor should reside in the L1 Buffer, with an element type of `uint64_t` and a 32-byte aligned address.
- Quantization parameters cannot be `inf`, `nan`, or denormalized numbers. This is a hardware constraint, and the API does not check the parameter values separately.
- `round_mode::hybrid` is supported only in quantization output scenarios where the source type is `float` and the destination type is `hifloat8_t`.
- When `quant` is not passed, only non-quantized output or direct cast output from `float` to `half`/`bfloat16_t` is supported.
