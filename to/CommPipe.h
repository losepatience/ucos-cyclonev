#ifndef _COMMPIPE_H_
#define _COMMPIPE_H_

INT8U InitCommPipe(INT8U channelNumber);

#define COMMPIPE_HEADBOARD_CHANNEL_ID   1
#define COMMPIPE_DSP_CHANNEL_ID         2
#define COMMPIPE_ARM_CHANNEL_ID         3
INT8U PushCommPipeData(INT8U channelNO, INT8U *pBuff, INT16U size, INT8U IsBlockModel);

#define MAX_PIPE_CMD_LEN    504
INT8U PushHugePipeData(INT16U ArmCmd, INT8U *pBuff, INT16U size);

void CommPipe_StartOneSendEP6();

#endif
