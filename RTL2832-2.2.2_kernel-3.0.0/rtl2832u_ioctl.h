#ifndef __RTL2832U_IOCTL_H__
#define __RTL2832U_IOCTL_H__

#include "rtl2832u_fe.h"
#include "rtl2832u_io.h"

#include "dvbdev.h"


struct rtl2832u_ioctl_rwcmd_struct
{
	int cmd_num;
	unsigned char page;
	unsigned char dev_i2c_addr;
	unsigned long reg_addr;
	unsigned char *data;          
	unsigned int  bytelength;

};
struct rtl2832u_ioctl_info_struct
{
	int cmd_num;
	unsigned long *data;
	unsigned int  data_length;
};

enum FE_RW_DEMOD_DATA_TAG{
		INFO_DATA_VALUE=0,
		INFO_DATA_DEN=1,
		INFO_DEMOD_DATA_LEN=2
};
#define FE_RW_SID_CMD    _IOWR('o', 81,  struct rtl2832u_ioctl_rwcmd_struct) 
#define FE_RW_DEMOD_INFO  _IOWR('o', 82,  struct rtl2832u_ioctl_info_struct)

#define MAX_STD_I2C_BUF_NUM 131

enum FE_STD_RW_CMD_TAG{
	FE_RW_STD_WR=0,
	FE_RW_STD_RD=1,
	FE_RW_DEMOD_WR=2,
	FE_RW_DEMOD_RD=3,
	FE_RW_STD_P_TEST=4,
	FE_RW_STD_M_TEST=5,
	FE_RW_STD_NUM	
};

enum FE_RW_DEMOD_INFO_TAG{
	FE_GET_SIGN_STRENGTH=0,
	FE_GET_SIGN_QUALITY=1,
	FE_GET_SIGN_LOCK=2,
	FE_GET_DEMOD_BER=3,
	FE_GET_DEMOD_SNR_DB=4,
	FE_GET_DEMOD_TR_PPM=5,
	FE_GET_DEMOD_CR_HZ=6,
	FE_DEMOD_INFO_NUM
};
int rtl2832u_ioctl_override(struct dvb_frontend *fe,unsigned int cmd, void *parg,unsigned int stage);
	
#endif
