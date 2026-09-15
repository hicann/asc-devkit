# Pointer

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:03:34.590Z -->

> [!NOTE]Note
> This is an experimental API that may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers should pay attention to updates in later versions during use.

## Description

**Pointer** defines a pointer iterator for traversing and accessing tensor data.

## Structure Definition

```cpp
template <typename T, typename P>
struct Pointer {
    T* ptr;
    P position;
};
```

## Fields

| Field | Type | Description |
|--------|------|------|
| **ptr** | **T*** | Pointer to the data. |
| **position** | **P** | Position information of the pointer. |

## Constraints

- **ptr** must point to valid memory space.
- **position** must correctly describe the current position of the pointer.
- **Pointer** supports pointer types for multiple memory spaces.

## Example

```cpp
constexpr int32_t size = 256;
__ubuf__ float ubData[size];
__cbuf__ float l1Data[size];
__ca__ float l0AData[size];

// Create a Unified Buffer pointer.
auto ubPtr = AscendC::Te::MakeMemPtr(ubData);

// Create an L1 Buffer pointer.
auto l1Ptr = AscendC::Te::MakeMemPtr(l1Data);

// Create an L0A Buffer pointer.
auto caPtr = AscendC::Te::MakeMemPtr(l0AData);
```
