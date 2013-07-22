/* include/asm/mach-msm/htc_pwrsink.h
 *
 * Copyright (C) 2008 HTC Corporation.
 * Copyright (C) 2007 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/clk.h>
#include <../../../drivers/staging/android/timed_output.h>
#include <linux/sched.h>

#include <mach/msm_rpcrouter.h>

//#include <mach/clk.h>
#include <mach/samsung_vibe.h>
#include <mach/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>


struct clk *android_vib_clk; /* core_clk */

#define CORE_CLK_M_DEFAULT			21
#define CORE_CLK_N_DEFAULT			18000
#define CORE_CLK_D_DEFAULT			9000	/* 50% duty cycle */ 
#define IMM_PWM_MULTIPLIER		    17778	/* Must be integer */

/*
 * ** Global variables for LRA PWM M,N and D values.
 * */
VibeInt32 g_nLRA_CORE_CLK_M = CORE_CLK_M_DEFAULT;
VibeInt32 g_nLRA_CORE_CLK_N = CORE_CLK_N_DEFAULT;
VibeInt32 g_nLRA_CORE_CLK_D = CORE_CLK_N_DEFAULT;
VibeInt32 g_nLRA_CORE_CLK_PWM_MUL = IMM_PWM_MULTIPLIER;

static struct hrtimer vibe_timer;
static int is_vibe_on = 0;


static int msm_vibrator_suspend(struct platform_device *pdev, pm_message_t state);
static int msm_vibrator_resume(struct platform_device *pdev);
static int msm_vibrator_probe(struct platform_device *pdev);
static int msm_vibrator_exit(struct platform_device *pdev);
static int msm_vibrator_power(int power_mode);


/* Variable for setting PWM in Force Out Set */
VibeInt32 g_nForce_32 = 0;

/*
 * This function is used to set and re-set the CORE_CLK M and N counters
 * to output the desired target frequency.
 * 
 */

/* for the suspend/resume VIBRATOR Module */
static struct platform_driver msm_vibrator_platdriver = 
{
	.probe   = msm_vibrator_probe,
	.suspend = msm_vibrator_suspend,
	.resume  = msm_vibrator_resume,
	.remove  = __devexit_p(msm_vibrator_exit),
	.driver = 
	{
			.name = MODULE_NAME,
			.owner = THIS_MODULE,
	},
};

static int msm_vibrator_suspend(struct platform_device *pdev, pm_message_t state)
{

#if 0
		if(is_vibe_on) {
			clk_disable(android_vib_clk);
			is_vibe_on = 0;
		}
#endif
	msm_vibrator_power(VIBRATION_OFF);
	printk("[VIB] susepend\n");
	return VIBE_S_SUCCESS;
}

static int msm_vibrator_resume(struct platform_device *pdev)
{

//	msm_vibrator_power(VIBRATION_ON);

	printk("[VIB] resume\n");
	return VIBE_S_SUCCESS;
}

static int __devexit msm_vibrator_exit(struct platform_device *pdev)
{
		printk("[VIB] EXIT\n");
		return 0;
}

static int msm_vibrator_power(int on)
{
	struct regulator *regulator_msm_vibrator;
	int ret;

	regulator_msm_vibrator = regulator_get(NULL, "vreg_vib");

	if(IS_ERR(regulator_msm_vibrator)) {
			printk(KERN_ERR "%s: regulator get failed (%ld)\n",
							__func__,PTR_ERR(regulator_msm_vibrator));
			return PTR_ERR(regulator_msm_vibrator);
	}

	if (on) {
		ret = regulator_set_voltage(regulator_msm_vibrator, 3000000, 3000000);
		if (ret) {
				printk(KERN_ERR "%s: regulator set level failed (%d)\n",
								__func__,ret);
				return -EIO;
		}

		ret = regulator_enable(regulator_msm_vibrator);
		if (ret) {
			printk(KERN_ERR "%s: regulator enable failed (%d)\n",
							__func__, ret);
			return -EIO;
		}
//		printk("[VIB] ON\n");
		mdelay(15);
	} else {
		ret = regulator_disable(regulator_msm_vibrator);
		if (ret) {
			printk(KERN_ERR "%s: regulator disable failed (%d)\n",
								__func__, ret);
			return -EIO;
		}
//		printk("[VIB] OFF\n");
	}

	return VIBE_S_SUCCESS;
}

#if 0
static int vibe_set_pwm_freq(int nForce)
{
#if 1
		/* Put the MND counter in reset mode for programming */
		HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_EN_BMSK, 0);
		HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_PRE_DIV_SEL_BMSK, 0 << HWIO_GP_NS_REG_PRE_DIV_SEL_SHFT); /* P: 0 => Freq/1, 1 => Freq/2, 4 => Freq/4 */
		HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_SRC_SEL_BMSK, 0 << HWIO_GP_NS_REG_SRC_SEL_SHFT); /* S : 0 => TXCO(19.2MHz), 1 => Sleep XTAL(32kHz) */
		HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_MODE_BMSK, 2 << HWIO_GP_NS_REG_MNCNTR_MODE_SHFT); /* Dual-edge mode */
		HWIO_OUTM(GP_MD_REG, HWIO_GP_MD_REG_M_VAL_BMSK, g_nLRA_CORE_CLK_M << HWIO_GP_MD_REG_M_VAL_SHFT);
		g_nForce_32 = ((nForce * g_nLRA_CORE_CLK_PWM_MUL) >> 8) + g_nLRA_CORE_CLK_D;
//		printk("%s, g_nForce_32 : %d\n",__FUNCTION__,g_nForce_32);
		HWIO_OUTM(GP_MD_REG, HWIO_GP_MD_REG_D_VAL_BMSK, ( ~((VibeInt16)g_nForce_32 << 1) ) << HWIO_GP_MD_REG_D_VAL_SHFT);
		HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_N_VAL_BMSK, ~(g_nLRA_CORE_CLK_N - g_nLRA_CORE_CLK_M) << HWIO_GP_NS_REG_GP_N_VAL_SHFT);
		HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_EN_BMSK, 1 << HWIO_GP_NS_REG_MNCNTR_EN_SHFT);                    /* Enable M/N counter */
		printk("%x, %x, %x\n",( ~((VibeInt16)g_nForce_32 << 1) ) << HWIO_GP_MD_REG_D_VAL_SHFT,~(g_nLRA_CORE_CLK_N - g_nLRA_CORE_CLK_M) << HWIO_GP_NS_REG_GP_N_VAL_SHFT,1 << HWIO_GP_NS_REG_MNCNTR_EN_SHFT);
#else
		clk_set_rate(android_vib_clk,32583);
#endif	
		return VIBE_S_SUCCESS;
}


static void set_pmic_vibrator(int on)
{
//	printk("[VIB] %s, input : %s\n",__func__,on ? "ON":"OFF");
	if (on) {
		clk_enable(android_vib_clk);
		gpio_direction_output(VIB_ON, VIBRATION_ON);
		is_vibe_on = 1;
	} else {
		if(is_vibe_on) {
			gpio_direction_output(VIB_ON, VIBRATION_OFF);
			clk_disable(android_vib_clk);
			is_vibe_on = 0;
		}
	}

}
#endif

#if 0
static void pmic_vibrator_on(struct work_struct *work)
{
	set_pmic_vibrator(VIBRATION_ON);
}

static void pmic_vibrator_off(struct work_struct *work)
{
	set_pmic_vibrator(VIBRATION_OFF);
}

static void timed_vibrator_on(struct timed_output_dev *sdev)
{
	schedule_work(&work_vibrator_on);
}

static void timed_vibrator_off(struct timed_output_dev *sdev)
{
	schedule_work(&work_vibrator_off);
}
//#else
static void pmic_vibrator_on(void)
{
		set_pmic_vibrator(VIBRATION_ON);
	msm_vibrator_power(VIBRATION_ON);
}

static void pmic_vibrator_off(void)
{
		msm_vibrator_power(VIBRATION_OFF);
		set_pmic_vibrator(VIBRATION_OFF);
}

#endif

static void vibrator_enable(struct timed_output_dev *dev, int value)
{
	unsigned long flags;

	hrtimer_cancel(&vibe_timer);

	if (value == 0) {
		printk("[VIB] OFF\n");
		msm_vibrator_power(VIBRATION_OFF);
		//pmic_vibrator_off();
	}
	else {

		if(value < 0)
			value = ~value;
		printk("[VIB] ON, %d ms\n",value);

		value = (value > 15000 ? 15000 : value);

		msm_vibrator_power(VIBRATION_ON);

		hrtimer_start(&vibe_timer,
			      ktime_set(value / 1000, (value % 1000) * 1000000),
			      HRTIMER_MODE_REL);
	}
}

static int vibrator_get_time(struct timed_output_dev *dev)
{
	if (hrtimer_active(&vibe_timer)) {
		ktime_t r = hrtimer_get_remaining(&vibe_timer);
		struct timeval t = ktime_to_timeval(r);
		return t.tv_sec * 1000 + t.tv_usec / 1000;
	}
	return 0;
}

static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
{
#if 0
		timed_vibrator_off(NULL);
		return HRTIMER_NORESTART;
#else
		unsigned int remain;

//		printk("[VIB] %s\n",__func__);
		if(hrtimer_active(&vibe_timer)) {
				ktime_t r = hrtimer_get_remaining(&vibe_timer);
				struct timeval t = ktime_to_timeval(r);
				remain=t.tv_sec * 1000000 + t.tv_usec;
				remain = remain / 1000;
				if(t.tv_sec < 0) {
						remain = 0;
				}
//				printk("[VIB] hrtimer active, remain:%d\n",remain);
				if(!remain) 
//					pmic_vibrator_off();
					msm_vibrator_power(VIBRATION_OFF);
		} else {
//				printk("[VIB] hrtimer not active\n");
//			pmic_vibrator_off();
				msm_vibrator_power(VIBRATION_OFF);
		}
		return HRTIMER_NORESTART;
#endif
}

static struct timed_output_dev pmic_vibrator = {
	.name = "vibrator",
	.get_time = vibrator_get_time,
	.enable = vibrator_enable,
};

static int __devinit msm_vibrator_probe(struct platform_device *pdev)
{
	hrtimer_init(&vibe_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	vibe_timer.function = vibrator_timer_func;

	timed_output_dev_register(&pmic_vibrator);

#if 0
	msm_vibrator_power(VIBRATION_ON);
	
		/* Vibrator init sequence 
		 * 1. power on ( regulator get )
		 * 2. clock get & enable ( core_clk )
		 * 3. VIB_EN on
		 */
	
		android_vib_clk = clk_get(NULL,"core_clk");
	
		if(IS_ERR(android_vib_clk)) {
			printk("android vib clk failed!!!\n");
		} else {
			printk("THNAK YOU!!\n");
		}
		vibe_set_pwm_freq(216);
#endif
	return 0;
}

static int __init msm_init_pmic_vibrator(void)
{
	int nRet;

	nRet = platform_driver_register(&msm_vibrator_platdriver);

	printk("[VIB] platform driver register result : %d\n",nRet);
	if (nRet)
	{ 
		printk("[VIB] platform_driver_register failed\n");
	}

	return nRet;

}

static void __exit msm_exit_pmic_vibrator(void)
{
	platform_driver_unregister(&msm_vibrator_platdriver);

}

module_init(msm_init_pmic_vibrator);
module_exit(msm_exit_pmic_vibrator);


MODULE_DESCRIPTION("timed output pmic vibrator device");
MODULE_LICENSE("GPL");
