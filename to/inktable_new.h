#define COLOR_NUMBER 6 	//0: K		1: C		2:M			3:Y			4: LC		5:LM
#define JET_SPEED	 3 	//0: 5m/s; 	1: 5.5m/s; 	2:6m/s 
#define INK_TYPE	 5	//0: UV;	1: INK_A;	2:INK_B		3: INK_C	4: INK_E

const float voltageRatio_42P[JET_SPEED][INK_TYPE][COLOR_NUMBER][30] = 
{
	{//5M/S	
		{//Toyo UV INK 35~50 dgree, standard is 45 dgree
			{	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,
			0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.74,	0.74,	0.74,	0.74,
			0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74}, //Black
			{	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,
			0.75,	0.74,	0.73,	0.72,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,
			0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.74,	0.71,	0.71,	0.71}, //Cyan
			{	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,
			0.78,	0.77,	0.76,	0.75,	0.74,	0.73,	0.73,	0.73,	0.73,	0.73,
			0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73}, //Magenta
			{	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,
			0.76,	0.75,	0.74,	0.73,	0.72,	0.71,	0.71,	0.71,	0.71,	0.71,
			0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71}, //Yellow
			{	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,
			0.80,	0.79,	0.78,	0.77,	0.76,	0.75,	0.75,	0.75,	0.75,	0.75,
			0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75}, //Light Magenta
			{	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.75,
			0.74,	0.73,	0.72,	0.71,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,
			0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70}  //Light Cyan
		}, 
		{//INK_A
		},
		{//INK_B
			{ 1.01, 1.00,	0.99,	0.98,	0.97,	0.96,	0.95,	0.94,	0.93,	0.92,
			0.91, 0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,
			0.82,	0.81,	0.80,	0.79,	0.78,	0.78,   0.77,	0.76,	0.76,	0.75}, //Black
			{ 1.08, 1.07,	1.06,	1.05,	1.04,	1.03,	1.02,	1.00,	0.99,	0.97,
			0.96, 0.95,	0.94,	0.92,	0.91,	0.90,	0.89,	0.88,	0.86,	0.85,
			0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.75},  //Cyan
			{ 1.03, 1.02,   1.01,   1.00,   0.99,   0.98,	0.97,	0.96,	0.95,	0.94,
			0.93, 0.92,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.75,	0.75}, //Magenta
			{ 1.12,	1.11,	1.10,	1.09,	1.07,	1.06,	1.04,	1.03,	1.01,	1.00,  //10~19
			0.99, 0.97,	0.96,	0.95,	0.93,	0.92,	0.91,	0.90,	0.88,	0.87,  //20~29
			0.86, 0.85,	0.84,	0.83,	0.82,  	0.81,	0.80,	0.79,   0.78,	0.77}, //yellow	
			{ 1.11,	1.10,	1.09,	1.08,	1.07,	1.06,	1.05,	1.03,	1.01,	1.00,  //10~19
			0.99, 0.97,	0.96,	0.94,	0.93,	0.92,	0.91,	0.89,	0.88,	0.87,  //20~29
			0.86, 0.85,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,   0.77,	0.76}, //Light Magenta	
			{ 1.12,	1.11,	1.10,	1.09,	1.08,	1.07,	1.06,	1.04,	1.02,	1.01,  //10~19
			0.99, 0.98,	0.97,	0.95,	0.94,	0.92,	0.91,	0.90,	0.89,	0.87,  //20~29
			0.86, 0.85,	0.84,	0.83,	0.82,   0.80,	0.79,	0.78,   0.77,	0.76}, //Light Cyan	
		},
		{//INK_C
			{	1.09,	1.08,	1.07,	1.06,	1.05, 	1.04,	1.03,	1.02,	1.0,	0.99,
			0.98,	0.96,	0.95,	0.94,	0.93,	0.91,	0.9,	0.89,	0.88,	0.87,
			0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77	},  //Black
			{ 	1.09,	1.08,	1.07,	1.06,	1.05,	1.04,	1.03,	1.01,	1.0,	0.99,
			0.97,	0.96,	0.95,	0.94,	0.92,	0.91,	0.9,	0.89,	0.88,	0.87,
			0.86,	0.85,	0.84,	0.83,	0.82,	0.81,  	0.80,	0.79,	0.78,   0.77,	}, //Cyan	
			{	1.10, 	1.09,   1.08,   1.07,   1.06,   1.05,	1.03,	1.02,	1.01,	0.99,
			0.98,	0.97,	0.95,	0.94,	0.93,	0.92,	0.9,	0.89,	0.88,	0.87,
			0.86,	0.85,	0.84,	0.83,	0.82,	0.81,  	0.80,	0.79,	0.78,	0.77	}, //Magenta
			{	1.09, 	1.08,	1.07,	1.06,	1.05,	1.04,	1.02,	1.01,	1.00,	0.98,
			0.97,	0.96,	0.95,	0.94,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,
			0.86,	0.85,	0.84,	0.83,	0.82,	0.81,  	0.80,	0.79,	0.78,	0.77	},  //Yellow
			{ 	1.14,	1.13,	1.12,	1.11,	1.10,	1.09,	1.07,	1.06,	1.04,	1.03,
			1.01,	1.0,	0.98,	0.97,	0.95,	0.94,	0.93,	0.91,	0.9,	0.89,
			0.88,	0.86,	0.85,	0.84,	0.83,	0.82,  	0.81,	0.80,	0.79,   0.78	}, //Light Magenta	
			{ 	1.16,	1.15,	1.14,	1.13,	1.12,	1.11,	1.09,	1.07,	1.06,	1.04,	
			1.03,	1.01,	1.0,	0.98,	0.97,	0.95,	0.94,	0.93,	0.91,	0.9,
			0.89,	0.88,	0.87,	0.85,	0.84,	0.83,  	0.82,	0.81,	0.80,   0.79	}, //Light Cyan	
		},		
		{//INK_E //Data is come from ALLWIN, Light Magenta, Light Cyan use INK_C data
			{	1.05,	1.04,	1.03,	1.02,	1,		0.98,	0.97,	0.96,	0.95,	0.94,
			0.93,	0.91,	0.89,	0.88,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,
			0.8,	0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.73,	0.72,	0.71	}, //Black
			{ 	1.06,	1.05,	1.04,	1.03,	1.02,	1,		0.99,	0.98,	0.97,	0.96,
			0.94,	0.93,	0.91,	0.9,	0.89,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.73,	0.72	}, //Cyan
			{	1.05,	1.04,	1.02,	1,		0.99,	0.98,	0.96,	0.95,	0.94,	0.93,
			0.92,	0.91,	0.89,	0.88,	0.87,	0.86,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.81,	0.8,	0.79,	0.79,	0.78,	0.77,	0.76,	0.75	}, //Magenta
			{	1.08,	1.07,	1.06,	1.05,	1.04,	1.03,	1.01,	1,		0.98,	0.96,
			0.95,	0.93,	0.91,	0.89,	0.88,	0.85,	0.84,	0.83,	0.83,	0.82,
			0.81,	0.8,	0.79,	0.78,	0.78,	0.77,	0.76,	0.75,	0.74,	0.73	}, //Yellow
			{ 	1.14,	1.13,	1.12,	1.11,	1.10,	1.09,	1.07,	1.06,	1.04,	1.03,
			1.01,	1.0,	0.98,	0.97,	0.95,	0.94,	0.93,	0.91,	0.9,	0.89,
			0.88,	0.86,	0.85,	0.84,	0.83,	0.82,  	0.81,	0.80,	0.79,   0.78	}, //Light Magenta	
			{ 	1.16,	1.15,	1.14,	1.13,	1.12,	1.11,	1.09,	1.07,	1.06,	1.04,	
			1.03,	1.01,	1.0,	0.98,	0.97,	0.95,	0.94,	0.93,	0.91,	0.9,
			0.89,	0.88,	0.87,	0.85,	0.84,	0.83,  	0.82,	0.81,	0.80,   0.79	} //Light Cyan	
		}
	},//5M/S
	{//5.5M/S
		{//Toyo UV INK 35~50 dgree, standard is 45 dgree. There is no 42PL data, just use same data with 14PL,need change
			{	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.77,	0.77,	0.77,	0.77,
			0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77}, //Black
			{	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,
			0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.74,	0.74,	0.74,	0.74,
			0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74}, //Cyan
			{	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,
			0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.76,	0.76,	0.76,	0.76,
			0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76}, //Magenta
			{	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,
			0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.74,	0.74,	0.74,	0.74,
			0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74}, //Yellow
			{	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,
			0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.78,	0.78,	0.78,	0.78,
			0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78}, //Light Magenta
			{	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,
			0.77,	0.76,	0.75,	0.74,	0.73,	0.72,	0.72,	0.72,	0.72,	0.72,
			0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72}  //Light Cyan
		},		
		{//INK_A
		},
		{//INK_B
			{ 1.05,	1.04,	1.03,	1.02,	1.01, 	1.00,	0.99,	0.98,	0.97,	0.96,  //10~19
			0.95, 0.94,	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,  //20~29
			0.86,	0.85,	0.84,	0.83,	0.82,	0.82,	0.81,	0.81,	0.80,	0.80 },  //Black
			{ 1.11, 1.10,	1.09,	1.08,	1.07,	1.06,	1.05,	1.04,	1.02,	1.01,
			1.00, 0.99,	0.97,	0.96,	0.95,	0.94,	0.93,	0.92,	0.90,	0.89,
			0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.80},  //Cyan
			{ 1.08, 1.07,   1.06,   1.05,   1.04,   1.03,	1.01,	1.00,	0.99,	0.98,
			0.97, 0.96,	0.95,	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,
			0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.80,	0.79}, //Magenta
			{ 1.14,	1.13,	1.12,	1.11,	1.10,	1.09,	1.08,	1.07,	1.05,	1.04,  //10~19
			1.02, 1.01,	1.00,	0.98,	0.97,	0.96,	0.95,	0.93,	0.92,	0.91,  //20~29
			0.90, 0.89,	0.88,	0.87,	0.86,  	0.85,	0.84,	0.83,   0.82,	0.81}, //yellow	
			{ 1.14,	1.13,	1.12,	1.11,	1.10,	1.09,	1.08,	1.06,	1.05,	1.04,  //10~19
			1.02, 1.01,	0.99,	0.98,	0.97,	0.96,	0.94,	0.93,	0.92,	0.91,  //20~29
			0.90, 0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,   0.81,	0.80}, //Light Magenta	
			{ 1.15,	1.14,	1.13,	1.12,	1.11,	1.10,	1.09,	1.07,	1.06,	1.04,  //10~19
			1.03, 1.01,	1.00,	0.99,	0.97,	0.96,	0.95,	0.94,	0.92,	0.91,  //20~29
			0.90, 0.89,	0.88,	0.87,	0.85,   0.84,	0.83,	0.82,   0.81,	0.80}, //Light Cyan	
		},
		{	//INK_C
			{ 1.13,	1.12,	1.11,	1.10,	1.09, 	1.08,	1.07,	1.05,	1.04,	1.03,  //10~19
			1.01, 1.00,	0.99,	0.98,	0.97,	0.95,	0.94,	0.93,	0.92,	0.91,  //20~29
			0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81 },  //Black
			{ 1.13, 1.12,	1.11,	1.10,	1.09,	1.08,	1.06,	1.05,	1.04,	1.02,
			1.01, 1.00,	0.99,	0.97,	0.96,	0.95,	0.94,	0.93,	0.92,	0.91,
			0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80},  //Cyan
			{ 1.13, 1.12,   1.11,   1.10,   1.09,   1.08,	1.07,	1.06,	1.04,	1.03,
			1.02, 1.00,	0.99,	0.98,	0.97,	0.95,	0.94,	0.93,	0.92,	0.91,
			0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81}, //Magenta
			{ 1.13,	1.12,	1.11,	1.10,	1.09,	1.08,	1.06,	1.05,	1.04,	1.02,  //10~19
			1.01, 1.00,	0.99,	0.98,	0.96,	0.95,	0.94,	0.93,	0.92,	0.91,  //20~29
			0.90, 0.89,	0.88,	0.87,	0.86,  	0.85,	0.84,	0.83,   0.82,	0.81}, //yellow	
			{ 1.17,	1.16,	1.15,	1.14,	1.13,	1.12,	1.11,	1.09,	1.08,	1.06,  //10~19
			1.05, 1.03,	1.02,	1.00,	0.99,	0.98,	0.96,	0.95,	0.94,	0.93,  //20~29
			0.91, 0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,   0.83,	0.82}, //Light Magenta	
			{ 1.19,	1.18,	1.17,	1.16,	1.15,	1.14,	1.13,	1.11,	1.09,	1.08,  //10~19
			1.06, 1.05,	1.03,	1.02,	1.01,	0.99,	0.98,	0.96,	0.95,	0.94,  //20~29
			0.93, 0.91,	0.90,	0.89,	0.88,   0.87,	0.86,	0.85,   0.84,	0.83}, //Light Cyan	
		},
		{//INK_E ��5M/S + 0.3����
			{	1.08,	1.07,	1.06,	1.05,	1.03,	1.01,	1.00,	0.99,	0.98,	0.97,
			0.96,	0.94,	0.92,	0.91,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,
			0.83�� 	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.75,	0.74	}, //Black
			{ 	1.09,	1.08,	1.07,	1.06,	1.05,	1.04,	1.02,	1.01,	1.00,	0.99,
			0.97,	0.96,	0.94,	0.93,	0.92,	0.90,	0.89,	0.88,	0.87,	0.86,
			0.85,	0.84,	0.83,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.75	}, //Cyan
			{	1.08,	1.07,	1.05,	1.03,	1.02,	1.01,	0.99,	0.98,	0.97,	0.96,
			0.95,	0.94,	0.92,	0.91,	0.90,	0.89,	0.89,	0.88,	0.87,	0.86,
			0.85,	0.84,	0.84,	0.83,	0.82,	0.82,	0.81,	0.80,	0.79,	0.78	}, //Magenta
			{	1.11,	1.10,	1.09,	1.08,	1.07,	1.06,	1.04,	1.03,	1.01,	0.99,
			0.98,	0.96,	0.94,	0.92,	0.91,	0.88,	0.87,	0.86,	0.86,	0.85,
			0.84,	0.83,	0.82,	0.81,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76	}, //Yellow
			{ 	1.17,	1.16,	1.15,	1.14,	1.13,	1.12,	1.10,	1.09,	1.07,	1.06,
			1.04,	1.03,	1.01,	1.00,	0.98,	0.97,	0.96,	0.94,	0.93,	0.92,
			0.91,	0.89,	0.88,	0.87,	0.86,	0.85,  	0.84,	0.83,	0.82,   0.81	}, //Light Magenta	
			{ 	1.19,	1.18,	1.17,	1.16,	1.15,	1.14,	1.12,	1.10,	1.09,	1.07,	
			1.06,	1.04,	1.03,	1.01,	1.00,	0.98,	0.97,	0.96,	0.94,	0.93,
			0.92,	0.91,	0.90,	0.88,	0.87,	0.86,  	0.85,	0.84,	0.83,   0.82	} //Light Cyan	
		}
	}//5.5M/S
	{//6M/S
		{//Tuyo 
			{	0.95,	0.94,	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,
			0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.80,	0.80,	0.80,	0.80,
			0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80}, //Black
			{	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.77,	0.77,	0.77,	0.77,
			0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77}, //Cyan
			{	0.94,	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,
			0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.79,	0.79,	0.79,	0.79,
			0.79,	0.79,	0.79,	0.79,	0.79,	0.79,	0.79,	0.79,	0.79,	0.79}, //Magenta
			{	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.77,	0.77,	0.77,	0.77,
			0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77}, //Yellow
			{	0.95,	0.94,	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,
			0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.80,	0.80,	0.80,	0.80,
			0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80,	0.80}, //Light Magenta
			{	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,
			0.80,	0.79,	0.78,	0.77,	0.76,	0.75,	0.75,	0.75,	0.75,	0.75,
			0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75}  //Light Cyan
#elif 	defined(INK_B)
			{ 1.09, 1.08, 1.07, 1.06, 1.05, 
			1.04, 1.03, 1.02, 1.01, 1.00, 0.99, 0.98, 0.97, 0.96, 0.95,
			0.94, 0.93, 0.92, 0.91, 0.91, 0.90, 0.89, 0.88, 0.87, 0.86, 0.86, 
			0.85, 0.84, 0.83, 0.82 	}, //K
			{ 1.15,1.14,1.13,1.12,1.11,
			1.10,1.09,1.07,1.06,1.05,1.03,1.02,1.01,1.00,0.99,
			0.98,0.96,0.95,0.94,0.93,0.92,0.91,0.90,0.89,0.88,0.87,
			0.86,0.85,0.84,0.83	},  //C 
			{ 1.12, 1.11, 1.10, 1.09, 1.08,
			1.07, 1.05, 1.04, 1.03, 1.02, 1.01, 1.00, 0.99, 0.97, 0.96,
			0.95, 0.94, 0.93, 0.92, 0.91, 0.90, 0.90, 0.89, 0.88, 0.87, 0.86, 
			0.85, 0.84, 0.83, 0.82	}, //M
			{ 1.18, 1.17, 1.16,	1.15, 1.14,	
			1.13, 1.12,	1.10, 1.09,	1.07, 1.06,	1.05, 1.03, 1.02, 1.01,
			1.00, 0.99,	0.97, 0.96,	0.95, 0.94,	0.93, 0.92,	0.91, 0.90, 0.89, 
			0.88,	0.87, 0.86,	0.85 },  //Y
			{ 1.18, 1.17, 1.16, 1.15, 1.14, 
			1.13, 1.11, 1.10, 1.09, 1.07, 1.06, 1.04, 1.03, 1.02, 1.01, 
			0.99, 0.98, 0.97, 0.96, 0.95, 0.94, 0.92, 0.91, 0.90, 0.89, 0.88, 
			0.87, 0.86, 0.85, 0.84	},  //Lm  
			{ 1.19,	1.18,	1.17,	1.16, 1.15,
			1.14,	1.12, 1.11,	1.09, 1.08, 1.06, 1.05, 1.04, 1.02, 1.01,	
			1.00,	0.99,	0.97,	0.96,	0.95,	0.94,	0.93,	0.92,	0.90,	0.89,	0.88,
			0.87,	0.86, 0.85,	0.84	} //Lc
#elif	defined(INK_C)|defined(INK_E)|defined(INK_E2)
			{ 	1.17, 1.16, 1.15, 1.14, 1.13, 
			1.12, 1.11, 1.09, 1.08, 1.07, 1.05, 1.04, 1.03, 1.02, 1.00, 
			0.99, 0.98, 0.97, 0.96, 0.95, 0.94, 0.93, 0.92, 0.91, 0.90, 0.89, 
			0.88, 0.87, 0.86, 0.85 	},
			{	1.16, 1.15, 1.14, 1.13, 1.12,	
			1.11,	1.10,	1.09,	1.07,	1.06, 1.05,	1.04, 1.02, 1.01, 1.00, 
			0.99, 0.98, 0.97, 0.95, 0.94, 0.93, 0.92, 0.91, 0.90, 0.89, 0.88, 
			0.87, 0.86, 0.85, 0.84 	},
			{	1.17, 1.16, 1.15, 1.14, 1.13, 
			1.12, 1.11, 1.09, 1.08, 1.07, 1.05, 1.04, 1.03, 1.02, 1.00, 
			0.99, 0.98, 0.97, 0.96, 0.95, 0.94, 0.93, 0.92, 0.91, 0.90, 0.89, 
			0.88, 0.87, 0.86, 0.85 	},  
			{	1.17, 1.16, 1.15,	1.14, 1.13,	
			1.12, 1.10,	1.09, 1.08,	1.06, 1.05,	1.04, 1.03, 1.01, 1.00, 
			0.99, 0.98, 0.97, 0.96, 0.95, 0.94, 0.93, 0.92, 0.91, 0.90, 0.89, 
			0.88, 0.87, 0.86, 0.85	},  
			{	1.21, 1.20, 1.19, 1.18, 1.17, 
			1.16, 1.14, 1.13, 1.11, 1.10, 1.08, 1.07, 1.05, 1.04, 1.03, 
			1.01, 1.00, 0.99, 0.98, 0.96, 0.95, 0.94, 0.93, 0.92, 0.91, 0.90, 
			0.89, 0.88, 0.87, 0.86	},  
			{	1.23, 1.22, 1.21, 1.20, 1.19, 
			1.18, 1.16, 1.14, 1.13, 1.11, 1.10, 1.06, 1.07, 1.06, 1.04, 
			1.03, 1.01, 1.00, 0.99, 0.98, 0.96, 0.95, 0.94, 0.93, 0.92, 0.91, 
			0.90, 0.89, 0.88, 0.87  }
#elif defined(INK_F)	  
			"Not defined ink type."
#else
				"Not defined ink type."
#endif	  
		};
		
		
		//INWEAR use the default value, 5.5MPS
		const float voltageRatio_14P_55MPS[COLOR_NUMBER][30] = {
#ifdef UV_PRINTER //Toyo UV INK 35~50 dgree, standard is 45 dgree
			{	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.77,	0.77,	0.77,	0.77,
			0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77,	0.77}, //Black
			{	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,
			0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.74,	0.74,	0.74,	0.74,
			0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74}, //Cyan
			{	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,
			0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.76,	0.76,	0.76,	0.76,
			0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76,	0.76}, //Magenta
			{	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,
			0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.74,	0.74,	0.74,	0.74,
			0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74}, //Yellow
			{	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,
			0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.78,	0.78,	0.78,	0.78,
			0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78,	0.78}, //Light Magenta
			{	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,
			0.77,	0.76,	0.75,	0.74,	0.73,	0.72,	0.72,	0.72,	0.72,	0.72,
			0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72,	0.72}  //Light Cyan
#elif defined(INK_B) 
			{	1.06,	1.05,	1.04,	1.03,	1.02, 	
			1.01,	1.00,	0.99,	0.98,	0.96,
			0.95,	0.94,	0.93,	0.92,	0.91,	0.90,	0.89,	0.88,	0.87,	0.86,
			0.85,	0.84,	0.83,	0.82,	0.81,	0.80,
			0.79,	0.78,	0.77,	0.76	},  //Black
			{ 	1.14,	1.13,	1.12,	1.11,	1.10,	
			1.09,	1.07,	1.06,	1.05,	1.03,
			1.02,	1.00,	0.99,	0.98,	0.97,	0.95,	0.94,	0.93,	0.92,	0.91,
			0.90,	0.88,	0.87,	0.86,	0.85,	0.84,
			0.83,	0.82,	0.81,	0.80	}, //Cyan
			{	1.04, 	1.03,   1.02,   1.01,   1.00,   
			0.99,	0.98,	0.98,	0.97,	0.96,
			0.95,	0.94,	0.93,	0.92,	0.91,	0.91,	0.90,	0.89,	0.88,	0.87,
			0.87,	0.86,	0.85,	0.84,	0.84,	0.83,
			0.82,	0.81,	0.80,	0.79	}, //Magenta
			{	1.16, 	1.15,	1.14,	1.13,	1.12,	
			1.11,	1.10,	1.08,	1.07,	1.05,
			1.03,	1.02,	1.01,	0.99,	0.98,	0.96,	0.95,	0.94,	0.92,	0.91,
			0.90,	0.89,	0.88,	0.86,	0.85,	0.84,
			0.83,	0.82,	0.81,	0.80	},  //Yellow
			{ 	1.17,	1.16,	1.15,	1.14,	1.13,	
			1.12,	1.11,	1.09,	1.07,	1.06,
			1.04,	1.03,	1.01,	1.00,	0.98,	0.97,	0.96,	0.94,	0.93,	0.92,
			0.91,	0.89,	0.88,	0.87,	0.86,	0.85,
			0.84,	0.83,	0.82,	0.81	}, //Light Magenta	
			{ 	1.22,	1.21,	1.20,	1.19,	1.18,	
			1.17,	1.15,	1.13,	1.11,	1.09,
			1.07,	1.06,	1.04,	1.02,	1.01,	0.99,	0.97,	0.96,	0.95,	0.93,
			0.92,	0.90,	0.89,	0.88,	0.86,	0.85,
			0.84,	0.83,	0.82,	0.81	}, //Light Cyan	
#elif	defined(INK_C)|defined(INK_E)|defined(INK_E2)
{ 1.14,	1.13,	1.12,	1.11,	1.10, 	
1.09,	1.08,	1.06,	1.05,	1.03,  //10~19
1.02, 1.01,	0.99,	0.98,	0.97,	0.95,	0.94,	0.93,	0.92,	0.91,  //20~29
0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	
0.83,	0.82,	0.81,	0.80 },  //Black
{ 1.15, 1.14,	1.13,	1.12,	1.11,	
1.10,	1.09,	1.07,	1.06,	1.05,
1.03, 1.02,	1.00,	0.99,	0.98,	0.97,	0.95,	0.94,	0.93,	0.92,
0.91,	0.89,	0.88,	0.87,	0.86,	0.85,
0.84,	0.83,	0.82,	0.81},  //Cyan
{ 1.13, 1.12,   1.11,   1.10,   1.09,   
1.08,	1.07,	1.05,	1.04,	1.03,
1.01, 1.00,	0.99,	0.97,	0.96,	0.95,	0.94,	0.93,	0.91,	0.90,
0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	
0.83,	0.82,	0.81,	0.80}, //Magenta
{ 1.20,	1.19,	1.18,	1.17,	1.16,	
1.15,	1.13,	1.11,	1.09,	1.08,  //10~19
1.06, 1.04,	1.03,	1.01,	1.00,	0.98,	0.97,	0.96,	0.94,	0.93,  //20~29
0.92, 0.90,	0.89,	0.88,	0.87,  	0.86,
0.85,	0.84,   0.83,	0.82}, //yellow	
{ 1.20,	1.19,	1.18,	1.17,	1.16,	
1.15,	1.13,	1.11,	1.10,	1.08,  //10~19
1.07, 1.05,	1.04,	1.02,	1.01,	1.00,	0.98,	0.97,	0.96,	0.94,  //20~29
0.93, 0.92,	0.91,	0.90,	0.89,	0.87,
0.86,	0.85,   0.84,	0.83}, //Light Magenta	
{ 1.23,	1.22,	1.21,	1.20,	1.19,	
1.18,	1.16,	1.15,	1.13,	1.11,  //10~19
1.10, 1.08,	1.06,	1.05,	1.03,	1.02,	1.00,	0.99,	0.98,	0.96,  //20~29
0.95, 0.94,	0.92,	0.91,	0.90,   0.89,
0.88,	0.87,   0.86,	0.85}, //Light Cyan	
#elif defined(INK_F)	  
{	1.06, 	1.05,	1.04,	1.03,	1.02,
1.01,	1,		0.99,	0.98,	0.96,
0.94,	0.93,	0.92,	0.9,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,
0.83,	0.82,	0.82,	0.8,	0.79,	0.78,
0.77,	0.76,	0.75,	0.74	},  //K
{	1.07,	1.06,	1.05,	1.04,	1.03,
1.01,	1,		0.98,	0.97,	0.95,
0.94,	0.91,	0.9,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
0.82,	0.81,	0.8,	0.79,	0.78,	0.77,
0.76,	0.75,	0.74,	0.74	}, //C
{	1.06,	1.05,	1.04,	1.03,	1.02,
1,		0.99,	0.98,	0.96,	0.95,
0.94,	0.93,	0.92,	0.91,	0.9,	0.89,	0.88,	0.86,	0.85,	0.84,
0.83,	0.82,	0.81,	0.8,	0.79,	0.78,
0.77,	0.76,	0.75,	0.74	}, //M
{	1.18,	1.17,	1.16,	1.15,	1.14,
1.13,	1.12,	1.1,	1.08,	1.06,
1.04,	1.03,	1.01,	1,		0.98,	0.96,	0.95,	0.94,	0.93,	0.92,
0.91,	0.89,	0.88,	0.87,	0.86,	0.85,
0.84,	0.83,	0.82,	0.81	}, //Y
{	1.06,	1.05,	1.04,	1.03,	1.02,
1,		0.99,	0.98,	0.96,	0.95,
0.94,	0.93,	0.92,	0.91,	0.9,	0.89,	0.88,	0.86,	0.85,	0.84,
0.83,	0.82,	0.81,	0.8,	0.79,	0.78,
0.77,	0.76,	0.75,	0.74	}, //LM
{	1.07,	1.06,	1.05,	1.04,	1.03,
1.01,	1,		0.98,	0.97,	0.95,
0.94,	0.91,	0.9,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
0.82,	0.81,	0.8,	0.79,	0.78,	0.77,
0.76,	0.75,	0.74,	0.74	} //LC
#else
"Not defined ink type"
#endif	  
		};
		
		
		//K = 0,  C = 1, M = 2, Y = 3, Lm = 4, Lc = 5 
		//From 10 degree. The value under 15 degree and above 35 degree is not from spec
		//10~39
		//5m/s, use new value from ALLWIN, KM256/KM512 14PL, for type C ink
		
		const float voltageRatio_14P_5MPS[COLOR_NUMBER][30] =
#ifdef UV_PRINTER //Toyo UV INK 35~50 dgree, standard is 45 dgree. There is no 42PL data, just use same data with 14PL,need change
		{
			{	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,
			0.79,	0.78,	0.77,	0.76,	0.75,	0.74,	0.74,	0.74,	0.74,	0.74,
			0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74,	0.74}, //Black
			{	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,
			0.75,	0.74,	0.73,	0.72,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,
			0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.74,	0.71,	0.71,	0.71}, //Cyan
			{	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,
			0.78,	0.77,	0.76,	0.75,	0.74,	0.73,	0.73,	0.73,	0.73,	0.73,
			0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73,	0.73}, //Magenta
			{	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,
			0.76,	0.75,	0.74,	0.73,	0.72,	0.71,	0.71,	0.71,	0.71,	0.71,
			0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71,	0.71}, //Yellow
			{	0.90,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,	0.81,
			0.80,	0.79,	0.78,	0.77,	0.76,	0.75,	0.75,	0.75,	0.75,	0.75,
			0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75,	0.75}, //Light Magenta
			{	0.84,	0.83,	0.82,	0.81,	0.80,	0.79,	0.78,	0.77,	0.76,	0.75,
			0.74,	0.73,	0.72,	0.71,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,
			0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70,	0.70}  //Light Cyan
		}; 
#elif defined(INK_C)|defined(INK_E)|defined(INK_E2) //There is no definition for E ink of 14PL
		{
			{	1.11,	1.10,	1.09,	1.08,	1.07, 	
			1.06,	1.04,	1.03,	1.01,	1,
			0.98,	0.97,	0.96,	0.94,	0.93,	0.92,	0.9,	0.89,	0.88,	0.87,	//20~29
			0.86,	0.84,	0.83,	0.82,	0.81,	0.8,
			0.79,	0.78,	0.77,	0.76	},  //Black
			{ 	1.12,	1.11,	1.10,	1.09,	1.08,	
			1.07,	1.06,	1.04,	1.03,	1.01,
			1,		0.98,	0.97,	0.95,	0.94,	0.93,	0.91,	0.9,	0.89,	0.88,
			0.87,	0.85,	0.84,	0.83,	0.82,	0.81,
			0.80,	0.79,	0.78,   0.77,	}, //Cyan	
			{	1.10, 	1.09,   1.08,   1.07,   1.06,   
			1.05,	1.03,	1.02,	1,		0.99,	
			0.98,	0.96,	0.95,	0.94,	0.92,	0.91,	0.9,	0.89,	0.88,	0.86,
			0.85,	0.84,	0.83,	0.82,	0.81,	0.8,
			0.79,	0.78,	0.77,	0.76	}, //Magenta
			{	1.17, 	1.16,	1.15,	1.14,	1.13,	
			1.12,	1.1,	1.08,	1.06,	1.04,
			1.03,	1.01,	1,		0.98,	0.96,	0.95,	0.93,	0.92,	0.91,	0.89,
			0.88,	0.87,	0.85,	0.84,	0.83,	0.82,	
			0.81,	0.80,	0.79,	0.78	},  //Yellow
			{ 	1.16,	1.15,	1.14,	1.13,	1.12,	
			1.11,	1.1,	1.08,	1.06,	1.05,
			1.03,	1.02,	1,		0.99,	0.97,	0.96,	0.95,	0.93,	0.92,	0.91,
			0.89,	0.88,	0.87,	0.86,	0.85,	0.83,
			0.82,	0.81,	0.80,   0.79	}, //Light Magenta	
			{ 	1.20,	1.19,	1.18,	1.17,	1.16,	
			1.15,	1.13,	1.11,	1.09,	1.08,
			1.06,	1.04,	1.03,	1.01,	1,		0.98,	0.97,	0.95,	0.94,	0.92,
			0.91,	0.9,	0.89,	0.87,	0.86,	0.85,
			0.84,	0.83,	0.82,   0.81	}, //Light Cyan	
		};
#elif defined(INK_B) //ALLWIN TYPE B INK, only for 14PL
		{
			{	1.02,	1.01,	1.00,	0.99,	0.98, 	
			0.97,	0.96,	0.95,	0.94,	0.93,
			0.91,	0.9,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,	0.82,
			0.81,	0.8,	0.79,	0.78,	0.77,	0.76,
			0.75,	0.74,	0.73,	0.72	},  //Black
			{ 	1.10,	1.09,	1.08,	1.07,	1.06,	
			1.05,	1.03,	1.02,	1,		0.99,
			0.97,	0.96,	0.95,	0.93,	0.92,	0.91,	0.89,	0.88,	0.87,	0.86,
			0.85,	0.83,	0.82,	0.81,	0.8,	0.79,
			0.78,	0.77,	0.76,	0.75	}, //Cyan
			{	1.00, 	0.99,   0.98,   0.97,   0.96,   
			0.95,	0.94,	0.93,	0.92,	0.91,
			0.9,	0.89,	0.88,	0.87,	0.86,	0.86,	0.85,	0.84,	0.83,	0.82,
			0.82,	0.81,	0.8,	0.79,	0.79,	0.78,
			0.77,	0.76,	0.75,	0.74	}, //Magenta
			{	1.13, 	1.12,	1.11,	1.10,	1.09,	
			1.08,	1.06,	1.04,	1.03,	1.01,
			1,		0.98,	0.96,	0.95,	0.94,	0.92,	0.91,	0.89,	0.88,	0.87,
			0.85,	0.84,	0.83,	0.82,	0.81,	0.79,
			0.78,	0.77,	0.76,	0.75	},  //Yellow
			{ 	1.14,	1.13,	1.12,	1.11,	1.10,	
			1.09,	1.07,	1.05,	1.04,	1.02,
			1,		0.99,	0.97,	0.96,	0.94,	0.93,	0.92,	0.9,	0.89,	0.88,
			0.86,	0.85,	0.84,	0.83,	0.82,	0.8,
			0.79,	0.78,	0.77,	0.76	}, //Light Magenta	
			{ 	1.19,	1.18,	1.17,	1.16,	1.15,	
			1.14,	1.12,	1.09,	1.08,	1.06,
			1.04,	1.02,	1,		0.98,	0.97,	0.95,	0.94,	0.92,	0.9,	0.89,
			0.87,	0.86,	0.85,	0.83,	0.82,	0.81,
			0.80,	0.79,	0.78,	0.77	}, //Light Cyan	
		};
#elif defined(INK_F)
		{
			{	1.06, 	1.05,	1.04,	1.03,	1.02,
			1.01,	1,		0.99,	0.98,	0.96,
			0.94,	0.93,	0.92,	0.9,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,
			0.83,	0.82,	0.82,	0.8,	0.79,	0.78,
			0.77,	0.76,	0.75,	0.74	},  //K
			{	1.07,	1.06,	1.05,	1.04,	1.03,
			1.01,	1,		0.98,	0.97,	0.95,
			0.94,	0.91,	0.9,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.8,	0.79,	0.78,	0.77,
			0.76,	0.75,	0.74,	0.74	}, //C
			{	1.06,	1.05,	1.04,	1.03,	1.02,
			1,		0.99,	0.98,	0.96,	0.95,
			0.94,	0.93,	0.92,	0.91,	0.9,	0.89,	0.88,	0.86,	0.85,	0.84,
			0.83,	0.82,	0.81,	0.8,	0.79,	0.78,
			0.77,	0.76,	0.75,	0.74	}, //M
			{	1.18,	1.17,	1.16,	1.15,	1.14,
			1.13,	1.12,	1.1,	1.08,	1.06,
			1.04,	1.03,	1.01,	1,		0.98,	0.96,	0.95,	0.94,	0.93,	0.92,
			0.91,	0.89,	0.88,	0.87,	0.86,	0.85,
			0.84,	0.83,	0.82,	0.81	}, //Y
			{	1.06,	1.05,	1.04,	1.03,	1.02,
			1,		0.99,	0.98,	0.96,	0.95,
			0.94,	0.93,	0.92,	0.91,	0.9,	0.89,	0.88,	0.86,	0.85,	0.84,
			0.83,	0.82,	0.81,	0.8,	0.79,	0.78,
			0.77,	0.76,	0.75,	0.74	}, //LM
			{	1.07,	1.06,	1.05,	1.04,	1.03,
			1.01,	1,		0.98,	0.97,	0.95,
			0.94,	0.91,	0.9,	0.89,	0.88,	0.87,	0.86,	0.85,	0.84,	0.83,
			0.82,	0.81,	0.8,	0.79,	0.78,	0.77,
			0.76,	0.75,	0.74,	0.74	} //LC				
		};
#else
		"Not defined ink type"
#endif
			
            