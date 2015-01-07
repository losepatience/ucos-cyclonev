#include <fpga_pwm.h>
#include <fpga_pwm.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/regs.h>
#include <asm/io.h>
#include <platform.h>

static void __pwm_clear_buffer(uint32_t ch, dbuf_id_t bufId);
/*-----------buffer half full interrupt register---------------*/
static void __pwm_irq_almost_enable(uint32_t ch, dbuf_id_t bufId, uint8_t isena);
static void __pwm_irq_almost_clr(uint32_t ch, dbuf_id_t bufId);
static void __pwm_irq_almost_mask(uint32_t ch, dbuf_id_t bufId, uint8_t ismast);
/*------------buffer empty interrupt register----------*/
static void __pwm_irq_empty_enable(uint32_t ch, dbuf_id_t bufId, uint8_t isena);
static void __pwm_irq_empty_clr(uint32_t ch, dbuf_id_t bufId);
static void __pwm_irq_empty_mask(uint32_t ch, dbuf_id_t bufId, uint8_t ismask);
static void __pwm_switch_buf(uint32_t ch, uint32_t bufids);
static inline void __pwm_handler_empty(void * chId);
static inline void __pwm_handler_almost(void * chId);
static void __pwm_irq_handler(void *arg);

pwm_t *const __pwm = (pwm_t *)PWM_BASE_ADDR;
channel_t *const __channels = (channel_t *)PWM_CHANNEL_ADDR;

//pwm func
static pwm_handler_t pwm_handler;		//pwm 中断处理函数
pwm_handler_t pwm_handler_e;
pwm_handler_t pwm_handler_ae;

pwm_t *pwm_get_pwm()
{
	return __pwm;
}

channel_t *pwm_get_channels()
{
	return __channels;
}

inline void pwm_mode_pwm(uint32_t ch, uint8_t ispwm)
{
	pwm_t *pwm = __pwm;

	if(ispwm == 0)
		clrbits32(&pwm->pwm_mr, ch);
	else
		setbits32(&pwm->pwm_mr, ch);
}

inline void pwm_control_source_hps(uint32_t ch, uint8_t isHps)
{
	pwm_t *pwm = __pwm;

	if(isHps)
		setbits32(&pwm->pwm_csr, ch);
	else
		clrbits32(&pwm->pwm_csr, ch);
}

inline void pwm_polar_pos(uint32_t ch, uint8_t ispos)
{
	pwm_t *pwm = __pwm;

	if(ispos == 0)
		setbits32(&pwm->pwm_pr, ch);
	else
		clrbits32(&pwm->pwm_pr, ch);
}

inline uint8_t pwm_ch_polar_isneg(uint32_t chId)
{
	return (__pwm->pwm_pr & (0x01 << chId)) ? PWM_PR_NEG : !PWM_PR_NEG;
}

inline void pwm_override(uint32_t ch, uint8_t isover)
{
	pwm_t *pwm = __pwm;

	if(isover == 0)
		clrbits32(&pwm->pwm_ovr, ch);
	else
		setbits32(&pwm->pwm_ovr, ch);
}

inline void pwm_enable(uint32_t ch, uint8_t isena)
{
	pwm_t *pwm = __pwm;

	if(isena == 0)
		clrbits32(&pwm->pwm_enr, ch);
	else
		setbits32(&pwm->pwm_enr, ch);
}

inline void pwm_switch_fill_b0(uint32_t ch, uint8_t isbuf0)		//**
{
	pwm_t *pwm = __pwm;

	if(isbuf0)
		clrbits32(&pwm->pwm_sfbr, ch);
	else
		setbits32(&pwm->pwm_sfbr, ch);
}

inline void pwm_switch_exe_b0(uint32_t ch, uint8_t isbuf0)		//**
{
	pwm_t *pwm = __pwm;

	if(isbuf0)
		clrbits32(&pwm->pwm_sebr, ch);
	else
		setbits32(&pwm->pwm_sebr, ch);
}

static inline void __pwm_clear_buffer(uint32_t ch, dbuf_id_t bufId)
{
	pwm_t *pwm = __pwm;

	setbits32(&pwm->pwm_cbr[bufId], ch);
}

static inline uint8_t __pwm_buffer_isfull(uint32_t ch, dbuf_id_t bufId, uint8_t isfull)
{
	pwm_t *pwm = __pwm;

	return ((pwm->pwm_sfr[bufId] & ch) ? 1 : 0);
}

//static inline uint8_t __pwm_buffer_isempty(uint32_t ch, dbuf_id_t bufId, uint8_t isfull){}

/*-----------buffer almost empty interrupt register---------------*/
static inline void __pwm_irq_almost_enable(uint32_t ch, dbuf_id_t bufId, uint8_t isena)
{
	pwm_t *pwm = __pwm;

	if(isena == 0)
		clrbits32(&pwm->pwm_iaeer[bufId], ch);
	else
		setbits32(&pwm->pwm_iaeer[bufId], ch);
}

static inline void __pwm_irq_almost_clr(uint32_t ch, dbuf_id_t bufId)
{
	pwm_t *pwm = __pwm;

	setbits32(&pwm->pwm_iaecr[bufId], ch);
}

static inline void __pwm_irq_almost_mask(uint32_t ch, dbuf_id_t bufId, uint8_t ismask)
{
	pwm_t *pwm = __pwm;

	if(ismask == 0)	//不屏蔽
		clrbits32(&pwm->pwm_iaemr[bufId], ch);
	else
		setbits32(&pwm->pwm_iaemr[bufId], ch);

}

/*------------buffer empty interrupt register----------*/
static inline void __pwm_irq_empty_enable(uint32_t ch, dbuf_id_t bufId, uint8_t isena)
{
	pwm_t *pwm = __pwm;

	if(isena == 0)
		clrbits32(&pwm->pwm_ieer[bufId], ch);
	else
		setbits32(&pwm->pwm_ieer[bufId], ch);
}

static inline void __pwm_irq_empty_clr(uint32_t ch, dbuf_id_t bufId)
{
	pwm_t *pwm = __pwm;

	setbits32(&pwm->pwm_iecr[bufId], ch);
}

static inline void __pwm_irq_empty_mask(uint32_t ch, dbuf_id_t bufId, uint8_t ismask)
{
	pwm_t *pwm = __pwm;

	if(ismask == 0)	//不屏蔽
		clrbits32(&pwm->pwm_iemr[bufId], ch);
	else
		setbits32(&pwm->pwm_iemr[bufId], ch);

}

/*对于FPGA来说，周期和脉宽的设定是有顺序要求的，先设周期、再设脉宽
 * 以防外部调用设置时顺序颠倒，不再对外提供单独设置周期或脉宽的函数
 */
inline uint32_t pwm_ch_set_pwm(uint32_t chId, uint32_t period, uint32_t duty)
{
	channel_t *channels = __channels;

#if 0
	uint32_t tmp;

	if(period < duty)
	{
		tmp = period;
		period = duty;
		duty = tmp;
	}
#endif
	writel(period, &channels[chId].pwmc_cprdr);
	writel(duty, &channels[chId].pwmc_cdtyr);
	return 0;
}

/*
 * ch		:要switch buffer的channel,32位，每一位对应一个channel,若对应位为1，则说明该要switch
 * bufids	:对应每一个channel的bufid,32位，每一位对应一个channel，若对应位为1，则说明该channel使用的bufid为DBUF_1
 */
static void __pwm_switch_buf(uint32_t ch, uint32_t bufids)
{
	pwm_t *pwm = __pwm;

	//首先转换 execute buffer和fill in buffer
	setbits32(&pwm->pwm_sebr, ch & bufids);
	clrbits32(&pwm->pwm_sebr, (~bufids) & ch);
	setbits32(&pwm->pwm_sfbr, ch & bufids);
	clrbits32(&pwm->pwm_sfbr, (~bufids) & ch);

	//默认不清空无效缓冲的buffer

	//切换相应的中断屏蔽寄存器
	clrbits32(&pwm->pwm_iaemr[DBUF_1], bufids & ch);			//将空
	setbits32(&pwm->pwm_iaemr[DBUF_1], (~bufids) & ch);
	setbits32(&pwm->pwm_iaemr[DBUF_0], bufids & ch);			//将空
	clrbits32(&pwm->pwm_iaemr[DBUF_0], (~bufids) & ch);
	clrbits32(&pwm->pwm_iemr[DBUF_1], bufids & ch);			//空
	setbits32(&pwm->pwm_iemr[DBUF_1], (~bufids) & ch);
	setbits32(&pwm->pwm_iemr[DBUF_0], bufids & ch);			//空
	clrbits32(&pwm->pwm_iemr[DBUF_0], (~bufids) & ch);

#if 1	//其实只要保证两个buffer的使能都是打开的，不切换也没关系，因为中断屏蔽已切换
	//切换相应的中断使能寄存器
	setbits32(&pwm->pwm_iaeer[DBUF_1], bufids & ch);			//将空
	clrbits32(&pwm->pwm_iaeer[DBUF_1], (~bufids) & ch);
	clrbits32(&pwm->pwm_iaeer[DBUF_0], bufids & ch);			//将空
	setbits32(&pwm->pwm_iaeer[DBUF_0], (~bufids) & ch);

	setbits32(&pwm->pwm_ieer[DBUF_1], bufids & ch);			//空
	clrbits32(&pwm->pwm_ieer[DBUF_1], (~bufids) & ch);
	clrbits32(&pwm->pwm_ieer[DBUF_0], bufids & ch);			//空
	setbits32(&pwm->pwm_ieer[DBUF_0], (~bufids) & ch);
#endif
}

/*NOTE：该函数会把缓冲中未执行的PWM数据清空，直接执行这里填充的PWM数据
 * 由于是紧急执行的pwm数据，所以填充的数据尽量不要太多（控制在10以内），
 * 由于受FPGA端buffer大小限制，所以填充数据不要太多，一定要小于buffer大小（FPAG buffer:256）
 * 若实际需要执行的pwm数大小大于10，可先填充5-10个，剩余数据可在执行期再次填充，
 * 但需要注意：若是继续填充必须直接调用函数pwm_ch_set_pwm填充，不可再次用pwm_ch_urgency_pwm
 * (经测试：新写入的数据执行是有延时时间的，大概每多写20个左右的PWM数据就会延时10um
 * 因此，该函数内写入的PWM数据建议保持在5-10个，其余数据可在稍后写入)
 */
uint32_t pwm_ch_urgency_pwm(uint32_t chId, uint32_t period[], uint32_t duty[], uint32_t num)
{
	pwm_t *pwm = __pwm;
	uint32_t i = 0;
	dbuf_id_t old_buf = DBUF_0;
	dbuf_id_t new_buf = DBUF_0;

#if 1
	old_buf = (pwm->pwm_sebr & (0x01 << chId)) ? DBUF_1 : DBUF_0;
	new_buf = (old_buf == DBUF_1) ? DBUF_0: DBUF_1;
	pwm_switch_fill_b0(0x01 << chId, new_buf == DBUF_0);		//新填充pwm数据使用另外一个buf

	for(i = 0; i < num; i ++)
	{
		//这里没有判断缓冲是否填满，所以填充时要注意填充大小
		pwm_ch_set_pwm(chId, period[i], duty[i]);
	}

	__pwm_switch_buf(0x01 << chId, new_buf << chId);			//转换执行新缓冲的PWM
	__pwm_clear_buffer(0x01 << chId, old_buf);					//清除以前的缓冲中的PWM
#endif
	if(num == 0)		//如果num为0，（对于外部）相当于只是清缓冲
		return 0;

	return i;			//返回压入缓冲的PWM个数
}

/*向某一个pwm channel里写想要紧急执行的pwm数据（要想使多个channel里的紧急数据执行，需要多次调用该函数，
* 且每次只能写一个channel），写完后需要立刻调用函数pwm_start_urgency_pwm才可使新写入的pwm数据执行
* 关于填充大小限制参数函数pwm_ch_urgency_pwm的注释
*/
uint32_t pwm_ch_set_urgency_pwm(uint32_t chId, uint32_t period[], uint32_t duty[], uint32_t num)
{
	pwm_t *pwm = __pwm;
	uint32_t i = 0;
	dbuf_id_t old_buf = DBUF_0;
	dbuf_id_t new_buf = DBUF_0;

	old_buf = (pwm->pwm_sebr & (0x01 << chId)) ? DBUF_1 : DBUF_0;
	new_buf = (old_buf == DBUF_1) ? DBUF_0: DBUF_1;

	pwm_switch_fill_b0(0x01 << chId, new_buf == DBUF_0);		//新填充pwm数据使用另外一个buffer
	for(i = 0; i < num; i ++)
	{
		pwm_ch_set_pwm(chId, period[i], duty[i]);
	}
	pwm_switch_fill_b0(0x01 << chId, old_buf == DBUF_0);		//填充完后切换回原来的fill buffer

	return i;
}

void pwm_start_urgency_pwm(uint32_t ch)
{
	uint32_t newBufId = 0;
	uint32_t oldBufId = 0;
	int i = 0;
	pwm_t *pwm = __pwm;

	for(i = 0; i < PWM_MAX_CH_CNT; i++)
	{
		if(!((pwm->pwm_sebr & ch) & (0x01 << i)))
			newBufId |= 0x01 << i;
	}
	__pwm_switch_buf(ch, newBufId);			//执行新的buffer的紧急pwm数据

	//清空旧的buffer里的pwm数据
	for(i = 0; i < PWM_MAX_CH_CNT; i++)
	{
		if(!(ch & (0x01 << i)))
			continue;
		oldBufId = (newBufId & (0x01 << i)) ? DBUF_0 : DBUF_1;
		__pwm_clear_buffer(ch & (0x01 << i), oldBufId);				//清除以前的缓冲中的PWM
	}
}

#if 1
/*对外接口，一般不需要使用这些函数，正常情况下不编译，(未经测试，不一定对，慎用,(-_-))
 * 这里选择性的开放了几个用到的可能性比较大的函数，其它函数暂且不编译
 * pwm_clear_buffer:清空当前执行缓冲的数据
 * pwm_buffer_isfull：判断当前写入的缓冲是否已满
 */
inline void pwm_clear_buffer(uint32_t ch)		//当前执行缓冲
{
	pwm_t *pwm = __pwm;
	int i = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;

		setbits32(&pwm->pwm_cbr[bufId], ch);
	}
}

inline uint8_t pwm_buffer_isfull(uint32_t ch)	//当前填充缓冲，同时只能判断一个通道
{
	pwm_t *pwm = __pwm;
	dbuf_id_t bufId = (pwm->pwm_sfbr & ch) ? DBUF_1 : DBUF_0;

	return ((pwm->pwm_sfr[bufId] & ch) ? 1 : 0);
}

inline uint8_t pwm_buffer_isempty(uint32_t ch)	//当前执行缓冲，同时只能判断一个通道
{
	pwm_t *pwm = __pwm;
	dbuf_id_t bufId = (pwm->pwm_sebr & ch) ? DBUF_1 : DBUF_0;

	return ((pwm->pwm_ser[bufId] & ch) ? 1 : 0);
}

inline void pwm_irq_almost_clr(uint32_t ch)		//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;

		setbits32(&pwm->pwm_iaecr[bufId], ch);
	}
}

inline void pwm_irq_almost_enable(uint32_t ch, uint8_t isena)	//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;

		if(isena == 0)
			clrbits32(&pwm->pwm_iaeer[bufId], ch);
		else
			setbits32(&pwm->pwm_iaeer[bufId], ch);
	}
}

inline void pwm_irq_almost_mask(uint32_t ch, uint8_t ismask)	//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;

		if(ismask == 0)	//不屏蔽
			clrbits32(&pwm->pwm_iaemr[bufId], ch);
		else
			setbits32(&pwm->pwm_iaemr[bufId], ch);
	}

}

inline uint32_t pwm_irq_almost_status(uint32_t ch)				//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;
	uint32_t irq_status = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;
		irq_status |= pwm->pwm_iaesr[bufId] & (0x01 << i);
	}

	return irq_status;
}
/*------------buffer empty interrupt register----------*/
inline void pwm_irq_empty_enable(uint32_t ch, uint8_t isena)		//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;

		if(isena == 0)
			clrbits32(&pwm->pwm_ieer[bufId], ch);
		else
			setbits32(&pwm->pwm_ieer[bufId], ch);
	}
}

inline void pwm_irq_empty_clr(uint32_t ch)			//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;

		setbits32(&pwm->pwm_iecr[bufId], ch);
	}
}

inline void pwm_irq_empty_mask(uint32_t ch, uint8_t ismask)			//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;

		if(ismask == 0)	//不屏蔽
			clrbits32(&pwm->pwm_iemr[bufId], ch);
		else
			setbits32(&pwm->pwm_iemr[bufId], ch);
	}
}

inline uint32_t pwm_irq_empty_status(uint32_t ch)				//当前执行
{
	pwm_t *pwm = __pwm;
	int i = 0;
	uint32_t irq_status = 0;

	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		dbuf_id_t bufId = ((pwm->pwm_sebr & ch) & (0x01 << i)) ? DBUF_1 : DBUF_0;
		irq_status |= pwm->pwm_iesr[bufId] & (0x01 << i);
	}

	return irq_status;
}

/*该函数慎用
 * 1:一般情况下外部不需要手动切换buffer
 * 2:切换buffer一般根据需要清除上一个buffer里未执行完的pwm数据，该函数内没有清除
 * 因此在调用后根据需要清除*/
dbuf_id_t pwm_switch_buf(uint32_t ch)
{
	pwm_t *pwm = __pwm;

	uint32_t cur_bufid = pwm->pwm_sebr;
	uint32_t new_bufid = (~(ch & cur_bufid)) | ((~ch) & cur_bufid);
	//首先转换 execute buffer和fill in buffer
	setbits32(&pwm->pwm_sebr, ch & new_bufid);
	clrbits32(&pwm->pwm_sebr, (~new_bufid) & ch);
	setbits32(&pwm->pwm_sfbr, ch & new_bufid);
	clrbits32(&pwm->pwm_sfbr, (~new_bufid) & ch);

	//默认不清空无效缓冲的buffer

	//切换相应的中断屏蔽寄存器
	clrbits32(&pwm->pwm_iaemr[DBUF_1], new_bufid & ch);			//将空
	setbits32(&pwm->pwm_iaemr[DBUF_1], (~new_bufid) & ch);
	setbits32(&pwm->pwm_iaemr[DBUF_0], new_bufid & ch);			//将空
	clrbits32(&pwm->pwm_iaemr[DBUF_0], (~new_bufid) & ch);
	clrbits32(&pwm->pwm_iemr[DBUF_1], new_bufid & ch);			//空
	setbits32(&pwm->pwm_iemr[DBUF_1], (~new_bufid) & ch);
	setbits32(&pwm->pwm_iemr[DBUF_0], new_bufid & ch);			//空
	clrbits32(&pwm->pwm_iemr[DBUF_0], (~new_bufid) & ch);

#if 1	//其实只要保证两个buffer的全能都是打开的，不切换也没关系，因为中断屏蔽已切换
	//切换相应的中断使能寄存器
	setbits32(&pwm->pwm_iaeer[DBUF_1], new_bufid & ch);			//将空
	clrbits32(&pwm->pwm_iaeer[DBUF_1], (~new_bufid) & ch);
	clrbits32(&pwm->pwm_iaeer[DBUF_0], new_bufid & ch);			//将空
	setbits32(&pwm->pwm_iaeer[DBUF_0], (~new_bufid) & ch);

	setbits32(&pwm->pwm_ieer[DBUF_1], new_bufid & ch);			//空
	clrbits32(&pwm->pwm_ieer[DBUF_1], (~new_bufid) & ch);
	clrbits32(&pwm->pwm_ieer[DBUF_0], new_bufid & ch);			//空
	setbits32(&pwm->pwm_ieer[DBUF_0], (~new_bufid) & ch);
#endif

	return new_bufid;
}
#endif


static inline void __pwm_handler_empty(void * arg)
{

}

static inline void __pwm_handler_almost(void * arg)
{
	uint32_t chId = (uint32_t)arg;
	static uint32_t cnt = 0;
	int chIndex = 0;

	if(!(chId & PWM_MSK(0)))
		return;

#if 0
	if(cnt == 100)
	{
		static uint8_t pwm_num = 18;
#if 0
		//方式一
		pwm_ch_urgency_pwm(PWM_IDX(0), period, duty, pwm_num);
		pwm_ch_urgency_pwm(PWM_IDX(1), period, duty, pwm_num);
#else
		//方式二
		pwm_ch_set_urgency_pwm(PWM_IDX(0), period, duty, pwm_num);
		pwm_ch_set_urgency_pwm(PWM_IDX(1), period, duty, pwm_num);
		pwm_start_urgency_pwm(PWM_MSK(0)|PWM_MSK(1));
#endif
	}
	else
#endif

#if 1
	static uint8_t level = 0;
	static uint32_t periodNum = 100;
	static uint32_t period1 = 5000;
	static uint32_t period2 = 3000;
	static uint32_t period3 = 3000;
	static uint32_t duty1 = 1500;
	static uint32_t duty2 = 0;
	static uint32_t duty3 = 1500;
	static uint32_t period_s = 5000;
	static uint32_t period_e = 1000;
	static uint32_t duty_acc = 500;
	static uint8_t acc_time = 1;	//秒
	static uint32_t per_tatal_num = 0;

	static uint32_t cur_per = 3000;
	uint32_t per = acc_time * 1000 * 10 / ((period_s * 10 / 1000) * (period_s * 10 / 1000 - 1) / 2
			- (period_e * 10 / 1000) * (period_e * 10 / 1000 - 1) / 2);


	if(per_tatal_num++ < 10 * 1000 * 60)	//加
	{

		if(cur_per >= period_e)
		{
			if(cnt++ < per)
			{
				pwm_ch_set_pwm(chIndex, cur_per, duty_acc);
			}
			else if(cnt >= per)
			{
				pwm_ch_set_pwm(chIndex, cur_per, duty_acc);
				cur_per--;
				cnt = 0;
			}
		}
		else
		{
			pwm_ch_set_pwm(chIndex, cur_per, duty_acc);
		}
	}
	else if(per_tatal_num++ < 10 * 1000 * 110)	// 匀
	{
		pwm_ch_set_pwm(chIndex, cur_per, duty_acc);
	}
	else if(per_tatal_num++ < 10 * 1000 * 180)	//减
	{
		if(cur_per < period_s)
		{
			if(cnt++ < per)
			{
				pwm_ch_set_pwm(chIndex, cur_per, duty_acc);
			}
			else if(cnt >= per)
			{
				pwm_ch_set_pwm(chIndex, cur_per, duty_acc);
				cur_per++;
				cnt = 0;
			}
		}
		else
		{
			pwm_ch_set_pwm(chIndex, cur_per, 0);
		}
	}
	else
	{
		per_tatal_num = 0;
	}
#endif
}

static void __pwm_irq_handler(void *arg)
{
	int i = 0;
	volatile uint32_t chId = 0;
	unsigned long flags = 0;


	// check the empty irq
	local_irq_save(flags);
	for(i = 0; i < PWM_MAX_CH_CNT; i++)
	{
		dbuf_id_t valid_buf = (__pwm->pwm_sebr & (0x01 << i)) ? DBUF_1 : DBUF_0;
		if(__pwm->pwm_iesr[valid_buf] == 0)
			break;
		chId |= __pwm->pwm_iesr[valid_buf] & (0x01 << i);
	}
	if(chId != 0 && pwm_handler_e != NULL)
		pwm_handler_e((void *)chId);		//call the interrupt handler
	local_irq_restore(flags);

	if(chId != 0)
	{
		__pwm_irq_empty_clr(PWM_MSK_ALL,DBUF_0);		//清中断
		__pwm_irq_empty_clr(PWM_MSK_ALL,DBUF_1);		//清中断
	}

	//check the almost empty irq
	chId = 0;
	volatile dbuf_id_t valid_buf = 0;
	local_irq_save(flags);
	for(i = 0; i < PWM_MAX_CH_CNT; i ++)
	{
		valid_buf = (__pwm->pwm_sebr & (0x01 << i)) ? DBUF_1 : DBUF_0;
		if(__pwm->pwm_iaesr[valid_buf] == 0)
			break;
		chId |= (__pwm->pwm_iaesr[valid_buf] & (0x01 << i));
	}
	if(chId != 0 && pwm_handler_ae != NULL)
		pwm_handler_ae((void *)chId);		//call the interrupt handler

	local_irq_restore(flags);

	if(chId != 0)
	{
		__pwm_irq_almost_clr(PWM_MSK_ALL,DBUF_0);		//清中断
		__pwm_irq_almost_clr(PWM_MSK_ALL,DBUF_1);		//清中断
	}

}

int pwm_reset(uint32_t ch)
{
	//disable pwm function first
	pwm_enable(ch, 0);

	pwm_mode_pwm(ch, 1);							// pwm mode
	pwm_control_source_hps(ch, 0);					//control by fpga
	pwm_polar_pos(ch, 1);							//positive logic
	pwm_override(ch, 0);							//非HPS越权直接控制
	__pwm_clear_buffer(ch, DBUF_0);					//clear buffer
	__pwm_clear_buffer(ch, DBUF_1);					//clear buffer
	pwm_switch_fill_b0(ch, 1);						//默认填充 buffer为buf0
	pwm_switch_exe_b0(ch, 1);						//默认执行buffer为buf0

	// disable all almost empty interrutp
	__pwm_irq_almost_enable(ch, DBUF_0, 0);
	__pwm_irq_almost_mask(ch, DBUF_0, 1);
	__pwm_irq_almost_enable(ch, DBUF_1, 0);
	__pwm_irq_almost_mask(ch, DBUF_1, 1);

	//disable all empty interrutp
	__pwm_irq_empty_enable(ch, DBUF_0, 0);
	__pwm_irq_empty_mask(ch, DBUF_0, 1);
	__pwm_irq_empty_enable(ch, DBUF_1, 0);
	__pwm_irq_empty_mask(ch, DBUF_1, 1);

	pwm_switch_fill_b0(ch, 1);							//设置填充buffer为buffer0

	/*设置中断处理函数，这个三中断函数可自定义，但一般情况下只需要定义两个
	 *即空中断处理函数和将空中断处理函数，即空中断处理函数和将空中断处理函数，
	 *因为中断处理函数会判断中断状态进入相关的中断处理函数（空或者将空）
	 */
	pwm_handler = __pwm_irq_handler;
	pwm_handler_e = __pwm_handler_empty;
	pwm_handler_ae = __pwm_handler_almost;

	return 0;
}

int pwm_init(void)
{
	pwm_reset(PWM_MSK_ALL);

	request_irq(CONFIG_PWM_IRQ, pwm_handler, __pwm);		//set interrupt handle function

	return 0;
}

#if 1// old function

#endif
