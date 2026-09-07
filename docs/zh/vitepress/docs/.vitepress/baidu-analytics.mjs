/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

export const DEFAULT_BAIDU_ANALYTICS_SITE_ID = '0814059bc7b4d1effb73a5726d6763fb'
export const BAIDU_ANALYTICS_SITE_ID = DEFAULT_BAIDU_ANALYTICS_SITE_ID

function validateBaiduAnalyticsSiteId(siteId) {
  if (!/^[0-9a-f]{32}$/i.test(siteId)) {
    throw new Error('BAIDU_ANALYTICS_SITE_ID must be a 32-character hexadecimal value')
  }
  return siteId
}

export function resolveBaiduAnalyticsSiteId(environment = {}) {
  const configuredSiteId = String(environment.BAIDU_ANALYTICS_SITE_ID || '').trim()
  return configuredSiteId
    ? validateBaiduAnalyticsSiteId(configuredSiteId)
    : DEFAULT_BAIDU_ANALYTICS_SITE_ID
}

export function createBaiduAnalyticsHead(siteId = DEFAULT_BAIDU_ANALYTICS_SITE_ID) {
  const validatedSiteId = validateBaiduAnalyticsSiteId(String(siteId).trim())
  return [
    [
      'script',
      {},
      `
      var _hmt = _hmt || [];
      (function () {
        var hm = document.createElement("script");
        hm.src = "https://hm.baidu.com/hm.js?${validatedSiteId}";
        var s = document.getElementsByTagName("script")[0];
        s.parentNode.insertBefore(hm, s);
      })();
    `,
    ],
  ]
}

export const baiduAnalyticsHead = createBaiduAnalyticsHead()

export function getBaiduPageviewPath(location) {
  if (!location) return '/'
  return `${location.pathname || '/'}${location.search || ''}`
}

export function installBaiduAnalytics(
  router,
  browser = typeof window === 'undefined' ? undefined : window,
) {
  if (!router || !browser) return false

  let currentPath = getBaiduPageviewPath(browser.location)
  const previousAfterRouteChange = router.onAfterRouteChange

  router.onAfterRouteChange = async (...args) => {
    if (previousAfterRouteChange) {
      await previousAfterRouteChange.apply(router, args)
    }

    const nextPath = getBaiduPageviewPath(browser.location)
    if (nextPath === currentPath) return

    currentPath = nextPath
    browser._hmt = browser._hmt || []
    browser._hmt.push(['_trackPageview', nextPath])
  }

  return true
}
