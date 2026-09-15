# TPipe Introduction<a name="ZH-CN_TOPIC_0000002417498765"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:39:30.384Z -->

**TPipe** is used to uniformly manage resources such as memory on the **Device** side. A **Kernel** function must and can only initialize one **TPipe** object. Its main functions include:

-   **Memory resource management**: Through the **InitBuffer** API of **TPipe**, memory can be allocated for **TQue** and **TBuf**, which are used for queue memory initialization and temporary variable memory initialization, respectively.
-   **Synchronization event management**: Through APIs such as **AllocEventID** and **ReleaseEventID** of **TPipe**, event IDs can be applied for and released for synchronization control.

