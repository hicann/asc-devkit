/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

let mermaidPromise
let renderQueue = Promise.resolve()

function initializeMermaid(resolve, reject) {
  const mermaid = globalThis.mermaid
  if (!mermaid) {
    reject(new Error('Mermaid browser bundle did not expose globalThis.mermaid'))
    return
  }

  try {
    mermaid.initialize({
      startOnLoad: false,
      securityLevel: 'strict',
      theme: 'default',
    })
    resolve(mermaid)
  } catch (error) {
    reject(error)
  }
}

function loadMermaid() {
  mermaidPromise ||= new Promise((resolve, reject) => {
    if (globalThis.mermaid) {
      initializeMermaid(resolve, reject)
      return
    }

    const script = document.createElement('script')
    script.src = `${import.meta.env.BASE_URL}assets/mermaid.min.js`
    script.async = true
    script.onload = () => initializeMermaid(resolve, reject)
    script.onerror = () => reject(new Error(`Failed to load ${script.src}`))
    document.head.appendChild(script)
  })
  return mermaidPromise
}

async function renderPendingDiagrams() {
  if (typeof document === 'undefined') return

  const diagrams = Array.from(
    document.querySelectorAll('[data-mermaid-diagram] > .mermaid:not([data-processed])')
  )
  if (diagrams.length === 0) return

  for (const diagram of diagrams) diagram.classList.add('is-rendering')

  try {
    const mermaid = await loadMermaid()
    await mermaid.run({ nodes: diagrams, suppressErrors: true })
  } catch (error) {
    console.error('Failed to render Mermaid diagram', error)
  } finally {
    for (const diagram of diagrams) {
      diagram.classList.remove('is-rendering')
      if (!diagram.hasAttribute('data-processed')) {
        diagram.classList.add('mermaid-error')
      }
    }
  }
}

export function renderMermaidDiagrams() {
  renderQueue = renderQueue.catch(() => {}).then(renderPendingDiagrams)
  return renderQueue
}
