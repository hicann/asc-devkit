# GroupBarrier Usage<a name="ZH-CN_TOPIC_0000001960225008"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:42:03.579Z -->

When a dependency exists between two AIV tasks in the same [CubeResGroupHandle](../CubeResGroupHandle/CubeResGroupHandle.md), **GroupBarrier** can be used to control synchronization. Suppose that after a group of AIVs (group A) completes task x, another group of AIVs (group B) can start subsequent operations. Group A is called the Arrive group, and group B is called the Wait group.

The steps for group synchronization based on **GroupBarrier** are as follows:

1.  Create a **GroupBarrier**.
2.  The AIVs being waited for call **Arrive**, and the AIVs that need to wait call **Wait**.

1.  Create a **GroupBarrier**.

    ```
    constexpr int32_t ARRIVE_NUM = 2; // Number of AIVs in the Arrive group
    constexpr int32_t WAIT_NUM = 6; // Number of AIVs in the Wait group
    // Create a GroupBarrier. The user manages and clears this part of the workspace.
    AscendC::GroupBarrier<AscendC::PipeMode::MTE3_MODE> barA(workspace, ARRIVE_NUM, WAIT_NUM);
    ```

2.  The AIVs being waited for call **Arrive**, and the AIVs that need to wait call **Wait**.

    ```
    auto id = AscendC::GetBlockIdx();
    if (id > 0 && id < ARRIVE_NUM) {
      //Various Vector computation logic, to be implemented by the user.
      barA.Arrive(id);
    } else(id >= ARRIVE_NUM && id < ARRIVE_NUM + WAIT_NUM){
      barA.Wait(id - ARRIVE_NUM);
      // Various Vector computation logic, to be implemented by the user.
    }
    ```

