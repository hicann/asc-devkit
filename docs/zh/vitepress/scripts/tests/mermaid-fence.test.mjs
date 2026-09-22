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
import { test } from 'node:test'

import { installMermaidFence } from '../mermaid-fence.mjs'

function createRenderer() {
  const fallbackCalls = []
  const md = {
    renderer: {
      rules: {
        fence(tokens, idx) {
          fallbackCalls.push(tokens[idx].info)
          return '<div class="ordinary-code"></div>\n'
        },
      },
    },
    utils: {
      escapeHtml(value) {
        return value
          .replace(/&/g, '&amp;')
          .replace(/</g, '&lt;')
          .replace(/>/g, '&gt;')
          .replace(/"/g, '&quot;')
      },
    },
  }
  installMermaidFence(md)
  return { md, fallbackCalls }
}

test('renders Mermaid fences as dedicated diagram containers', () => {
  const { md, fallbackCalls } = createRenderer()
  const html = md.renderer.rules.fence([
    { info: 'mermaid', content: 'graph TD\n  A["<start>"] --> B & C\n' },
  ], 0, {}, {}, {})

  assert.equal(
    html,
    '<div class="mermaid-diagram" data-mermaid-diagram><pre class="mermaid">graph TD\n' +
      '  A[&quot;&lt;start&gt;&quot;] --&gt; B &amp; C</pre></div>\n'
  )
  assert.deepEqual(fallbackCalls, [])
})

test('leaves non-Mermaid fences on the existing renderer', () => {
  const { md, fallbackCalls } = createRenderer()
  const html = md.renderer.rules.fence([
    { info: 'cpp', content: 'int main() {}\n' },
  ], 0, {}, {}, {})

  assert.equal(html, '<div class="ordinary-code"></div>\n')
  assert.deepEqual(fallbackCalls, ['cpp'])
})
