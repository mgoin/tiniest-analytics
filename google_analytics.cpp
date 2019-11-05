/*
Tiniest Analytics - v1.1 - MIT License (i.e. can use it for whatever purpose)

Version history:
v1.1 - 2017/12/15 - changed to C-style C++

Original authors:
Mihai Dragomir, email:dmc@pixelshard.com
Mihai Gosa, email:pintea@inthekillhouse.com  twitter: @gosamihai

*/

#include "google_analytics.h"
#include "curl/curl.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <dbg.h>

static CURLM *g_pMultiHandle = NULL;
static char g_strServicePath[2048] = {'\0'}; // caches clientID and trackingID after calling init() (http://www.google-analytics.com/collect?v=1&tid=%s&cid=%s)

// utility function, used to replace spaces with pluses for URLs
static void replace_str_char(char *s, const int len, const char what, const char with)
{
	for (int i = 0; i < len; ++i)
	{
		if (s[i] == what)
			s[i] = with;
	}
}

// utility function, used to send the HTTP get
static bool execute_curl_url(const char *url, ...)
{
	dbg(g_pMultiHandle);
	if (!g_pMultiHandle)
	{
		return false;
	}

	va_list argptr;
	va_start(argptr, url);
	char strAddr[2048] = {'\0'};
	int slen = vsprintf(strAddr, url, argptr);
	va_end(argptr);

	replace_str_char(strAddr, slen, ' ', '+');
	dbg(strAddr);

	CURL *pCurlHandle = curl_easy_init();
	curl_easy_setopt(pCurlHandle, CURLOPT_URL, strAddr);
	curl_easy_setopt(pCurlHandle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(pCurlHandle, CURLOPT_TIMEOUT, 20);
	CURLMcode result = curl_multi_add_handle(g_pMultiHandle, pCurlHandle);
	return (result == CURLM_OK);
}

bool google_analytics_init(const char *trackingId, const char *uniqueClientId)
{
	curl_global_init(CURL_GLOBAL_ALL);
	g_pMultiHandle = curl_multi_init();
	dbg(g_pMultiHandle);
	if (!g_pMultiHandle)
	{
		return false;
	}

	sprintf(g_strServicePath, "https://www.google-analytics.com/collect?v=1&tid=%s&cid=%s", trackingId, uniqueClientId);
	dbg(g_strServicePath);
	return true;
}

void google_analytics_shutdown()
{
	dbg(g_pMultiHandle);
	if (!g_pMultiHandle)
	{
		return;
	}

	google_analytics_update(); // one last update to remove handles from stack if they're ready
	curl_multi_cleanup(g_pMultiHandle);
	g_pMultiHandle = NULL;
}

void google_analytics_event(const char *category, const char *action, const char *label, unsigned int value)
{
	execute_curl_url("%s&t=event&ec=%s&ea=%s&el=%s&ev=%u&z=%d", g_strServicePath, category, action, label, value, rand());
}

void google_analytics_event(const char *category, const char *action, const char *label)
{
	execute_curl_url("%s&t=event&ec=%s&ea=%s&el=%s&z=%d", g_strServicePath, category, action, label, rand());
}

void google_analytics_event(const char *category, const char *action)
{
	execute_curl_url("%s&t=event&ec=%s&ea=%s&z=%d", g_strServicePath, category, action, rand());
}

void google_analytics_update()
{
	if (!g_pMultiHandle)
	{
		return;
	}

	int stillRunning = 0;
	curl_multi_perform(g_pMultiHandle, &stillRunning);

	CURLMsg *pMsg = NULL;
	do
	{
		int msgsInQueue = 0;
		pMsg = curl_multi_info_read(g_pMultiHandle, &msgsInQueue);
		if (pMsg && (pMsg->msg == CURLMSG_DONE))
		{
			dbg(pMsg);
			long response_code;
			curl_easy_getinfo(pMsg->easy_handle, CURLINFO_RESPONSE_CODE, &response_code);
			dbg(response_code);
			if (response_code != 200)
			{
				const char *urlp;
				curl_easy_getinfo(pMsg->easy_handle, CURLINFO_EFFECTIVE_URL, &urlp);

				char strerr[2048];
				sprintf(strerr, "[Error] google_analytics_Update() failed for URL '%s' with error %ld\n", urlp ? urlp : "?", response_code);
				assert(response_code == 200 && strerr);
			}
			curl_multi_remove_handle(g_pMultiHandle, pMsg->easy_handle);
			curl_easy_cleanup(pMsg->easy_handle);
		}
	} while (pMsg);
}
