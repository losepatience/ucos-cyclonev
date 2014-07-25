oo
==

ucos

usb:	fpga 栈深至少 2 才给 hps 发中断
uart:	确认 wait 失败原因, OS-MAX-EVENTS(太小).


pin:	ctrl0-ctrl15(ink supply) output(buf to 24v) J36-J39
pin:	stepper pwm&dir&reset(0,1) output  J32-J33

encoder:	AB(dirction:)
V = pulses / S * l
     __
A __|  |__
      __
B  __|  |__

Z 0

pin:	encoder([xyz]2[abz]) pulse input J25 J35 J49-J52(fpga fixed) 
pin:	motor([xzy]pulse&dir) output pulse&dir J20 J21 J34 J43-J44
	J46 J48 (fpga-fixed)

pin:	sensor(limit&origin) sensor notify input
	J4-J8 J10-J14 J54-J57

uv led: 
	status:
		over-hot
		ready
		flault(故障)
		shutter open ok
		shutter close ok

	control:
		open
		high_power
		shutter(open or close immediately)

pin:	UV(lp)(buf to 24V) J9

