/*
 * WPA Supplicant - Scanning
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "eloop.h"
#include "config.h"
#include "wpa_supplicant_i.h"
#include "mlme.h"
#include "uuid.h"


static void wpa_supplicant_gen_assoc_event(struct wpa_supplicant *wpa_s)
{
	struct wpa_ssid *ssid;
	union wpa_event_data data;

	ssid = wpa_supplicant_get_ssid(wpa_s);
	if (ssid == NULL)
		return;

	if (wpa_s->current_ssid == NULL)
		wpa_s->current_ssid = ssid;
	wpa_supplicant_initiate_eapol(wpa_s);
	wpa_printf(MSG_DEBUG, "Already associated with a configured network - "
		   "generating associated event");
	os_memset(&data, 0, sizeof(data));
	wpa_supplicant_event(wpa_s, EVENT_ASSOC, &data);
}

int wpa_supplicant_may_scan(struct wpa_supplicant *wpa_s)
{
	struct os_time time;

	if (wpa_s->conf->scan_cache > 0) {
		os_get_time(&time);
		time.sec -= wpa_s->conf->scan_cache;
		if (os_time_before(&time, &wpa_s->last_scan_results))
			return 0;
	}
	return 1;
}

static void wpa_supplicant_scan(void *eloop_ctx, void *timeout_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;
	struct wpa_ssid *ssid;
	int enabled, scan_req = 0, ret;
	const u8 *extra_ie = NULL;
	size_t extra_ie_len = 0;
	int scan_ssid_all = 1;

	if (wpa_s->disconnected && !wpa_s->scan_req)
		return;

	enabled = 0;

	/* check if all configured ssids should be scanned directly */
	ssid = wpa_s->conf->ssid;
	while (ssid) {
		if (!ssid->scan_ssid) {
			scan_ssid_all = 0;
			break;
		}
		ssid = ssid->next;
	}

	ssid = wpa_s->conf->ssid;
	while (ssid) {
		if (!ssid->disabled) {
			enabled++;
			break;
		}
		ssid = ssid->next;
	}
	if (!enabled && !wpa_s->scan_req) {
		wpa_printf(MSG_DEBUG, "No enabled networks - do not scan");
		wpa_supplicant_set_state(wpa_s, WPA_INACTIVE);
		return;
	}
	scan_req = wpa_s->scan_req;
	wpa_s->scan_req = 0;

	if (wpa_s->conf->ap_scan != 0 &&
	    wpa_s->driver && os_strcmp(wpa_s->driver->name, "wired") == 0) {
		wpa_printf(MSG_DEBUG, "Using wired driver - overriding "
			   "ap_scan configuration");
		wpa_s->conf->ap_scan = 0;
	}

	if (wpa_s->conf->ap_scan == 0) {
		wpa_supplicant_gen_assoc_event(wpa_s);
		return;
	}

	if (wpa_s->wpa_state == WPA_DISCONNECTED ||
	    wpa_s->wpa_state == WPA_INACTIVE)
		wpa_supplicant_set_state(wpa_s, WPA_SCANNING);

	ssid = wpa_s->conf->ssid;
	if (wpa_s->prev_scan_ssid != BROADCAST_SSID_SCAN) {
		while (ssid) {
			if (ssid == wpa_s->prev_scan_ssid) {
				ssid = ssid->next;
				break;
			}
			ssid = ssid->next;
		}
	}
	while (ssid) {
		if (!ssid->disabled &&
		    (ssid->scan_ssid || wpa_s->conf->ap_scan == 2))
			break;
		ssid = ssid->next;
	}

	if (scan_req != 2 && wpa_s->conf->ap_scan == 2) {
		/*
		 * ap_scan=2 mode - try to associate with each SSID instead of
		 * scanning for each scan_ssid=1 network.
		 */
		if (ssid == NULL) {
			wpa_printf(MSG_DEBUG, "wpa_supplicant_scan: Reached "
				   "end of scan list - go back to beginning");
			wpa_s->prev_scan_ssid = BROADCAST_SSID_SCAN;
			wpa_supplicant_req_scan(wpa_s, 0, 0);
			return;
		}
		if (ssid->next) {
			/* Continue from the next SSID on the next attempt. */
			wpa_s->prev_scan_ssid = ssid;
		} else {
			/* Start from the beginning of the SSID list. */
			wpa_s->prev_scan_ssid = BROADCAST_SSID_SCAN;
		}
		wpa_supplicant_associate(wpa_s, NULL, ssid);
		return;
	}

	if (scan_ssid_all && !ssid) {
		ssid = wpa_s->conf->ssid;
	}

	wpa_printf(MSG_DEBUG, "Starting AP scan (%s SSID)",
		   ssid ? "specific": "broadcast");
	if (ssid) {
		wpa_hexdump_ascii(MSG_DEBUG, "Scan SSID",
				  ssid->ssid, ssid->ssid_len);
		wpa_s->prev_scan_ssid = ssid;
	} else
		wpa_s->prev_scan_ssid = BROADCAST_SSID_SCAN;

	if (!wpa_supplicant_may_scan(wpa_s) ||
		(wpa_s->scan_res_tried == 0 && wpa_s->conf->ap_scan == 1 &&
	    !wpa_s->use_client_mlme)) {
		wpa_s->scan_res_tried++;
		wpa_printf(MSG_DEBUG, "Trying to get current scan results "
			   "first without requesting a new scan to speed up "
			   "initial association");
		wpa_supplicant_event(wpa_s, EVENT_SCAN_RESULTS, NULL);
		return;
	}

	wpa_drv_flush_pmkid(wpa_s);
	if (wpa_s->use_client_mlme) {
		ieee80211_sta_set_probe_req_ie(wpa_s, extra_ie, extra_ie_len);
		ret = ieee80211_sta_req_scan(wpa_s, ssid ? ssid->ssid : NULL,
					     ssid ? ssid->ssid_len : 0);
	} else {
		wpa_drv_set_probe_req_ie(wpa_s, extra_ie, extra_ie_len);
		ret = wpa_drv_scan(wpa_s, ssid ? ssid->ssid : NULL,
				   ssid ? ssid->ssid_len : 0);
	}

	if (ret) {
		wpa_printf(MSG_WARNING, "Failed to initiate AP scan.");
		wpa_supplicant_req_scan(wpa_s, 3, 0);
	}
}


/**
 * wpa_supplicant_req_scan - Schedule a scan for neighboring access points
 * @wpa_s: Pointer to wpa_supplicant data
 * @sec: Number of seconds after which to scan
 * @usec: Number of microseconds after which to scan
 *
 * This function is used to schedule a scan for neighboring access points after
 * the specified time.
 */
void wpa_supplicant_req_scan(struct wpa_supplicant *wpa_s, int sec, int usec)
{
	/* If there's at least one network that should be specifically scanned
	 * then don't cancel the scan and reschedule.  Some drivers do
	 * background scanning which generates frequent scan results, and that
	 * causes the specific SSID scan to get continually pushed back and
	 * never happen, which causes hidden APs to never get probe-scanned.
	 */
	if (eloop_is_timeout_registered(wpa_supplicant_scan, wpa_s, NULL) &&
	    wpa_s->conf->ap_scan == 1) {
		struct wpa_ssid *ssid = wpa_s->conf->ssid;

		while (ssid) {
			if (!ssid->disabled && ssid->scan_ssid)
				break;
			ssid = ssid->next;
		}
		if (ssid) {
			wpa_msg(wpa_s, MSG_DEBUG, "Not rescheduling scan to "
			        "ensure that specific SSID scans occur");
			return;
		}
	}

	wpa_msg(wpa_s, MSG_DEBUG, "Setting scan request: %d sec %d usec",
		sec, usec);
	eloop_cancel_timeout(wpa_supplicant_scan, wpa_s, NULL);
	eloop_register_timeout(sec, usec, wpa_supplicant_scan, wpa_s, NULL);
}


/**
 * wpa_supplicant_cancel_scan - Cancel a scheduled scan request
 * @wpa_s: Pointer to wpa_supplicant data
 *
 * This function is used to cancel a scan request scheduled with
 * wpa_supplicant_req_scan().
 */
void wpa_supplicant_cancel_scan(struct wpa_supplicant *wpa_s)
{
	wpa_msg(wpa_s, MSG_DEBUG, "Cancelling scan request");
	eloop_cancel_timeout(wpa_supplicant_scan, wpa_s, NULL);
}
