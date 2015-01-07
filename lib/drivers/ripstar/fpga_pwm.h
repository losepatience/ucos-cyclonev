/*
 *  PWM0		: out std_logic;	-- FPGA_3A_GPIO10     --xPul     --AJ2
 *	PWM1		: out std_logic;	-- FPGA_3A_GPIO16     --xDir     --AG2
 *	PWM2		: out std_logic;	-- FPGA_3A_GPIO12     --yPul     --AJ1
 *	PWM3		: out std_logic;	-- FPGA_3A_GPIO14     --yDir     --AH3
 *	PWM4		: out std_logic;	-- FPGA_3A_GPIO0      --zPul     --AF9
 *	PWM5		: out std_logic;	-- FPGA_3A_GPIO9      --zDir     --AH5
 *	PWM6		: out std_logic;	-- FPGA_3A_GPIO11     --xxPul     --AG5
 *	PWM7		: out std_logic;	-- FPGA_3A_GPIO3      --xxDir     --AF8
 *	PWM8		: out std_logic;	-- FPGA_5A_GPIO10     --yyPul     --Y23
 *	PWM9		: out std_logic;	-- FPGA_5A_GPIO1      --yyDir     --Y21
 *	PWM10		: out std_logic;	-- FPGA_5A_GPIO0      --zzPul     --W20
 *	PWM11		: out std_logic;	-- FPGA_3A_GPIO20     --zzDir     --AH28
 *
 */

#if 1
#define REG_PRINT_STATUS 	 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x00))		//R
#define REG_FPGA_CONFIG		 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x04))		//RW
#define REG_FPGA_CMD 		 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x08))		//RW
#define REG_MOTOR_COOR_X 	 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x0c))		//R
#define REG_MOTOR_COOR_Y 	 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x10))		//R
#define REG_PRT_COOR_X 		 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x14))		//R
#define REG_COOR_SYS_CTRL	 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x18))		//RW
#define REG_COOR_SYS_STAUS	 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x1c))		//R
#define REG_FIRE_START		 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x20))		//RW
#define REG_FIRE_END		 ((volatile unsigned int*)(FPGA_FIFO_ADDR + 0x24))		//RW
#endif

#ifndef __FPGA_PWM_H__
#define __FPGA_PWM_H__
#include <asm/io.h>

#define PWM_MAX_CH_CNT 14
#define CONFIG_PWM_IRQ		78

#define PWM_BASE_ADDR (SOCFPGA_LWH2F_ADDRESS + 0x2000)
#define PWM_CHANNEL_ADDR (PWM_BASE_ADDR + 0x80)

#define PWM_IDX(n) (n)					//pwm 通道索引
#define PWM_MSK(n) (0x01 << (n))		//pwm 通道位掩码
#define PWM_MSK_ALL (0xFFFFFFFF)

#define PWM_PR_NEG 1
typedef struct pwm_channel_t
{
	volatile uint32_t pwmc_cprdr; 	// Channel Period Register
	volatile uint32_t pwmc_cdtyr; 	// Channel Duty Cycle Register

	volatile uint32_t reserved[1];
}channel_t,*channelp_t;

typedef enum IRQ_TYPE
{
	IRQ_EMPTY = 1,
	IRQ_HALF,
}irq_type_t;

typedef enum DOUBLE_BUFFER_ID
{
	DBUF_0 = 0x00,
	DBUF_1 = 0x01,
}dbuf_id_t;

typedef struct pwm_t
{
	/*1 pwm_ovr:超控那个直接就是arm控制：
	 * 		当为pwm_mr(模式选择)为PWM模式且控制源（PWM源）为HPS时，直接在pwm_ovr里写010101010
	 * 		当为gpio模式时，所有寄存器中只有pwm_ovr起作用，此时在pwm_ovr里写0/1即输出0/1
	 * 2 pwm_csr:control source register专用在输出PWM时,这个时候输出是收到极性寄存器的影响的
	 * 3 将空中断:当buffer里少于某一数值时，会一直发中断，空中断当buffer每次由非空变为空时发送一次中断
	 * 4 pwm的实现为双缓冲机制，但在一般情况下，对外双缓冲是透明的，外部而言一般情况下不需要用户切换缓冲
	 * 	缓冲的切换在内部自动切换，因此关于缓冲的一般函数外部尽量不要调用
	 * 5 pwm_mr:当设置为0时，为gpio。
	 */
	volatile uint32_t pwm_mr;		// PWM Mode Register 0:gpio	1:pwm,当作为gpio时，用作pwm电机的方向控制
	volatile uint32_t pwm_csr;		// PWM Control Source Register 0:FPGA	1:HPS
	volatile uint32_t pwm_pr;		// PWM Polarity Register: 0正逻辑，1：负逻辑
	volatile uint32_t pwm_ovr;		// PWM Override Register 1:HPS 越权控制.当配置为gpio模式时，该位控制GPIO电平
	volatile uint32_t pwm_enr; 		// PWM Enable Register
	volatile uint32_t pwm_sfbr;		// PWM Switch Fill Buffer Register 0:buffer0, 1:buffer1
	volatile uint32_t pwm_sebr;		// PWM Switch Execute Buffer Register 0:buffer0, 1:buffer1
	volatile uint32_t pwm_cbr[2];	// PWM Clear Buffer Register
	volatile uint32_t pwm_sfr[2];	// PWM Status Full Register
	volatile uint32_t pwm_ser[2];	// PWM Status Empty Register
	//将空中断寄存器，当pwm buffer里少于一定程度（目前(FPGA)规定为10个）时会一直有中断，当大于时不再有中断
	volatile uint32_t pwm_iaesr[2];	// PWM Almost Empty Interrupt Status Register
	volatile uint32_t pwm_iaeer[2];	// PWM Almost Empty Interrupt Enable Register
	volatile uint32_t pwm_iaemr[2];	// PWM Almost Empty Interrupt Mask Register
	volatile uint32_t pwm_iaecr[2];	// PWM Almost Empty Interrupt Clear Register
	//空中断寄存器，当pwm buffer 里为空时发送一个中断
	volatile uint32_t pwm_iesr[2]; 	// PWM Empty Interrupt Status Register
	volatile uint32_t pwm_ieer[2]; 	// PWM Empty Interrupt Enable Register
	volatile uint32_t pwm_iemr[2]; 	// PWM Empty Interrupt Mask Register
	volatile uint32_t pwm_iecr[2]; 	// PWM Empty Interrupt Clear Register

	//channel_t *channels;
	volatile uint32_t reserved[4];
}pwm_t,*pwmp_t;

pwm_t *pwm_get_pwm(void);
channel_t *pwm_get_channels(void);
void pwm_mode_pwm(uint32_t ch, uint8_t ispwm);
void pwm_control_source_hps(uint32_t ch, uint8_t isHps);
void pwm_polar_pos(uint32_t ch, uint8_t ispos);
void pwm_override(uint32_t ch, uint8_t isover);
void pwm_enable(uint32_t ch, uint8_t isena);
inline void pwm_clear_buffer(uint32_t ch);
inline uint8_t pwm_buffer_isfull(uint32_t ch);
inline uint8_t pwm_buffer_isempty(uint32_t ch);
uint32_t pwm_ch_urgency_pwm(uint32_t chId, uint32_t period[], uint32_t duty[], uint32_t num);
uint32_t pwm_ch_set_urgency_pwm(uint32_t chId, uint32_t period[], uint32_t duty[], uint32_t num);
void pwm_start_urgency_pwm(uint32_t ch);
/*-----------buffer half full interrupt register---------------*/
void pwm_irq_almost_enable(uint32_t ch, uint8_t isena);
void pwm_irq_almost_clr(uint32_t ch);
void pwm_irq_almost_mask(uint32_t ch, uint8_t ismast);
/*------------buffer empty interrupt register----------*/
void pwm_irq_empty_enable(uint32_t ch, uint8_t isena);
void pwm_irq_empty_clr(uint32_t ch);
void pwm_irq_empty_mask(uint32_t ch, uint8_t ismask);
unsigned pwm_ch_set_pwm(uint32_t chId, uint32_t period, uint32_t duty);
int pwm_reset(uint32_t ch);
extern int pwm_init(void);

typedef void (*pwm_handler_t)(void *);
extern pwm_handler_t pwm_handler_e;			//空中断处理函数
extern pwm_handler_t pwm_handler_ae;		//将空中断处理函数


/*	 can delete
	-- FPGA_5A_GPIO8   : inout std_logic;	 --FPGA_GPIO_Set0(0)    --AB25
	-- FPGA_5B_GPIO11  : inout std_logic;    --FPGA_GPIO_Set0(1)    --AA28
	-- FPGA_5B_GPIO9   : inout std_logic;    --FPGA_GPIO_Set0(2)    --AB28
	-- FPGA_5A_GPIO20  : inout std_logic;    --FPGA_GPIO_Set0(3)    --AC27
	-- FPGA_3A_GPIO26  : inout std_logic;    --FPGA_GPIO_Set0(4)    --AD27
	-- FPGA_5A_GPIO18  : inout std_logic;    --FPGA_GPIO_Set0(5)    --AD26
	-- FPGA_5A_GPIO13  : inout std_logic;    --FPGA_GPIO_Set0(6)    --AF28
	-- FPGA_3A_GPIO19  : inout std_logic;    --FPGA_GPIO_Set0(7)    --AD25
	-- FPGA_3A_GPIO24  : inout std_logic;    --FPGA_GPIO_Set0(8)    --AE26
	-- FPGA_5B_GPIO3   : inout std_logic;    --FPGA_GPIO_Set0(9)    --Y27
	-- FPGA_5B_GPIO7   : inout std_logic;    --FPGA_GPIO_Set0(10)    --AC29
	-- FPGA_5B_GPIO2   : inout std_logic;    --FPGA_GPIO_Set0(11)    --Y26

	-- FPGA_5A_GPIO6   : inout std_logic;    --FPGA_GPIO_Set1(0)    --AA24
	-- FPGA_5A_GPIO5   : inout std_logic;    --FPGA_GPIO_Set1(1)    --AB23
	-- FPGA_5A_GPIO4   : inout std_logic;    --FPGA_GPIO_Set1(2)    --AB26
	-- FPGA_5B_GPIO5   : inout std_logic;    --FPGA_GPIO_Set1(3)    --AC28
	-- FPGA_3A_GPIO21  : inout std_logic;    --FPGA_GPIO_Set1(4)    --AC25
	-- FPGA_5A_GPIO9   : inout std_logic;    --FPGA_GPIO_Set1(5)    --AE28
	-- FPGA_5A_GPIO15  : inout std_logic;    --FPGA_GPIO_Set1(6)    --AF29
	-- FPGA_5A_GPIO7   : inout std_logic;    --FPGA_GPIO_Set1(7)    --AE27
	-- FPGA_5B_GPIO10  : inout std_logic;    --FPGA_GPIO_Set1(8)    --AA30
	-- FPGA_5B_GPIO6   : inout std_logic;    --FPGA_GPIO_Set1(9)    --V25
	-- FPGA_5B_GPIO8   : inout std_logic;    --FPGA_GPIO_Set1(10)    --AB30
	-- FPGA_5B_GPIO4   : inout std_logic;    --FPGA_GPIO_Set1(11)    --W25
	-- FPGA_5B_GPIO1   : inout std_logic;    --FPGA_GPIO_Set1(12)    --AD29
	-- FPGA_5B_GPIO0   : inout std_logic;    --FPGA_GPIO_Set1(13)    --AE29
	-- FPGA_5A_GPIO16  : inout std_logic;    --FPGA_GPIO_Set1(14)    --W24
	-- FPGA_5A_GPIO12  : inout std_logic;    --FPGA_GPIO_Set1(15)    --Y24
	-- FPGA_5A_GPIO14  : inout std_logic;    --FPGA_GPIO_Set1(16)    --V23
	-- FPGA_5A_GPIO19  : inout std_logic;    --FPGA_GPIO_Set1(17)    --AH30

	-- FPGA_3A_GPIO15  : inout std_logic;    --FPGA_GPIO_Set2(0)    --AC12
	-- FPGA_3A_GPIO7   : inout std_logic;    --FPGA_GPIO_Set2(1)    --AA12
	-- FPGA_8A_GPIO4   : inout std_logic;    --FPGA_GPIO_Set2(2)    --J12
	-- FPGA_8A_GPIO18  : inout std_logic;    --FPGA_GPIO_Set2(3)    --J10
	-- FPGA_8A_GPIO10  : inout std_logic;    --FPGA_GPIO_Set2(4)    --K7
	-- FPGA_8A_GPIO20  : inout std_logic;    --FPGA_GPIO_Set2(5)    --J9
	-- FPGA_8A_GPIO16  : inout std_logic;    --FPGA_GPIO_Set2(6)    --F10
	-- FPGA_8A_GPIO22  : inout std_logic;    --FPGA_GPIO_Set2(7)    --F9
	-- FPGA_8A_GPIO24  : inout std_logic;    --FPGA_GPIO_Set2(8)    --F8
	-- FPGA_8A_GPIO13  : inout std_logic;    --FPGA_GPIO_Set2(9)    --E2
	-- FPGA_8A_GPIO17  : inout std_logic;    --FPGA_GPIO_Set2(10)    --D1
	-- FPGA_8A_GPIO7   : inout std_logic;    --FPGA_GPIO_Set2(11)    --E4
	-- FPGA_8A_GPIO9   : inout std_logic;    --FPGA_GPIO_Set2(12)    --D4
	-- FPGA_8A_GPIO19  : inout std_logic;    --FPGA_GPIO_Set2(13)    --E7

	-- FPGA_3A_GPIO5   : inout std_logic;    --FPGA_GPIO_Set3(0)    --AB12
	-- FPGA_8A_GPIO1   : inout std_logic;    --FPGA_GPIO_Set3(1)    --K14
	-- FPGA_8A_GPIO2   : inout std_logic;    --FPGA_GPIO_Set3(2)    --J14
	-- FPGA_8A_GPIO6   : inout std_logic;    --FPGA_GPIO_Set3(3)    --G12
	-- FPGA_8A_GPIO12  : inout std_logic;    --FPGA_GPIO_Set3(4)    --K8
	-- FPGA_8A_GPIO8   : inout std_logic;    --FPGA_GPIO_Set3(5)    --G11
	-- FPGA_8A_GPIO14  : inout std_logic;    --FPGA_GPIO_Set3(6)    --G10
	-- FPGA_8A_GPIO23  : inout std_logic;    --FPGA_GPIO_Set3(7)    --G7
	-- FPGA_8A_GPIO25  : inout std_logic;    --FPGA_GPIO_Set3(8)    --F6
	-- FPGA_8A_GPIO15  : inout std_logic;    --FPGA_GPIO_Set3(9)    --E1
	-- FPGA_8A_GPIO11  : inout std_logic;    --FPGA_GPIO_Set3(10)    --E3
	-- FPGA_8A_GPIO3   : inout std_logic;    --FPGA_GPIO_Set3(11)    --D2
	-- FPGA_8A_GPIO5   : inout std_logic;    --FPGA_GPIO_Set3(12)    --C2
	-- FPGA_8A_GPIO21  : inout std_logic;    --FPGA_GPIO_Set3(13)    --E6
	-- FPGA_8A_GPIO0   : inout std_logic;    --FPGA_GPIO_Set3(14)    --A10

	-- FPGA_5A_GPIO8   : inout std_logic;	 --FPGA_GPIO_Set0(0)    --AB25
	-- FPGA_5B_GPIO11  : inout std_logic;    --FPGA_GPIO_Set0(1)    --AA28
	-- FPGA_5B_GPIO9   : inout std_logic;    --FPGA_GPIO_Set0(2)    --AB28
	-- FPGA_5A_GPIO20  : inout std_logic;    --FPGA_GPIO_Set0(3)    --AC27
	-- FPGA_3A_GPIO26  : inout std_logic;    --FPGA_GPIO_Set0(4)    --AD27
	-- FPGA_5A_GPIO18  : inout std_logic;    --FPGA_GPIO_Set0(5)    --AD26
	-- FPGA_5A_GPIO13  : inout std_logic;    --FPGA_GPIO_Set0(6)    --AF28
	-- FPGA_3A_GPIO19  : inout std_logic;    --FPGA_GPIO_Set0(7)    --AD25
	-- FPGA_3A_GPIO24  : inout std_logic;    --FPGA_GPIO_Set0(8)    --AE26
	-- FPGA_5B_GPIO3   : inout std_logic;    --FPGA_GPIO_Set0(9)    --Y27
	-- FPGA_5B_GPIO7   : inout std_logic;    --FPGA_GPIO_Set0(10)    --AC29
	-- FPGA_5B_GPIO2   : inout std_logic;    --FPGA_GPIO_Set0(11)    --Y26

	-- FPGA_5A_GPIO6   : inout std_logic;    --FPGA_GPIO_Set1(0)    --AA24
	-- FPGA_5A_GPIO5   : inout std_logic;    --FPGA_GPIO_Set1(1)    --AB23
	-- FPGA_5A_GPIO4   : inout std_logic;    --FPGA_GPIO_Set1(2)    --AB26
	-- FPGA_5B_GPIO5   : inout std_logic;    --FPGA_GPIO_Set1(3)    --AC28
	-- FPGA_3A_GPIO21  : inout std_logic;    --FPGA_GPIO_Set1(4)    --AC25
	-- FPGA_5A_GPIO9   : inout std_logic;    --FPGA_GPIO_Set1(5)    --AE28
	-- FPGA_5A_GPIO15  : inout std_logic;    --FPGA_GPIO_Set1(6)    --AF29
	-- FPGA_5A_GPIO7   : inout std_logic;    --FPGA_GPIO_Set1(7)    --AE27
	-- FPGA_5B_GPIO10  : inout std_logic;    --FPGA_GPIO_Set1(8)    --AA30
	-- FPGA_5B_GPIO6   : inout std_logic;    --FPGA_GPIO_Set1(9)    --V25
	-- FPGA_5B_GPIO8   : inout std_logic;    --FPGA_GPIO_Set1(10)    --AB30
	-- FPGA_5B_GPIO4   : inout std_logic;    --FPGA_GPIO_Set1(11)    --W25
	-- FPGA_5B_GPIO1   : inout std_logic;    --FPGA_GPIO_Set1(12)    --AD29
	-- FPGA_5B_GPIO0   : inout std_logic;    --FPGA_GPIO_Set1(13)    --AE29
	-- FPGA_5A_GPIO16  : inout std_logic;    --FPGA_GPIO_Set1(14)    --W24
	-- FPGA_5A_GPIO12  : inout std_logic;    --FPGA_GPIO_Set1(15)    --Y24
	-- FPGA_5A_GPIO14  : inout std_logic;    --FPGA_GPIO_Set1(16)    --V23
	-- FPGA_5A_GPIO19  : inout std_logic;    --FPGA_GPIO_Set1(17)    --AH30

	-- FPGA_3A_GPIO15  : inout std_logic;    --FPGA_GPIO_Set2(0)    --AC12
	-- FPGA_3A_GPIO7   : inout std_logic;    --FPGA_GPIO_Set2(1)    --AA12
	-- FPGA_8A_GPIO4   : inout std_logic;    --FPGA_GPIO_Set2(2)    --J12
	-- FPGA_8A_GPIO18  : inout std_logic;    --FPGA_GPIO_Set2(3)    --J10
	-- FPGA_8A_GPIO10  : inout std_logic;    --FPGA_GPIO_Set2(4)    --K7
	-- FPGA_8A_GPIO20  : inout std_logic;    --FPGA_GPIO_Set2(5)    --J9
	-- FPGA_8A_GPIO16  : inout std_logic;    --FPGA_GPIO_Set2(6)    --F10
	-- FPGA_8A_GPIO22  : inout std_logic;    --FPGA_GPIO_Set2(7)    --F9
	-- FPGA_8A_GPIO24  : inout std_logic;    --FPGA_GPIO_Set2(8)    --F8
	-- FPGA_8A_GPIO13  : inout std_logic;    --FPGA_GPIO_Set2(9)    --E2
	-- FPGA_8A_GPIO17  : inout std_logic;    --FPGA_GPIO_Set2(10)    --D1
	-- FPGA_8A_GPIO7   : inout std_logic;    --FPGA_GPIO_Set2(11)    --E4
	-- FPGA_8A_GPIO9   : inout std_logic;    --FPGA_GPIO_Set2(12)    --D4
	-- FPGA_8A_GPIO19  : inout std_logic;    --FPGA_GPIO_Set2(13)    --E7

	-- FPGA_3A_GPIO5   : inout std_logic;    --FPGA_GPIO_Set3(0)    --AB12
	-- FPGA_8A_GPIO1   : inout std_logic;    --FPGA_GPIO_Set3(1)    --K14
	-- FPGA_8A_GPIO2   : inout std_logic;    --FPGA_GPIO_Set3(2)    --J14
	-- FPGA_8A_GPIO6   : inout std_logic;    --FPGA_GPIO_Set3(3)    --G12
	-- FPGA_8A_GPIO12  : inout std_logic;    --FPGA_GPIO_Set3(4)    --K8
	-- FPGA_8A_GPIO8   : inout std_logic;    --FPGA_GPIO_Set3(5)    --G11
	-- FPGA_8A_GPIO14  : inout std_logic;    --FPGA_GPIO_Set3(6)    --G10
	-- FPGA_8A_GPIO23  : inout std_logic;    --FPGA_GPIO_Set3(7)    --G7
	-- FPGA_8A_GPIO25  : inout std_logic;    --FPGA_GPIO_Set3(8)    --F6
	-- FPGA_8A_GPIO15  : inout std_logic;    --FPGA_GPIO_Set3(9)    --E1
	-- FPGA_8A_GPIO11  : inout std_logic;    --FPGA_GPIO_Set3(10)    --E3
	-- FPGA_8A_GPIO3   : inout std_logic;    --FPGA_GPIO_Set3(11)    --D2
	-- FPGA_8A_GPIO5   : inout std_logic;    --FPGA_GPIO_Set3(12)    --C2
	-- FPGA_8A_GPIO21  : inout std_logic;    --FPGA_GPIO_Set3(13)    --E6
	-- FPGA_8A_GPIO0   : inout std_logic;    --FPGA_GPIO_Set3(14)    --A10
	*/
#endif
