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
import { execFile } from 'node:child_process'
import { chmod, mkdtemp, readFile, rm, writeFile } from 'node:fs/promises'
import { tmpdir } from 'node:os'
import { join, resolve } from 'node:path'
import { test } from 'node:test'
import { promisify } from 'node:util'

const execFileAsync = promisify(execFile)
const benchmark = resolve(import.meta.dirname, '../bin/asc-search-benchmark.mjs')

function processExists(pid) {
  try {
    process.kill(pid, 0)
    return true
  }
  catch (error) {
    if (error.code === 'ESRCH') return false
    throw error
  }
}

test('cleans up Chromium when CDP initialization fails', async () => {
  const testDirectory = await mkdtemp(join(tmpdir(), 'asc-search-browser-test-'))
  const chromium = join(testDirectory, 'fake-chromium.mjs')
  const statePath = join(testDirectory, 'state.json')
  let state
  try {
    await writeFile(chromium, `#!/usr/bin/env node
import { writeFileSync } from 'node:fs'
if (process.argv[2] === '--version') {
  console.log('Fake Chromium 1.0')
  process.exit(0)
}
const userDataDir = process.argv.find(arg => arg.startsWith('--user-data-dir=')).split('=')[1]
writeFileSync(${JSON.stringify(statePath)}, JSON.stringify({ args: process.argv.slice(2), pid: process.pid, userDataDir }))
console.error('DevTools listening on ws://127.0.0.1:9/devtools/browser/fake')
setInterval(() => {}, 1000)
`, 'utf8')
    await chmod(chromium, 0o755)

    await assert.rejects(
      execFileAsync(process.execPath, [
        benchmark,
        '--url', 'http://127.0.0.1:9/',
        '--chromium', chromium,
        '--timeout', '250',
        '--no-link-check',
      ], { timeout: 5000 }),
      /Timed out waiting for a Chromium page target/,
    )

    state = JSON.parse(await readFile(statePath, 'utf8'))
    assert.equal(state.args.includes('--no-sandbox'), false)
    assert.equal(processExists(state.pid), false)
    await assert.rejects(readFile(state.userDataDir), { code: 'ENOENT' })
  }
  finally {
    if (!state) {
      try {
        state = JSON.parse(await readFile(statePath, 'utf8'))
      }
      catch {}
    }
    if (state && processExists(state.pid)) process.kill(state.pid, 'SIGKILL')
    await rm(testDirectory, { recursive: true, force: true })
  }
})
