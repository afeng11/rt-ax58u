/*
 * MBSS (Multi BSS) related declarations and exported functions for
 * Broadcom 802.11 Networking Device Driver
 *
 * Copyright 2020 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wlc_mbss.h 772065 2019-02-13 19:49:39Z $
 */

#ifndef _WLC_MBSS_H_
#define _WLC_MBSS_H_

#include "wlc_cfg.h"

#include <typedefs.h>
#include <ethernet.h>

#include "wlc_types.h"

/* max ap bss supported by driver */
#define WLC_MAX_AP_BSS(d11corerev) \
	(D11REV_IS(d11corerev, 22) \
	 ? 8 : \
	 (D11REV_IS(d11corerev, 24) || D11REV_IS(d11corerev, 25)) ? 4 : WLC_MAX_UCODE_BSS)

/* calculate the number of template mem blks needed for beacons
 * and probe responses of all the BSSs. add one additional block
 * to account for 104 bytes of space reserved (?) at the start of
 * template memory.
 */
#define TPLBLKS_PER_BCN(r)	(D11REV_GE(r, 40) ?		\
				 TPLBLKS_AC_PER_BCN_NUM : TPLBLKS_PER_BCN_NUM)
#define TPLBLKS_PER_PRS(r)	(D11REV_GE(r, 40) ?		\
				 TPLBLKS_AC_PER_PRS_NUM : TPLBLKS_PER_PRS_NUM)

#define	MBSS_TPLBLKS(r, n)		(1 + ((n) * (TPLBLKS_PER_BCN(r) + TPLBLKS_PER_PRS(r))))
#define	MBSS_TXFIFO_START_BLK(r, n)	MBSS_TPLBLKS(r, n)

/*
 * Under MBSS, a pre-TBTT interrupt is generated.  The driver puts beacons in
 * the ATIM fifo at that time and tells uCode about pending BC/MC packets.
 * The delay is settable thru uCode.  MBSS_PRE_TBTT_DEFAULT_us is the default
 * setting for this value.
 * If the driver experiences significant latency, it must avoid setting up
 * beacons or changing the SHM FID registers.  The "max latency" setting
 * indicates the maximum permissible time between the TBTT interrupt and the
 * DPC to respond to the interrupt before the driver must abort the TBTT
 * beacon operations.
 */
#define MBSS_PRE_TBTT_DEFAULT_us 5000		/* 5 milliseconds! */
#define MBSS_PRE_TBTT_MAX_LATENCY_us 4000	/* XXX: still keeping this parameter to
						 * support MBSS 4
						 */
#define MBSS_PRE_TBTT_MIN_THRESH_us 1000	/* 1 msec threshold before actual TBTT */

/* external function prototypes */
extern wlc_mbss_info_t *wlc_mbss_attach(wlc_info_t *wlc);
extern void wlc_mbss_detach(wlc_mbss_info_t *mbss);

extern int wlc_mbss_bsscfg_up(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss_bsscfg_down(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss_update_beacon(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss_update_probe_resp(wlc_info_t *wlc, wlc_bsscfg_t *cfg, bool suspend);
extern bool wlc_prq_process(wlc_info_t *wlc, bool bounded);
extern void wlc_mbss_dotxstatus(wlc_info_t *wlc, tx_status_t *txs, void *pkt, uint16 fc,
                                wlc_pkttag_t *pkttag, uint supr_status);
extern void wlc_mbss_dotxstatus_mcmx(wlc_info_t *wlc, wlc_bsscfg_t *cfg, tx_status_t *txs);
extern void wlc_mbss_shm_ssid_upd(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint16 *base);
extern void wlc_mbss_txq_update_bcmc_counters(wlc_info_t *wlc, wlc_bsscfg_t *cfg, void *p);
extern void wlc_mbss_increment_ps_trans_cnt(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss16_upd_closednet(wlc_info_t *wlc, wlc_bsscfg_t *cfg);

#ifdef BCMDBG
extern void wlc_mbss_dump_spt_pkt_state(wlc_info_t *wlc, wlc_bsscfg_t *cfg, int i);
#else
#define wlc_mbss_dump_spt_pkt_state(wlc, cfg, i)
#endif /* BCMDBG || BCMDBG_ERR */

extern wlc_pkt_t wlc_mbss_get_probe_template(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern wlc_pkt_t wlc_mbss_get_bcn_template(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern wlc_spt_t *wlc_mbss_get_bcn_spt(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss_set_bcn_tim_ie(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint8 *ie);
extern uint32 wlc_mbss_get_bcmc_pkts_sent(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss_bcmc_reset(wlc_info_t *wlc, wlc_bsscfg_t *cfg);

extern int wlc_mbss_validate_mac(wlc_info_t *wlc, wlc_bsscfg_t *cfg, struct ether_addr *addr);
extern void wlc_mbss_reset_mac(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss_set_mac(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_mbss_record_time(wlc_info_t *wlc);
extern void wlc_mbss_reset_prq(wlc_info_t *wlc);
extern int wlc_mbss16_tbtt(wlc_info_t *wlc, uint32 macintstatus);
extern int wlc_mbss_bss_idx(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_macgen(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
#ifdef TESTBED_AP_11AX
#define WLC_MBSS_MAXBSSID_EXP		3				/* 2^3 = 8 APs */
#define WLC_MBSS_MAXBSSID		(1 << WLC_MBSS_MAXBSSID_EXP)
extern bool wlc_mbss_mbssid_active(wlc_info_t *wlc);
#endif /* TESTBED_AP_11AX */

#endif /* _WLC_MBSS_H_ */
