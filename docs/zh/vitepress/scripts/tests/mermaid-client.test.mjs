/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'
import { test } from 'node:test'

const clientSource = readFileSync(
  new URL('../../docs/.vitepress/theme/mermaid.mjs', import.meta.url),
  'utf8'
)

test('loads the Mermaid browser bundle from the Vite deployment base', () => {
  assert.match(
    clientSource,
    /script\.src\s*=\s*`\$\{import\.meta\.env\.BASE_URL\}assets\/mermaid\.min\.js`/
  )
  assert.doesNotMatch(clientSource, /script\.src\s*=\s*['"]\/assets\/mermaid\.min\.js['"]/)
})
