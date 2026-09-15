# Init<a name="ZH-CN_TOPIC_0000001717287036"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:33:41.338Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Used to initialize the memory and synchronization pipeline event **EventID**.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void Init()
```

## Constraints<a name="section633mcpsimp"></a>

-   When a **Tpipe** object is reused, it must be used in pairs with the [Destroy](Destroy.md) API. For each reuse, call **Destroy** first to release resources, and then call **Init** to reinitialize.
-   The **Tpipe** constructor already calls **Init** internally, so there is no need to call it again after the **Tpipe** object is initialized.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>


```
// Instantiate a custom operator object of the float type.
KernelTPipeInit<float> op;
uint32_t srcSize = 128;

// Construct a TPipe object.
AscendC::TPipe tpipe;

// Execute the process for the first time.
op.Init(x, z, srcSize, &tpipe);
op.Process();
tpipe.Destroy();  // To reuse it, call Destroy before Init.

// Execute the process for the second time: reuse the operator object and the tpipe object.
tpipe.Init();
op.Init(x, z, srcSize, &tpipe);
op.Process();
tpipe.Destroy();  // Destroy the tpipe resources.
```
