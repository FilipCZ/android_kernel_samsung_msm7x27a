/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU General Public License, version 2, in which case the provisions
 * of the GPL version 2 are required INSTEAD OF the BSD license.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
 * WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 */

#ifndef _M_ADC_PROC_H
#define _M_ADC_PROC_H

#include <linux/msm_adc.h>
int32_t tdkntcgtherm(int32_t adc_code, const struct adc_properties *,
		const struct chan_properties *, struct adc_chan_result *);
int32_t scale_default(int32_t adc_code, const struct adc_properties *,
		const struct chan_properties *, struct adc_chan_result *);
int32_t scale_msm_therm(int32_t adc_code, const struct adc_properties *,
		const struct chan_properties *, struct adc_chan_result *);
int32_t scale_batt_therm(int32_t adc_code, const struct adc_properties *,
		const struct chan_properties *, struct adc_chan_result *);
int32_t scale_pmic_therm(int32_t adc_code, const struct adc_properties *,
		const struct chan_properties *, struct adc_chan_result *);
int32_t scale_xtern_chgr_cur(int32_t adc_code, const struct adc_properties *,
		const struct chan_properties *, struct adc_chan_result *);
#endif /* _M_ADC_PROC_H */
