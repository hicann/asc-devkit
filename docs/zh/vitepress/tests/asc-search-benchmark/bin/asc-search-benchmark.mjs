#!/usr/bin/env node

/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import { spawn, spawnSync } from 'node:child_process'
import { createServer } from 'node:http'
import { accessSync, constants, createReadStream, existsSync, statSync } from 'node:fs'
import { mkdir, mkdtemp, readFile, rm, writeFile } from 'node:fs/promises'
import { tmpdir } from 'node:os'
import { dirname, extname, isAbsolute, join, relative, resolve, sep } from 'node:path'
import { fileURLToPath } from 'node:url'

import {
  assignUniqueTargetGrades,
  escapeMarkdownCell as escapeCell,
  fetchSameOrigin,
  markdownReportPath,
  pathMatchesPrefix,
  RESULT_HIGHLIGHT_SELECTOR,
  resolveFileWithinRoot,
  resolveSameOriginHttpUrl,
  SECTION_HIGHLIGHT_SELECTOR,
} from '../lib/benchmark-utils.mjs'

const REPOSITORY_ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..')
const DEFAULT_CASES = resolve(REPOSITORY_ROOT, 'cases/ascendc.json')
const DEFAULT_OUTPUT = resolve(REPOSITORY_ROOT, 'results/current.json')
const DEFAULT_BUILD_COMMAND = 'bash docs/zh/vitepress/deploy.sh'
const DEFAULT_DIST_RELATIVE = 'docs/zh/vitepress/docs/.vitepress/dist'
const DEFAULT_VIEWPORT_WIDTH = 800
const DEFAULT_VIEWPORT_HEIGHT = 600
const POLL_INTERVAL_MS = 20
const QUERY_TIMEOUT_MS = 15000
const PRODUCT_FILTER_VALUES = new Set(['all', '950', 'A3', '910b', '910', '310p', '310b', 'x90', '9030'])

function printHelp() {
  console.log(`Usage:
  asc-search-benchmark --url <site-url> [options]
  asc-search-benchmark --dist <static-site-dir> [options]
  asc-search-benchmark --repo <path-or-url> --commit <ref> [options]

Target options (choose exactly one):
  --url <url>                 Test a deployed site, for example https://asc.gitcode.com/
  --dist <path>               Serve and test an existing static build
  --repo <path-or-url>        Clone an Ascend C documentation repository into a temporary directory
  --commit <ref>              Commit or ref to check out; required with --repo
  --build-command <command>   Build command run at repository root
                              (default: ${DEFAULT_BUILD_COMMAND})
  --dist-relative <path>      Build output relative to checkout root
                              (default: ${DEFAULT_DIST_RELATIVE})
  --keep-checkout             Keep the temporary checkout after a commit test

Benchmark options:
  --cases <path>              Cases JSON (default: cases/ascendc.json)
  --case <id>                 Run one case; repeat to run more cases
  --chromium <path>           Chromium or Chrome executable
  --no-sandbox                Disable the Chromium sandbox (isolated environments only)
  --enforce                   Exit with code 2 when configured thresholds fail
  --no-link-check             Do not verify Top 10 result URLs
  --output <path>             JSON report path (default: results/current.json)
  --timeout <ms>              Browser startup and page initialization timeout (default: 180000)
  --help                      Show this help
`)
}

function parseArgs(argv) {
  const options = {
    buildCommand: DEFAULT_BUILD_COMMAND,
    cases: DEFAULT_CASES,
    caseIds: [],
    distRelative: DEFAULT_DIST_RELATIVE,
    enforce: false,
    keepCheckout: false,
    linkCheck: true,
    noSandbox: false,
    output: DEFAULT_OUTPUT,
    timeoutMs: 180000,
  }
  for (let index = 0; index < argv.length; index++) {
    const argument = argv[index]
    if (argument === '--help' || argument === '-h') {
      printHelp()
      process.exit(0)
    }
    if (argument === '--enforce') {
      options.enforce = true
      continue
    }
    if (argument === '--keep-checkout') {
      options.keepCheckout = true
      continue
    }
    if (argument === '--no-link-check') {
      options.linkCheck = false
      continue
    }
    if (argument === '--no-sandbox') {
      options.noSandbox = true
      continue
    }
    if (!argument.startsWith('--')) throw new Error(`Unknown argument: ${argument}`)
    const value = argv[++index]
    if (!value) throw new Error(`Missing value for ${argument}`)
    if (argument === '--build-command') options.buildCommand = value
    else if (argument === '--case') options.caseIds.push(value)
    else if (argument === '--cases') options.cases = resolve(value)
    else if (argument === '--chromium') options.chromium = resolve(value)
    else if (argument === '--commit') options.commit = value
    else if (argument === '--dist') options.dist = resolve(value)
    else if (argument === '--dist-relative') options.distRelative = value
    else if (argument === '--output') options.output = resolve(value)
    else if (argument === '--repo') options.repo = value
    else if (argument === '--timeout') options.timeoutMs = Number(value)
    else if (argument === '--url') options.url = value
    else throw new Error(`Unknown option: ${argument}`)
  }

  const targetCount = [options.url, options.dist, options.repo].filter(Boolean).length
  if (targetCount !== 1) throw new Error('Choose exactly one target: --url, --dist, or --repo')
  if (options.repo && !options.commit) throw new Error('--commit is required with --repo')
  if (options.commit && !options.repo) throw new Error('--commit may only be used with --repo')
  if (isAbsolute(options.distRelative)) throw new Error('--dist-relative must be relative to the checkout root')
  if (!Number.isFinite(options.timeoutMs) || options.timeoutMs <= 0) {
    throw new Error('--timeout must be a positive number')
  }
  if (options.url) options.url = new URL(options.url).href
  return options
}

function run(command, args, options = {}) {
  const result = spawnSync(command, args, {
    cwd: options.cwd,
    encoding: 'utf8',
    stdio: options.inherit ? 'inherit' : ['ignore', 'pipe', 'pipe'],
  })
  if (result.status !== 0) {
    const details = options.inherit ? '' : `\n${result.stderr || result.stdout || ''}`
    throw new Error(`${command} ${args.join(' ')} failed with exit code ${result.status}${details}`)
  }
  return String(result.stdout || '').trim()
}

function findChromium(explicitPath) {
  const candidates = explicitPath
    ? [explicitPath]
    : ['/snap/bin/chromium', '/usr/bin/chromium', '/usr/bin/chromium-browser', '/usr/bin/google-chrome']
  for (const candidate of candidates) {
    try {
      accessSync(candidate, constants.X_OK)
      return candidate
    }
    catch {}
  }
  for (const command of ['chromium', 'chromium-browser', 'google-chrome']) {
    const result = spawnSync('which', [command], { encoding: 'utf8' })
    if (result.status === 0 && result.stdout.trim()) return result.stdout.trim()
  }
  throw new Error('Chromium was not found. Pass its executable path with --chromium.')
}

const CONTENT_TYPES = {
  '.css': 'text/css; charset=utf-8',
  '.gif': 'image/gif',
  '.html': 'text/html; charset=utf-8',
  '.ico': 'image/x-icon',
  '.jpeg': 'image/jpeg',
  '.jpg': 'image/jpeg',
  '.js': 'text/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.mjs': 'text/javascript; charset=utf-8',
  '.pagefind': 'application/octet-stream',
  '.png': 'image/png',
  '.svg': 'image/svg+xml',
  '.wasm': 'application/wasm',
  '.woff': 'font/woff',
  '.woff2': 'font/woff2',
}

function safePath(root, requestedPath) {
  const candidate = resolve(root, `.${requestedPath}`)
  const relativePath = relative(root, candidate)
  if (relativePath === '..' || relativePath.startsWith(`..${sep}`) || isAbsolute(relativePath)) return null
  return candidate
}

function resolveStaticFile(root, requestedPath) {
  const candidate = safePath(root, requestedPath)
  if (!candidate) return null
  const attempts = [candidate]
  if (extname(candidate) === '') attempts.push(`${candidate}.html`, join(candidate, 'index.html'))
  return resolveFileWithinRoot(root, attempts)
}

function startStaticServer(distDir) {
  const server = createServer((request, response) => {
    try {
      const url = new URL(request.url || '/', 'http://127.0.0.1')
      const pathname = decodeURIComponent(url.pathname)
      const filePath = resolveStaticFile(distDir, pathname)
      if (!filePath) {
        response.writeHead(404, { 'content-type': 'text/plain; charset=utf-8' })
        response.end('Not found')
        return
      }
      const stat = statSync(filePath)
      response.writeHead(200, {
        'cache-control': 'no-store',
        'content-length': stat.size,
        'content-type': CONTENT_TYPES[extname(filePath).toLowerCase()] || 'application/octet-stream',
      })
      if (request.method === 'HEAD') response.end()
      else createReadStream(filePath).pipe(response)
    }
    catch (error) {
      response.writeHead(500, { 'content-type': 'text/plain; charset=utf-8' })
      response.end(error.stack || String(error))
    }
  })
  return new Promise((resolvePromise, reject) => {
    server.once('error', reject)
    server.listen(0, '127.0.0.1', () => {
      const address = server.address()
      resolvePromise({ server, url: `http://127.0.0.1:${address.port}/` })
    })
  })
}

function closeServer(server) {
  return new Promise((resolvePromise, reject) => {
    server.close(error => error ? reject(error) : resolvePromise())
  })
}

async function prepareTarget(options) {
  let checkoutRoot = null
  let temporaryRoot = null
  let distDir = options.dist || null
  const metadata = {}

  async function cleanTemporaryCheckout() {
    if (!temporaryRoot) return
    if (options.keepCheckout) console.log(`Temporary checkout kept at ${checkoutRoot}`)
    else await rm(temporaryRoot, { recursive: true, force: true })
  }

  try {
    if (options.repo) {
      temporaryRoot = await mkdtemp(join(tmpdir(), 'asc-search-commit-'))
      checkoutRoot = join(temporaryRoot, 'repository')
      const localRepository = existsSync(options.repo)
      const repository = localRepository ? resolve(options.repo) : options.repo
      console.log(`Preparing ${options.commit} from ${repository}...`)
      run('git', ['clone', ...(localRepository ? ['--shared'] : []), '--no-checkout', repository, checkoutRoot], { inherit: true })
      run('git', ['checkout', '--detach', options.commit], { cwd: checkoutRoot, inherit: true })
      const resolvedCommit = run('git', ['rev-parse', 'HEAD'], { cwd: checkoutRoot })
      console.log(`Building ${resolvedCommit} with: ${options.buildCommand}`)
      run('/bin/bash', ['-lc', options.buildCommand], { cwd: checkoutRoot, inherit: true })
      distDir = resolve(checkoutRoot, options.distRelative)
      metadata.repository = repository
      metadata.requestedCommit = options.commit
      metadata.resolvedCommit = resolvedCommit
      metadata.buildCommand = options.buildCommand
    }

    if (distDir) {
      if (!existsSync(distDir) || !statSync(distDir).isDirectory()) {
        throw new Error(`Static site directory does not exist: ${distDir}`)
      }
      const { server, url } = await startStaticServer(distDir)
      return {
        url,
        distDir,
        metadata: {
          mode: options.repo ? 'commit' : 'dist',
          distPath: distDir,
          ...metadata,
        },
        async dispose() {
          await closeServer(server)
          await cleanTemporaryCheckout()
        },
      }
    }

    return {
      url: options.url,
      distDir: null,
      metadata: { mode: 'url', deployedUrl: options.url },
      async dispose() {},
    }
  }
  catch (error) {
    await cleanTemporaryCheckout()
    throw error
  }
}

function delay(ms) {
  return new Promise(resolvePromise => setTimeout(resolvePromise, ms))
}

function connectCdp(webSocketUrl) {
  if (typeof WebSocket !== 'function') {
    throw new Error('Node.js 22 or newer is required for the built-in WebSocket API.')
  }
  return new Promise((resolvePromise, reject) => {
    const socket = new WebSocket(webSocketUrl)
    const pending = new Map()
    const listeners = new Map()
    let commandId = 0
    socket.addEventListener('open', () => {
      resolvePromise({
        command(method, params = {}) {
          return new Promise((resolveCommand, rejectCommand) => {
            const id = ++commandId
            pending.set(id, { resolve: resolveCommand, reject: rejectCommand })
            socket.send(JSON.stringify({ id, method, params }))
          })
        },
        on(method, listener) {
          if (!listeners.has(method)) listeners.set(method, new Set())
          listeners.get(method).add(listener)
          return () => listeners.get(method)?.delete(listener)
        },
        close() {
          socket.close()
        },
      })
    }, { once: true })
    socket.addEventListener('message', (event) => {
      const message = JSON.parse(String(event.data))
      if (message.id && pending.has(message.id)) {
        const command = pending.get(message.id)
        pending.delete(message.id)
        if (message.error) command.reject(new Error(message.error.message))
        else command.resolve(message.result)
        return
      }
      for (const listener of listeners.get(message.method) || []) listener(message.params)
    })
    socket.addEventListener('error', () => reject(new Error(`Unable to connect to Chromium DevTools at ${webSocketUrl}`)), { once: true })
    socket.addEventListener('close', () => {
      for (const command of pending.values()) command.reject(new Error('Chromium DevTools connection closed'))
      pending.clear()
    })
  })
}

async function findPageTarget(debuggerPort, deadline) {
  const endpoint = `http://127.0.0.1:${debuggerPort}/json/list`
  while (Date.now() < deadline) {
    try {
      const targets = await fetch(endpoint).then(response => response.json())
      const target = targets.find(item => item.type === 'page')
      if (target?.webSocketDebuggerUrl) return target.webSocketDebuggerUrl
    }
    catch {}
    await delay(50)
  }
  throw new Error('Timed out waiting for a Chromium page target')
}

async function terminateChild(child, signal = 'SIGTERM') {
  if (!child?.pid || child.exitCode !== null || child.signalCode !== null) return
  const exited = new Promise(resolvePromise => child.once('exit', resolvePromise))
  if (!child.kill(signal)) return
  if (signal === 'SIGKILL') {
    await exited
    return
  }
  let timeout
  const exitedGracefully = await Promise.race([
    exited.then(() => true),
    new Promise(resolvePromise => {
      timeout = setTimeout(() => resolvePromise(false), 1000)
    }),
  ])
  clearTimeout(timeout)
  if (!exitedGracefully && child.exitCode === null && child.signalCode === null) {
    if (child.kill('SIGKILL')) await exited
  }
}

async function cleanupBrowser(client, child, userDataDir, signal = 'SIGTERM') {
  try {
    client?.close()
  }
  finally {
    try {
      await terminateChild(child, signal)
    }
    finally {
      await rm(userDataDir, { recursive: true, force: true })
    }
  }
}

async function launchBrowser(chromium, timeoutMs, noSandbox = false) {
  const userDataDir = await mkdtemp(join(tmpdir(), 'asc-search-browser-'))
  let child
  let client
  try {
    const args = [
      '--headless',
      '--disable-gpu',
      '--disable-dev-shm-usage',
      '--disable-background-timer-throttling',
      '--disable-renderer-backgrounding',
      '--remote-debugging-port=0',
      `--user-data-dir=${userDataDir}`,
      ...(noSandbox ? ['--no-sandbox'] : []),
      'about:blank',
    ]
    child = spawn(chromium, args, { stdio: ['ignore', 'ignore', 'pipe'] })
    child.stderr.setEncoding('utf8')
    let stderr = ''
    const deadline = Date.now() + timeoutMs
    let debuggerPort
    while (!debuggerPort && Date.now() < deadline) {
      const chunk = await new Promise((resolvePromise, reject) => {
        const timeout = setTimeout(() => resolvePromise(''), 100)
        child.stderr.once('data', (data) => {
          clearTimeout(timeout)
          resolvePromise(data)
        })
        child.once('error', reject)
      })
      stderr += chunk
      debuggerPort = stderr.match(/DevTools listening on ws:\/\/127\.0\.0\.1:(\d+)\//)?.[1]
      if (child.exitCode !== null) throw new Error(`Chromium exited during startup (${child.exitCode})\n${stderr}`)
    }
    if (!debuggerPort) throw new Error(`Timed out waiting for Chromium DevTools after ${timeoutMs}ms\n${stderr}`)
    client = await connectCdp(await findPageTarget(debuggerPort, deadline))
    await client.command('Runtime.enable')
    await client.command('Page.enable')
    return {
      client,
      child,
      userDataDir,
      async dispose() {
        await cleanupBrowser(client, child, userDataDir)
      },
    }
  }
  catch (error) {
    await cleanupBrowser(client, child, userDataDir, 'SIGKILL').catch((cleanupError) => {
      error.cause = cleanupError
    })
    throw error
  }
}

async function evaluate(client, expression) {
  const response = await client.command('Runtime.evaluate', {
    expression,
    awaitPromise: true,
    returnByValue: true,
  })
  if (response.exceptionDetails) {
    const description = response.exceptionDetails.exception?.description || response.exceptionDetails.text
    throw new Error(description || 'Browser evaluation failed')
  }
  return response.result.value
}

async function waitForBrowser(client, expression, timeoutMs, description) {
  const deadline = Date.now() + timeoutMs
  while (Date.now() < deadline) {
    try {
      if (await evaluate(client, expression)) return
    }
    catch {}
    await delay(50)
  }
  throw new Error(`Timed out waiting for ${description} after ${timeoutMs}ms`)
}

async function initializeSearchPage(client, targetUrl, timeoutMs) {
  await client.command('Page.navigate', { url: targetUrl })
  const origin = new URL(targetUrl).origin
  await waitForBrowser(client, `location.origin === ${JSON.stringify(origin)} && document.readyState === 'complete'`, timeoutMs, targetUrl)
  const initStarted = performance.now()
  const payload = await evaluate(client, `(async () => {
    const sleep = ms => new Promise(resolve => setTimeout(resolve, ms))
    const waitFor = async (check, timeout, description) => {
      const started = performance.now()
      while (performance.now() - started < timeout) {
        const value = check()
        if (value) return value
        await sleep(${POLL_INTERVAL_MS})
      }
      throw new Error('Timed out waiting for ' + description + ' after ' + timeout + 'ms')
    }
    window.__ascSearchBenchmark = { errors: [] }
    addEventListener('unhandledrejection', event => {
      window.__ascSearchBenchmark.errors.push(event.reason?.stack || event.reason?.message || String(event.reason))
    })
    addEventListener('error', event => {
      window.__ascSearchBenchmark.errors.push(event.error?.stack || event.message || String(event.error))
    })
    const button = await waitFor(() => document.querySelector('.nav-search-btn-wait'), ${timeoutMs}, 'the search button')
    await waitFor(() => window.__pagefind__?.search, ${timeoutMs}, 'Pagefind initialization')
    button.click()
    await waitFor(() => document.querySelector('input[command-input]'), ${timeoutMs}, 'the search input')
    return { userAgent: navigator.userAgent, title: document.title }
  })()`)
  return { ...payload, initMs: performance.now() - initStarted }
}

async function runSearchCase(client, definition) {
  return evaluate(client, `(async () => {
    const definition = ${JSON.stringify({
      id: definition.id,
      query: definition.query,
      productFilter: definition.productFilter || 'all',
    })}
    const sleep = ms => new Promise(resolve => setTimeout(resolve, ms))
    const waitFor = async (check, timeout, description) => {
      const started = performance.now()
      while (performance.now() - started < timeout) {
        const value = check()
        if (value) return value
        await sleep(${POLL_INTERVAL_MS})
      }
      throw new Error('Timed out waiting for ' + description + ' after ' + timeout + 'ms')
    }
    const input = document.querySelector('input[command-input]')
    if (!input) throw new Error('Search input is no longer available')
    const productSelect = document.querySelector('.filter-select')
    if (!productSelect) throw new Error('Product filter is no longer available')
    const setter = Object.getOwnPropertyDescriptor(HTMLInputElement.prototype, 'value').set
    const selectSetter = Object.getOwnPropertyDescriptor(HTMLSelectElement.prototype, 'value').set
    const setValue = value => {
      setter.call(input, value)
      input.dispatchEvent(new Event('input', { bubbles: true }))
    }
    const readHighlights = (element, selector) => Array.from(element.querySelectorAll(selector)).map(mark => {
      const style = getComputedStyle(mark)
      const rect = mark.getBoundingClientRect()
      const containerRect = mark.closest('.des, .headings, .section-title')?.getBoundingClientRect()
      const intersectionWidth = containerRect
        ? Math.max(0, Math.min(rect.right, containerRect.right) - Math.max(rect.left, containerRect.left))
        : 0
      const intersectionHeight = containerRect
        ? Math.max(0, Math.min(rect.bottom, containerRect.bottom) - Math.max(rect.top, containerRect.top))
        : 0
      const visibleRatio = rect.width > 0 && rect.height > 0
        ? intersectionWidth * intersectionHeight / (rect.width * rect.height)
        : 0
      return {
        text: mark.textContent || '',
        backgroundColor: style.backgroundColor,
        color: style.color,
        textDecorationLine: style.textDecorationLine,
        visibleRatio: Number(visibleRatio.toFixed(4)),
      }
    })
    const sectionPlacementValid = (element) => {
      const headingRect = element.querySelector('.headings')?.getBoundingClientRect()
      const sectionRect = element.querySelector('.section-title')?.getBoundingClientRect()
      const titleRect = element.querySelector('.title')?.getBoundingClientRect()
      if (!headingRect || !sectionRect || !titleRect) return false
      const sameLine = Math.abs(headingRect.bottom - sectionRect.bottom) <= 3
      if (sameLine) {
        const gap = sectionRect.left - headingRect.right
        return gap >= -1 && gap <= 24
      }
      return sectionRect.top >= headingRect.bottom - 2
        && Math.abs(sectionRect.left - titleRect.left) <= 2
    }
    const results = () => Array.from(document.querySelectorAll('[command-item]')).map(element => ({
      route: element.getAttribute('data-route') || element.getAttribute('data-value') || '',
      title: element.querySelector('.headings')?.textContent?.trim() || '',
      sectionTitle: element.querySelector('.section-title')?.textContent?.replace(/\\s+/g, ' ').trim() || '',
      sectionPlacementValid: sectionPlacementValid(element),
      description: element.querySelector('.des')?.textContent?.replace(/\\s+/g, ' ').trim() || '',
      highlights: readHighlights(element, ${JSON.stringify(RESULT_HIGHLIGHT_SELECTOR)}),
      titleHighlights: readHighlights(element, '.headings mark'),
      descriptionHighlights: readHighlights(element, '.des mark'),
      sectionHighlights: readHighlights(element, ${JSON.stringify(SECTION_HIGHLIGHT_SELECTOR)}),
    })).filter(result => result.route)

    selectSetter.call(productSelect, definition.productFilter)
    productSelect.dispatchEvent(new Event('change', { bubbles: true }))
    await waitFor(
      () => productSelect.value === definition.productFilter,
      1000,
      'product filter ' + definition.productFilter
    )
    setValue('')
    await waitFor(() => !document.querySelector('.search-loading') && results().length === 0, 3000, 'search reset')
    window.__ascSearchBenchmark.errors.length = 0
    const started = performance.now()
    setValue(definition.query)
    let timedOut = false
    try {
      await waitFor(() => document.querySelector('.search-loading'), 1500, 'query start ' + definition.id)
      await waitFor(() => !document.querySelector('.search-loading'), ${QUERY_TIMEOUT_MS}, 'query ' + definition.id)
      await sleep(${POLL_INTERVAL_MS})
    }
    catch (error) {
      timedOut = true
      window.__ascSearchBenchmark.errors.push(error.stack || error.message || String(error))
    }
    return {
      id: definition.id,
      query: definition.query,
      latencyMs: performance.now() - started,
      timedOut,
      errors: [...window.__ascSearchBenchmark.errors],
      productFilter: productSelect.value,
      viewportWidth: innerWidth,
      results: results()
    }
  })()`)
}

async function runSectionNavigation(client, definition, resultRank) {
  return evaluate(client, `(async () => {
    const definition = ${JSON.stringify({ id: definition.id, sectionHash: definition.expect.sectionHash })}
    const resultRank = ${resultRank}
    const sleep = ms => new Promise(resolve => setTimeout(resolve, ms))
    const waitFor = async (check, timeout, description) => {
      const started = performance.now()
      while (performance.now() - started < timeout) {
        const value = check()
        if (value) return value
        await sleep(${POLL_INTERVAL_MS})
      }
      throw new Error('Timed out waiting for ' + description + ' after ' + timeout + 'ms')
    }
    const item = document.querySelectorAll('[command-item]')[resultRank]
    if (!item) throw new Error('Section result rank ' + (resultRank + 1) + ' is not available')
    const resultRoute = item.getAttribute('data-route') || ''
    item.click()
    await waitFor(() => {
      try {
        return decodeURIComponent(location.hash) === definition.sectionHash
      }
      catch {
        return false
      }
    }, ${QUERY_TIMEOUT_MS}, 'section hash ' + definition.sectionHash)
    await waitFor(() => {
      try {
        const target = document.getElementById(decodeURIComponent(location.hash).slice(1))
        const rect = target?.getBoundingClientRect()
        return rect && rect.bottom > 0 && rect.top < innerHeight
      }
      catch {
        return false
      }
    }, 3000, 'visible section ' + definition.sectionHash)
    const decodedHash = decodeURIComponent(location.hash)
    const target = document.getElementById(decodedHash.slice(1))
    const rect = target?.getBoundingClientRect()
    return {
      resultRank: resultRank + 1,
      resultRoute,
      hash: decodedHash,
      targetExists: Boolean(target),
      targetVisible: Boolean(rect && rect.bottom > 0 && rect.top < innerHeight),
      targetTop: rect?.top ?? null,
      scrollY,
    }
  })()`)
}

async function readResources(client, origin) {
  return evaluate(client, `performance.getEntriesByType('resource').map(entry => ({
    name: entry.name.replace(${JSON.stringify(origin)}, ''),
    initiatorType: entry.initiatorType,
    transferSize: entry.transferSize || 0,
    encodedBodySize: entry.encodedBodySize || 0,
    decodedBodySize: entry.decodedBodySize || 0,
    durationMs: entry.duration
  }))`)
}

function normalizeRoute(route) {
  if (!route) return ''
  let pathname
  try {
    pathname = new URL(route, 'http://search-benchmark.local').pathname
    pathname = decodeURIComponent(pathname)
  }
  catch {
    pathname = route.split(/[?#]/)[0]
  }
  pathname = pathname.replace(/\.html?$/, '').replace(/\/index$/, '/').replace(/\/$/, '')
  return pathname || '/'
}

function validateDataset(dataset) {
  if (!Array.isArray(dataset.cases) || dataset.cases.length === 0) {
    throw new Error('The benchmark dataset must contain at least one case')
  }
  const ids = new Set()
  for (const definition of dataset.cases) {
    if (!definition.id || ids.has(definition.id)) throw new Error(`Duplicate or missing case id: ${definition.id}`)
    ids.add(definition.id)
    if (!definition.query || !definition.category || !definition.expect) {
      throw new Error(`Case ${definition.id} is missing query, category, or expect`)
    }
    if (definition.viewportWidth !== undefined
      && (!Number.isInteger(definition.viewportWidth) || definition.viewportWidth < 320 || definition.viewportWidth > 3840)) {
      throw new Error(`Case ${definition.id} viewportWidth must be an integer between 320 and 3840`)
    }
    if (definition.productFilter !== undefined && !PRODUCT_FILTER_VALUES.has(definition.productFilter)) {
      throw new Error(`Case ${definition.id} productFilter is not recognized: ${definition.productFilter}`)
    }
    if (!Array.isArray(definition.expect.targets)) throw new Error(`Case ${definition.id} must define expect.targets`)
    if (definition.expect.excludedTargets !== undefined && !Array.isArray(definition.expect.excludedTargets)) {
      throw new Error(`Case ${definition.id} expect.excludedTargets must be an array`)
    }
    for (const key of [
      'descriptionIncludes',
      'highlightIncludes',
      'titleHighlightIncludes',
      'descriptionHighlightIncludes',
      'sectionTitleIncludes',
      'sectionHighlightIncludes',
    ]) {
      const values = definition.expect[key]
      if (values !== undefined
        && (!Array.isArray(values) || values.length === 0
          || values.some(value => typeof value !== 'string' || !value))) {
        throw new Error(`Case ${definition.id} expect.${key} must be a non-empty string array`)
      }
    }
    if (definition.expect.sectionHash !== undefined
      && (typeof definition.expect.sectionHash !== 'string' || !definition.expect.sectionHash.startsWith('#'))) {
      throw new Error(`Case ${definition.id} expect.sectionHash must start with #`)
    }
    if (definition.expect.maxFirstRelevantRank !== undefined
      && (!Number.isInteger(definition.expect.maxFirstRelevantRank)
        || definition.expect.maxFirstRelevantRank < 1
        || definition.expect.maxFirstRelevantRank > 10)) {
      throw new Error(`Case ${definition.id} expect.maxFirstRelevantRank must be an integer between 1 and 10`)
    }
    if (definition.expect.minFirstRelevantRank !== undefined
      && (!Number.isInteger(definition.expect.minFirstRelevantRank)
        || definition.expect.minFirstRelevantRank < 1
        || definition.expect.minFirstRelevantRank > 10)) {
      throw new Error(`Case ${definition.id} expect.minFirstRelevantRank must be an integer between 1 and 10`)
    }
    if (definition.expect.minFirstRelevantRank !== undefined
      && definition.expect.maxFirstRelevantRank !== undefined
      && definition.expect.minFirstRelevantRank > definition.expect.maxFirstRelevantRank) {
      throw new Error(`Case ${definition.id} relevant rank range is empty`)
    }
    for (const target of [...definition.expect.targets, ...(definition.expect.excludedTargets || [])]) {
      const matchers = ['exact', 'prefix', 'contains'].filter(key => target[key] !== undefined)
      if (matchers.length !== 1) throw new Error(`Target in ${definition.id} must have exactly one matcher`)
      if (typeof target[matchers[0]] !== 'string' || !target[matchers[0]]) {
        throw new Error(`Target matcher in ${definition.id} must be a non-empty string`)
      }
      if (![1, 2, 3].includes(target.grade)) throw new Error(`Target in ${definition.id} must have grade 1, 2, or 3`)
    }
  }
}

function targetMatches(route, target) {
  const normalizedRoute = normalizeRoute(route)
  if (target.exact !== undefined) return normalizedRoute === normalizeRoute(target.exact)
  if (target.prefix !== undefined) return pathMatchesPrefix(normalizedRoute, normalizeRoute(target.prefix))
  if (target.contains !== undefined) return normalizedRoute.includes(target.contains)
  return false
}

function relevanceForRoute(route, targets) {
  return targets.reduce((grade, target) => targetMatches(route, target) ? Math.max(grade, target.grade) : grade, 0)
}

function routeHash(route) {
  try {
    return decodeURIComponent(new URL(route, 'http://search-benchmark.local').hash)
  }
  catch {
    return ''
  }
}

function dcg(grades) {
  return grades.reduce((total, grade, index) => total + (2 ** grade - 1) / Math.log2(index + 2), 0)
}

function mean(values) {
  return values.length ? values.reduce((sum, value) => sum + value, 0) / values.length : null
}

function percentile(values, fraction) {
  if (values.length === 0) return null
  const sorted = [...values].sort((left, right) => left - right)
  return sorted[Math.min(sorted.length - 1, Math.ceil(sorted.length * fraction) - 1)]
}

function round(value, digits = 4) {
  if (value === null || value === undefined || Number.isNaN(value)) return null
  return Number(value.toFixed(digits))
}

function evaluateHighlights(highlights, expectedValues) {
  const matches = (item, expected) => String(item.text || '')
    .normalize('NFKC')
    .toLocaleLowerCase()
    .includes(String(expected).normalize('NFKC').toLocaleLowerCase())
  return {
    contentValid: expectedValues.every(expected => highlights.some(item => matches(item, expected))),
    styleValid: expectedValues.every(expected => highlights.some((item) => {
      const background = String(item.backgroundColor || '').replace(/\s+/g, '').toLocaleLowerCase()
      return matches(item, expected)
        && background !== ''
        && background !== 'transparent'
        && background !== 'rgba(0,0,0,0)'
        && item.textDecorationLine === 'none'
    })),
    visibleValid: expectedValues.every(expected => highlights.some(item => (
      matches(item, expected) && Number(item.visibleRatio) >= 0.95
    ))),
  }
}

function evaluateCase(definition, observed, topK, successGrade, routeValidity) {
  const results = observed.results || []
  const targets = definition.expect.targets || []
  const excludedTargets = definition.expect.excludedTargets || []
  const errors = observed.errors || []
  const expectedProductFilter = definition.productFilter || 'all'
  const productFilterValid = observed.productFilter === expectedProductFilter
  const relevantCase = targets.length > 0
  const grades = results.map(result => relevanceForRoute(result.route, targets))
  const firstRelevantIndex = grades.findIndex(grade => grade >= successGrade)
  const maxFirstRelevantRank = definition.expect.maxFirstRelevantRank ?? null
  const firstRelevantRankValid = maxFirstRelevantRank === null
    || (firstRelevantIndex >= 0 && firstRelevantIndex + 1 <= maxFirstRelevantRank)
  const minFirstRelevantRank = definition.expect.minFirstRelevantRank ?? null
  const firstRelevantRankLowEnough = minFirstRelevantRank === null
    || (firstRelevantIndex >= 0 && firstRelevantIndex + 1 >= minFirstRelevantRank)
  const uniqueTargetGrades = assignUniqueTargetGrades(
    results,
    targets,
    topK,
    (result, target) => targetMatches(result.route, target),
  )
  const matchedTargetCount = uniqueTargetGrades.filter(grade => grade > 0).length
  const idealGrades = targets.map(target => target.grade).sort((left, right) => right - left).slice(0, topK)
  const idealDcg = dcg(idealGrades)
  const minResults = definition.expect.minResults ?? 0
  const maxResults = definition.expect.maxResults ?? Number.POSITIVE_INFINITY
  const resultCountValid = results.length >= minResults && results.length <= maxResults
  const runtimeComplete = !observed.timedOut && errors.length === 0
  const targetFound = !relevantCase || firstRelevantIndex >= 0 && firstRelevantIndex < topK
  const expectedDescriptionIncludes = definition.expect.descriptionIncludes || []
  const descriptionResult = firstRelevantIndex >= 0 ? results[firstRelevantIndex] : results[0]
  const normalizedDescription = String(descriptionResult?.description || '').normalize('NFKC').toLocaleLowerCase()
  const descriptionValid = expectedDescriptionIncludes.every(value => normalizedDescription.includes(
    String(value).normalize('NFKC').toLocaleLowerCase()
  ))
  const expectedHighlightIncludes = definition.expect.highlightIncludes || []
  const highlightResult = firstRelevantIndex >= 0 ? results[firstRelevantIndex] : results[0]
  const highlights = highlightResult?.highlights || []
  const highlightValidation = evaluateHighlights(highlights, expectedHighlightIncludes)
  const highlightContentValid = highlightValidation.contentValid
  const highlightStyleValid = highlightValidation.styleValid
  const highlightVisibleValid = highlightValidation.visibleValid
  const expectedTitleHighlightIncludes = definition.expect.titleHighlightIncludes || []
  const titleHighlights = highlightResult?.titleHighlights || []
  const titleHighlightValidation = evaluateHighlights(titleHighlights, expectedTitleHighlightIncludes)
  const expectedDescriptionHighlightIncludes = definition.expect.descriptionHighlightIncludes || []
  const descriptionHighlights = highlightResult?.descriptionHighlights || []
  const descriptionHighlightValidation = evaluateHighlights(
    descriptionHighlights,
    expectedDescriptionHighlightIncludes,
  )
  const expectedSectionTitleIncludes = definition.expect.sectionTitleIncludes || []
  const normalizedSectionTitle = String(highlightResult?.sectionTitle || '').normalize('NFKC').toLocaleLowerCase()
  const sectionTitleValid = expectedSectionTitleIncludes.every(value => normalizedSectionTitle.includes(
    String(value).normalize('NFKC').toLocaleLowerCase()
  ))
  const sectionPlacementValid = expectedSectionTitleIncludes.length === 0
    || Boolean(highlightResult?.sectionPlacementValid)
  const expectedSectionHighlightIncludes = definition.expect.sectionHighlightIncludes || []
  const sectionHighlights = highlightResult?.sectionHighlights || []
  const sectionHighlightValidation = evaluateHighlights(sectionHighlights, expectedSectionHighlightIncludes)
  const expectedSectionHash = definition.expect.sectionHash || ''
  const sectionResult = firstRelevantIndex >= 0 ? results[firstRelevantIndex] : results[0]
  const sectionRouteValid = !expectedSectionHash || routeHash(sectionResult?.route) === expectedSectionHash
  const navigation = observed.navigation || null
  const sectionNavigationValid = !expectedSectionHash || Boolean(
    navigation
    && navigation.hash === expectedSectionHash
    && navigation.targetExists
    && navigation.targetVisible
  )
  const excludedResults = results.filter(result => (
    excludedTargets.some(target => targetMatches(result.route, target))
  ))
  const excludedTargetsValid = excludedResults.length === 0
  const invalidResults = results.slice(0, topK).filter(result => routeValidity.get(normalizeRoute(result.route)) === false)
  return {
    id: definition.id,
    query: definition.query,
    category: definition.category,
    intent: definition.intent,
    passed: runtimeComplete && productFilterValid && resultCountValid && targetFound && descriptionValid
      && highlightContentValid && highlightStyleValid && highlightVisibleValid
      && titleHighlightValidation.contentValid && titleHighlightValidation.styleValid
      && titleHighlightValidation.visibleValid && descriptionHighlightValidation.contentValid
      && descriptionHighlightValidation.styleValid && descriptionHighlightValidation.visibleValid
      && sectionTitleValid && sectionPlacementValid && sectionHighlightValidation.contentValid
      && sectionHighlightValidation.styleValid && sectionHighlightValidation.visibleValid
      && sectionRouteValid && sectionNavigationValid && firstRelevantRankValid && firstRelevantRankLowEnough
      && excludedTargetsValid
      && invalidResults.length === 0,
    runtimeComplete,
    timedOut: Boolean(observed.timedOut),
    errors,
    expectedProductFilter,
    productFilter: observed.productFilter,
    productFilterValid,
    latencyMs: observed.latencyMs,
    viewportWidth: observed.viewportWidth,
    resultCount: results.length,
    resultCountValid,
    descriptionValid,
    expectedDescriptionIncludes,
    highlightContentValid,
    highlightStyleValid,
    highlightVisibleValid,
    expectedHighlightIncludes,
    highlights,
    titleHighlightContentValid: titleHighlightValidation.contentValid,
    titleHighlightStyleValid: titleHighlightValidation.styleValid,
    titleHighlightVisibleValid: titleHighlightValidation.visibleValid,
    expectedTitleHighlightIncludes,
    titleHighlights,
    descriptionHighlightContentValid: descriptionHighlightValidation.contentValid,
    descriptionHighlightStyleValid: descriptionHighlightValidation.styleValid,
    descriptionHighlightVisibleValid: descriptionHighlightValidation.visibleValid,
    expectedDescriptionHighlightIncludes,
    descriptionHighlights,
    sectionTitleValid,
    sectionPlacementValid,
    expectedSectionTitleIncludes,
    sectionHighlightContentValid: sectionHighlightValidation.contentValid,
    sectionHighlightStyleValid: sectionHighlightValidation.styleValid,
    sectionHighlightVisibleValid: sectionHighlightValidation.visibleValid,
    expectedSectionHighlightIncludes,
    sectionHighlights,
    expectedSectionHash,
    sectionRouteValid,
    sectionNavigationValid,
    excludedTargetsValid,
    excludedRoutes: excludedResults.map(result => result.route),
    maxFirstRelevantRank,
    firstRelevantRankValid,
    minFirstRelevantRank,
    firstRelevantRankLowEnough,
    navigation,
    firstRelevantRank: firstRelevantIndex >= 0 ? firstRelevantIndex + 1 : null,
    reciprocalRankAt10: firstRelevantIndex >= 0 && firstRelevantIndex < 10 ? 1 / (firstRelevantIndex + 1) : 0,
    successAt1: relevantCase ? firstRelevantIndex === 0 : null,
    successAt3: relevantCase ? firstRelevantIndex >= 0 && firstRelevantIndex < 3 : null,
    successAt10: relevantCase ? firstRelevantIndex >= 0 && firstRelevantIndex < 10 : null,
    ndcgAt10: relevantCase && idealDcg > 0 ? dcg(uniqueTargetGrades) / idealDcg : null,
    targetRecallAt10: relevantCase ? matchedTargetCount / targets.length : null,
    negativeCorrect: relevantCase ? null : results.length <= maxResults,
    invalidResultCountAt10: invalidResults.length,
    results: results.slice(0, topK).map((result, index) => ({
      rank: index + 1,
      route: result.route,
      title: result.title,
      ...(result.sectionTitle ? { sectionTitle: result.sectionTitle } : {}),
      ...(result.description ? { description: result.description } : {}),
      relevance: grades[index],
      routeExists: routeValidity.get(normalizeRoute(result.route)) ?? null,
    })),
  }
}

function summarizeCases(cases, linkCheck) {
  const relevant = cases.filter(item => item.successAt10 !== null)
  const negative = cases.filter(item => item.negativeCorrect !== null)
  const latencies = cases.map(item => item.latencyMs).filter(Number.isFinite)
  const resultCount = cases.reduce((sum, item) => sum + item.results.length, 0)
  const invalidCount = cases.reduce((sum, item) => sum + item.invalidResultCountAt10, 0)
  return {
    caseCount: cases.length,
    passedCaseCount: cases.filter(item => item.passed).length,
    completionRate: round(cases.filter(item => item.runtimeComplete).length / cases.length),
    successAt1: round(mean(relevant.map(item => Number(item.successAt1)))),
    successAt3: round(mean(relevant.map(item => Number(item.successAt3)))),
    successAt10: round(mean(relevant.map(item => Number(item.successAt10)))),
    mrrAt10: round(mean(relevant.map(item => item.reciprocalRankAt10))),
    ndcgAt10: round(mean(relevant.map(item => item.ndcgAt10))),
    targetRecallAt10: round(mean(relevant.map(item => item.targetRecallAt10))),
    zeroResultAccuracy: round(mean(negative.map(item => Number(item.negativeCorrect)))),
    invalidResultRateAt10: linkCheck ? round(resultCount ? invalidCount / resultCount : 0) : null,
    invalidResultCountAt10: linkCheck ? invalidCount : null,
    p50LatencyMs: round(percentile(latencies, 0.5), 1),
    p95LatencyMs: round(percentile(latencies, 0.95), 1),
    runtimeErrorCaseCount: cases.filter(item => item.errors.length > 0).length,
    timeoutCaseCount: cases.filter(item => item.timedOut).length,
  }
}

function categorySummaries(cases, linkCheck) {
  const categories = new Map()
  for (const item of cases) {
    if (!categories.has(item.category)) categories.set(item.category, [])
    categories.get(item.category).push(item)
  }
  return Object.fromEntries([...categories].map(([category, items]) => [category, summarizeCases(items, linkCheck)]))
}

function evaluateThresholds(summary, thresholds, linkCheck) {
  const evaluations = {}
  const minimums = ['completionRate', 'successAt1', 'successAt3', 'successAt10', 'mrrAt10', 'ndcgAt10', 'targetRecallAt10', 'zeroResultAccuracy']
  for (const metric of minimums) {
    if (thresholds[metric] === undefined || summary[metric] === null) continue
    evaluations[metric] = { actual: summary[metric], threshold: thresholds[metric], direction: 'minimum', passed: summary[metric] >= thresholds[metric] }
  }
  for (const metric of ['invalidResultRateAt10', 'p95LatencyMs']) {
    if (thresholds[metric] === undefined || summary[metric] === null) continue
    if (metric === 'invalidResultRateAt10' && !linkCheck) continue
    evaluations[metric] = { actual: summary[metric], threshold: thresholds[metric], direction: 'maximum', passed: summary[metric] <= thresholds[metric] }
  }
  return evaluations
}

function summarizeResources(resources, origin) {
  const unique = new Map()
  for (const resource of resources) unique.set(resource.name, resource)
  const values = [...unique.values()]
  const pagefind = values.filter(resource => {
    const pathname = resource.name.startsWith('http') ? new URL(resource.name).pathname : resource.name
    return pathname.startsWith('/pagefind/') || pathname.startsWith('/pagefind-titles/')
  })
  return {
    origin,
    resourceCount: values.length,
    totalTransferBytes: values.reduce((sum, resource) => sum + resource.transferSize, 0),
    totalEncodedBytes: values.reduce((sum, resource) => sum + resource.encodedBodySize, 0),
    totalDecodedBytes: values.reduce((sum, resource) => sum + resource.decodedBodySize, 0),
    pagefindResourceCount: pagefind.length,
    pagefindTransferBytes: pagefind.reduce((sum, resource) => sum + resource.transferSize, 0),
  }
}

async function mapLimit(values, limit, mapper) {
  const results = new Array(values.length)
  let nextIndex = 0
  async function worker() {
    while (nextIndex < values.length) {
      const index = nextIndex++
      results[index] = await mapper(values[index], index)
    }
  }
  await Promise.all(Array.from({ length: Math.min(limit, values.length) }, worker))
  return results
}

async function buildRouteValidity(observedCases, target, topK, enabled) {
  const routeMap = new Map()
  for (const observed of observedCases) {
    for (const result of (observed.results || []).slice(0, topK)) {
      routeMap.set(normalizeRoute(result.route), result.route)
    }
  }
  const routes = [...routeMap.keys()]
  const validity = new Map()
  if (!enabled) return validity
  if (target.distDir) {
    for (const route of routes) validity.set(route, Boolean(resolveStaticFile(target.distDir, route)))
    return validity
  }
  console.log(`Checking ${routes.length} unique result URLs...`)
  const states = await mapLimit(routes, 12, async (route) => {
    let originalUrl
    try {
      originalUrl = resolveSameOriginHttpUrl(routeMap.get(route), target.url)
    }
    catch {
      return false
    }
    const attempts = [originalUrl]
    if (!extname(originalUrl.pathname) && !originalUrl.pathname.endsWith('/')) {
      attempts.push(new URL(`${originalUrl.pathname}.html`, originalUrl))
      attempts.push(new URL(`${originalUrl.pathname}/index.html`, originalUrl))
    }
    for (const url of attempts) {
      try {
        let response = await fetchSameOrigin(url, target.url, {
          method: 'HEAD',
          signal: AbortSignal.timeout(10000),
        })
        if (!response.ok && ![404, 410].includes(response.status)) {
          response = await fetchSameOrigin(url, target.url, {
            headers: { Range: 'bytes=0-0' },
            signal: AbortSignal.timeout(10000),
          })
          await response.body?.cancel()
        }
        if (response.ok) return true
      }
      catch {}
    }
    return false
  })
  routes.forEach((route, index) => validity.set(route, states[index]))
  return validity
}

async function readEngine(targetUrl) {
  try {
    const response = await fetchSameOrigin(
      new URL('/pagefind/pagefind-entry.json', targetUrl),
      targetUrl,
      { signal: AbortSignal.timeout(10000) },
    )
    if (!response.ok) return null
    const entry = await response.json()
    const [language, languageData] = Object.entries(entry.languages || {})[0] || []
    return {
      name: 'Pagefind',
      version: entry.version,
      language: language || 'unknown',
      pageCount: languageData?.page_count ?? 0,
      indexHash: languageData?.hash || '',
    }
  }
  catch {
    return null
  }
}

function formatPercent(value) {
  return value === null || value === undefined ? '-' : `${(value * 100).toFixed(1)}%`
}

function failureReason(item) {
  if (item.timedOut) return '查询超时'
  if (item.errors.length) return item.errors[0].split('\n')[0]
  if (!item.productFilterValid) return `产品筛选未生效，实际为 ${item.productFilter || '未知'}`
  if (!item.excludedTargetsValid) return `结果包含当前产品不支持的页面：${item.excludedRoutes.join('、')}`
  if (item.invalidResultCountAt10) return `Top 10 包含 ${item.invalidResultCountAt10} 个无效链接`
  if (!item.resultCountValid) return `结果数量不符合预期，实际 ${item.resultCount}`
  if (item.firstRelevantRank === null) return '未返回主要目标'
  if (!item.firstRelevantRankValid) return `首个主要目标位于第 ${item.firstRelevantRank} 名，要求不超过第 ${item.maxFirstRelevantRank} 名`
  if (!item.firstRelevantRankLowEnough) return `首个主要目标位于第 ${item.firstRelevantRank} 名，要求不早于第 ${item.minFirstRelevantRank} 名`
  if (!item.descriptionValid) return `摘要未包含：${item.expectedDescriptionIncludes.join('、')}`
  if (!item.sectionTitleValid) return `小标题未包含：${item.expectedSectionTitleIncludes.join('、')}`
  if (!item.sectionPlacementValid) return '小标题未紧跟大标题或换行后未左对齐'
  if (!item.highlightContentValid) return `摘要未高亮：${item.expectedHighlightIncludes.join('、')}`
  if (!item.highlightStyleValid) return `摘要高亮样式无效：${item.expectedHighlightIncludes.join('、')}`
  if (!item.highlightVisibleValid) return `摘要高亮被裁剪：${item.expectedHighlightIncludes.join('、')}`
  if (!item.titleHighlightContentValid) return `标题未高亮：${item.expectedTitleHighlightIncludes.join('、')}`
  if (!item.titleHighlightStyleValid) return `标题高亮样式无效：${item.expectedTitleHighlightIncludes.join('、')}`
  if (!item.titleHighlightVisibleValid) return `标题高亮被裁剪：${item.expectedTitleHighlightIncludes.join('、')}`
  if (!item.descriptionHighlightContentValid) return `摘要未完整高亮：${item.expectedDescriptionHighlightIncludes.join('、')}`
  if (!item.descriptionHighlightStyleValid) return `摘要完整高亮样式无效：${item.expectedDescriptionHighlightIncludes.join('、')}`
  if (!item.descriptionHighlightVisibleValid) return `摘要完整高亮被裁剪：${item.expectedDescriptionHighlightIncludes.join('、')}`
  if (!item.sectionHighlightContentValid) return `小标题未高亮：${item.expectedSectionHighlightIncludes.join('、')}`
  if (!item.sectionHighlightStyleValid) return `小标题高亮样式无效：${item.expectedSectionHighlightIncludes.join('、')}`
  if (!item.sectionHighlightVisibleValid) return `小标题高亮被裁剪：${item.expectedSectionHighlightIncludes.join('、')}`
  if (!item.sectionRouteValid) return `结果未定位到小节：${item.expectedSectionHash}`
  if (!item.sectionNavigationValid) return `点击后未滚动到小节：${item.expectedSectionHash}`
  return `首个主要目标位于第 ${item.firstRelevantRank} 名`
}

function buildMarkdown(report) {
  const labels = {
    completionRate: '查询无异常完成率',
    successAt1: 'Success@1',
    successAt3: 'Success@3',
    successAt10: 'Success@10',
    mrrAt10: 'MRR@10',
    ndcgAt10: 'nDCG@10',
    targetRecallAt10: '目标召回率@10',
    zeroResultAccuracy: '无结果准确率',
    invalidResultRateAt10: '无效链接率@10',
    p95LatencyMs: 'P95 端到端延迟',
  }
  const lines = [
    '# Ascend C 搜索质量报告',
    '',
    `- 生成时间：${report.generatedAt}`,
    `- 目标模式：\`${report.target.mode}\``,
    `- 目标地址：${report.target.url}`,
    ...(report.target.resolvedCommit ? [`- Commit：\`${report.target.resolvedCommit}\``] : []),
    `- 浏览器：${report.chromiumVersion}`,
    `- 查询数：${report.summary.caseCount}`,
    '',
    '## 总体指标',
    '',
    '| 指标 | 实测 | 阈值 | 结果 |',
    '| --- | ---: | ---: | :---: |',
  ]
  for (const [metric, evaluation] of Object.entries(report.thresholds)) {
    const latency = metric.endsWith('LatencyMs')
    const actual = latency ? `${evaluation.actual} ms` : formatPercent(evaluation.actual)
    const threshold = latency
      ? `<= ${evaluation.threshold} ms`
      : `${evaluation.direction === 'minimum' ? '>=' : '<='} ${formatPercent(evaluation.threshold)}`
    lines.push(`| ${labels[metric] || metric} | ${actual} | ${threshold} | ${evaluation.passed ? '通过' : '未通过'} |`)
  }
  lines.push('', `通过用例：${report.summary.passedCaseCount}/${report.summary.caseCount}；运行时异常：${report.summary.runtimeErrorCaseCount}；超时：${report.summary.timeoutCaseCount}。`)
  lines.push('', '## 分类指标', '', '| 分类 | 用例 | 完成率 | Success@1 | Success@3 | Success@10 | MRR@10 | nDCG@10 | P95 |', '| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |')
  for (const [category, summary] of Object.entries(report.categories)) {
    lines.push(`| ${category} | ${summary.caseCount} | ${formatPercent(summary.completionRate)} | ${formatPercent(summary.successAt1)} | ${formatPercent(summary.successAt3)} | ${formatPercent(summary.successAt10)} | ${summary.mrrAt10 ?? '-'} | ${summary.ndcgAt10 ?? '-'} | ${summary.p95LatencyMs} ms |`)
  }
  const failures = report.cases.filter(item => !item.passed)
  lines.push('', '## 未通过用例', '')
  if (!failures.length) lines.push('无。')
  else {
    lines.push('| 查询 | 分类 | 原因 |', '| --- | --- | --- |')
    for (const item of failures) lines.push(`| \`${escapeCell(item.query)}\` | ${escapeCell(item.category)} | ${escapeCell(failureReason(item))} |`)
  }
  lines.push('', '## 用例明细', '', '| 结果 | 分类 | 查询 | 结果数 | 首个相关排名 | 延迟 | Top 1 |', '| :---: | --- | --- | ---: | ---: | ---: | --- |')
  for (const item of report.cases) {
    const top = item.results[0]
    lines.push(`| ${item.passed ? '通过' : '失败'} | ${escapeCell(item.category)} | \`${escapeCell(item.query)}\` | ${item.resultCount} | ${item.firstRelevantRank ?? '-'} | ${item.latencyMs?.toFixed(1) ?? '-'} ms | ${escapeCell(top ? `${top.title} (${top.route})` : '-')} |`)
  }
  return `${lines.join('\n')}\n`
}

async function main() {
  const options = parseArgs(process.argv.slice(2))
  if (options.noSandbox) {
    console.warn('WARNING: Chromium sandbox is disabled; only use --no-sandbox inside a trusted isolation boundary.')
  }
  if (!existsSync(options.cases)) throw new Error(`Cases file does not exist: ${options.cases}`)
  const dataset = JSON.parse(await readFile(options.cases, 'utf8'))
  validateDataset(dataset)
  if (options.caseIds.length) {
    const requested = new Set(options.caseIds)
    dataset.cases = dataset.cases.filter(item => requested.has(item.id))
    const missing = options.caseIds.filter(id => !dataset.cases.some(item => item.id === id))
    if (missing.length) throw new Error(`Unknown case id(s): ${missing.join(', ')}`)
  }

  const chromium = findChromium(options.chromium)
  const chromiumVersion = run(chromium, ['--version'])
  const target = await prepareTarget(options)
  let browser
  try {
    browser = await launchBrowser(chromium, options.timeoutMs, options.noSandbox)
    const browserErrors = []
    browser.client.on('Runtime.exceptionThrown', params => {
      browserErrors.push(params.exceptionDetails?.exception?.description || params.exceptionDetails?.text || 'Runtime exception')
    })
    console.log(`Running ${dataset.cases.length} search cases against ${target.url} with ${chromiumVersion}...`)
    const page = await initializeSearchPage(browser.client, target.url, options.timeoutMs)
    const observedCases = []
    let searchPageReady = true
    for (const [index, definition] of dataset.cases.entries()) {
      await browser.client.command('Emulation.setDeviceMetricsOverride', {
        width: definition.viewportWidth || DEFAULT_VIEWPORT_WIDTH,
        height: DEFAULT_VIEWPORT_HEIGHT,
        deviceScaleFactor: 1,
        mobile: false,
      })
      if (!searchPageReady) {
        await initializeSearchPage(browser.client, target.url, options.timeoutMs)
        searchPageReady = true
      }
      browserErrors.length = 0
      let observed
      try {
        observed = await runSearchCase(browser.client, definition)
      }
      catch (error) {
        observed = { id: definition.id, query: definition.query, latencyMs: null, timedOut: false, errors: [error.stack || String(error)], results: [] }
      }
      observed.errors.push(...browserErrors)
      if (definition.expect.sectionHash) {
        const resultRank = observed.results.findIndex(result => relevanceForRoute(
          result.route,
          definition.expect.targets
        ) >= (dataset.successGrade || 1))
        if (resultRank >= 0) {
          browserErrors.length = 0
          try {
            observed.navigation = await runSectionNavigation(browser.client, definition, resultRank)
          }
          catch (error) {
            observed.errors.push(error.stack || String(error))
          }
          observed.errors.push(...browserErrors)
          searchPageReady = false
        }
      }
      observedCases.push(observed)
      process.stdout.write(`\r${String(index + 1).padStart(String(dataset.cases.length).length)}/${dataset.cases.length} ${definition.query.slice(0, 48).padEnd(48)}`)
    }
    process.stdout.write('\n')

    const topK = dataset.topK || 10
    const routeValidity = await buildRouteValidity(observedCases, target, topK, options.linkCheck)
    const evaluatedCases = dataset.cases.map((definition, index) => evaluateCase(
      definition,
      observedCases[index],
      topK,
      dataset.successGrade || 1,
      routeValidity,
    ))
    const summary = summarizeCases(evaluatedCases, options.linkCheck)
    const thresholds = evaluateThresholds(summary, dataset.thresholds || {}, options.linkCheck)
    const origin = new URL(target.url).origin
    const report = {
      schemaVersion: 2,
      generatedAt: new Date().toISOString(),
      dataset: { version: dataset.version, name: dataset.name, path: relative(REPOSITORY_ROOT, options.cases) },
      target: { ...target.metadata, url: target.url },
      engine: await readEngine(target.url),
      chromiumVersion,
      userAgent: page.userAgent,
      runtime: { initMs: round(page.initMs, 1), linkCheck: options.linkCheck },
      summary,
      thresholds,
      thresholdPassed: Object.values(thresholds).every(item => item.passed),
      casePassed: summary.passedCaseCount === summary.caseCount,
      resources: summarizeResources(await readResources(browser.client, origin), origin),
      categories: categorySummaries(evaluatedCases, options.linkCheck),
      cases: evaluatedCases,
    }
    report.passed = report.thresholdPassed && report.casePassed
    await mkdir(dirname(options.output), { recursive: true })
    await writeFile(options.output, `${JSON.stringify(report, null, 2)}\n`, 'utf8')
    const markdownPath = markdownReportPath(options.output)
    await writeFile(markdownPath, buildMarkdown(report), 'utf8')
    console.log(`Completion: ${formatPercent(summary.completionRate)}`)
    console.log(`Cases passed: ${summary.passedCaseCount}/${summary.caseCount}`)
    console.log(`Success@1/3/10: ${formatPercent(summary.successAt1)} / ${formatPercent(summary.successAt3)} / ${formatPercent(summary.successAt10)}`)
    console.log(`MRR@10: ${summary.mrrAt10}; nDCG@10: ${summary.ndcgAt10}; target recall@10: ${formatPercent(summary.targetRecallAt10)}`)
    console.log(`Latency P50/P95: ${summary.p50LatencyMs} ms / ${summary.p95LatencyMs} ms`)
    console.log(`Reports: ${relative(process.cwd(), options.output)}, ${relative(process.cwd(), markdownPath)}`)
    if (options.enforce && !report.passed) process.exitCode = 2
  }
  finally {
    await browser?.dispose()
    await target.dispose()
  }
}

main().catch((error) => {
  console.error(error.stack || error.message || String(error))
  process.exitCode = 1
})
