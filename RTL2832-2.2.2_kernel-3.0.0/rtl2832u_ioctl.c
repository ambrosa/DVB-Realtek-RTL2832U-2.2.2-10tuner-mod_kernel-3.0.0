#include "rtl2832u_ioctl.h"
int rtl2832u_ioctl_override(struct dvb_frontend *fe,unsigned int cmd, void *parg,unsigned int stage);
int 
set_rtl2832_ioctrl_8bit_stdi2c(
	struct dvb_frontend 		*fe,
	unsigned short			dev_i2c_addr,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength);
	
int 
get_rtl2832_ioctrl_8bit_stdi2c(
	struct dvb_frontend 		*fe,
	unsigned short			dev_i2c_addr,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength);
int 
set_rtl2832_ioctrl_8bit_demodbytes(
	struct dvb_frontend 		*fe,
	unsigned char 			page,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength);	
int 
get_rtl2832_ioctrl_8bit_demodbytes(
	struct dvb_frontend 		*fe,
	unsigned char 			page,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength);

extern int rtl2832_read_signal_quality(
	struct dvb_frontend*	fe,
	u32*	quality);
extern int 
rtl2832_read_signal_strength(
	struct dvb_frontend*	fe,
	u16*	strength);
		
static int fe_rw_std_cmd_ioctrl(struct dvb_frontend *fe, void *parg)
{

	struct rtl2832u_ioctl_rwcmd_struct*  rw_ctrl = (struct rtl2832u_ioctl_rwcmd_struct*)parg;
	int ret =0;
	int rw_cmd = rw_ctrl->cmd_num;
	
	deb_info("+%s : ioctl -std rw cmd = %d \n", __FUNCTION__,rw_cmd);
	

	switch(rw_cmd)
	{
		case FE_RW_STD_WR:
			ret=set_rtl2832_ioctrl_8bit_stdi2c(	fe,
								rw_ctrl->dev_i2c_addr,
								rw_ctrl->reg_addr,
								rw_ctrl->data,
								rw_ctrl->bytelength);
			
		break;
		case FE_RW_STD_RD:
		
			ret=get_rtl2832_ioctrl_8bit_stdi2c(	fe,
								rw_ctrl->dev_i2c_addr,
								rw_ctrl->reg_addr,
								rw_ctrl->data,
								rw_ctrl->bytelength);			
		
		
		break;	
		case FE_RW_DEMOD_WR:
	
			ret=set_rtl2832_ioctrl_8bit_demodbytes( fe,
								rw_ctrl->page,
								rw_ctrl->reg_addr,
								rw_ctrl->data,
								rw_ctrl->bytelength);	
		break;
		case FE_RW_DEMOD_RD:

			ret=get_rtl2832_ioctrl_8bit_demodbytes( fe,
								rw_ctrl->page,
								rw_ctrl->reg_addr,
								rw_ctrl->data,
								rw_ctrl->bytelength);			
		break;
		case FE_RW_STD_P_TEST:
			ret=1;
		break;
		case FE_RW_STD_M_TEST:
			ret=100;
		break;
	default:
		deb_info("ERROR::rtl2832u ioctl - std rw cmd ret= unknow rw commond. \n" );	
		ret=-1;	
	break;
	}

	deb_info("-%s ioctl - std rw cmd ret= %d \n", __FUNCTION__,ret);

	return ret;
	
}
static int fe_demod_info_ioctrl(struct dvb_frontend *fe, void *parg)
{

	struct rtl2832u_ioctl_info_struct*  info_ctrl = (struct rtl2832u_ioctl_info_struct*)parg;
	int ret =-1;
	struct rtl2832_state*	p_state = fe->demodulator_priv;
	DVBT_NIM_MODULE 	*pNim;
	DVBT_DEMOD_MODULE 	*pDemod;
	u16			strength=0;
	u32			quality=0;
	int 			answer=0;
	unsigned long 		BerNum=0,BerDen=1;
	long 			SnrDbNum=0, SnrDbDen=1;
	long 			tr_ppm=0,cr_hz=0;
	unsigned long 		data_length=info_ctrl->data_length;
	int 			info_cmd = info_ctrl->cmd_num;
	deb_info("+%s : ioctl -demod info cmd = %d ,data length= %d (must > %d )\n", __FUNCTION__,info_cmd,(int)data_length,INFO_DEMOD_DATA_LEN);
	if (p_state->demod_type != RTL2832){
		deb_info("ERROR::demod type not rtl2832u...\n");
		return -1;
	}
	if (data_length < INFO_DEMOD_DATA_LEN){
		deb_info("ERROR::data length is too short...,must be greater than %d \n",INFO_DEMOD_DATA_LEN);
		return -1;
	}
	pNim = p_state->pNim;	
	if (pNim == NULL){
		deb_info("ERROR::nim is null...\n");
		return -1;
	}
	
	pDemod=pNim->pDemod;	
	
	
	if ((info_cmd != FE_GET_SIGN_STRENGTH) && (info_cmd != FE_GET_SIGN_QUALITY))
	{
			if( mutex_lock_interruptible(&p_state->i2c_repeater_mutex) )	{
				return -10;
			}
	}
	info_ctrl->data[INFO_DATA_VALUE]=0x00000000;
	info_ctrl->data[INFO_DATA_DEN]=1;
	switch(info_cmd)
	{
		case FE_GET_SIGN_STRENGTH:
			if (rtl2832_read_signal_strength(fe,&strength) != 0)
			{
				ret=-10;
			}
			else
			{
				ret=10;
				info_ctrl->data[INFO_DATA_VALUE]=(unsigned long)strength;
				info_ctrl->data[INFO_DATA_DEN]=1;
//				deb_info("rtl2832u ioctl (FE_GET_SIGN_STRENGTH)- strength = %x (%x). \n",strength,info_ctrl->data[INFO_DATA_VALUE] );
	
			}

			
		break;
		case FE_GET_SIGN_QUALITY:
			if (rtl2832_read_signal_quality(fe,&quality) != 0)
			{
				ret=-10;
			}
			else
			{
				ret=10;
				info_ctrl->data[INFO_DATA_VALUE]=(unsigned long)quality;
				info_ctrl->data[INFO_DATA_DEN]=1;
//				deb_info("rtl2832u ioctl (FE_GET_SIGN_QUALITY)- quality = %x (%x). \n",quality,info_ctrl->data[INFO_DATA_VALUE] );
			}
			
		break;
		case FE_GET_SIGN_LOCK:

			answer=NO;
			if (pDemod->IsSignalLocked(pDemod, &answer) != FUNCTION_SUCCESS)
			{
				ret=-10;
			}
			else
			{
				if (answer == YES){
					info_ctrl->data[INFO_DATA_VALUE]=1;
				}
				else	info_ctrl->data[INFO_DATA_VALUE]=0;
				
				info_ctrl->data[INFO_DATA_DEN]=1;
				ret=10;
			}
			

		break;
		case FE_GET_DEMOD_BER:

			if(pDemod->GetBer(pDemod, &BerNum, &BerDen) != FUNCTION_SUCCESS)
			{
				ret=-10;
				info_ctrl->data[INFO_DATA_VALUE]=19616;
				info_ctrl->data[INFO_DATA_DEN]=1;	
			}
			else
			{
				ret=10;
				info_ctrl->data[INFO_DATA_VALUE]=BerNum;
				info_ctrl->data[INFO_DATA_DEN]=BerDen;				
			}
		
		break;
		case FE_GET_DEMOD_SNR_DB:			
			if(pDemod->GetSnrDb(pDemod, &SnrDbNum, &SnrDbDen) != FUNCTION_SUCCESS)
			{
				ret=-10;
				info_ctrl->data[INFO_DATA_VALUE]=0;
				info_ctrl->data[INFO_DATA_DEN]=1;	
			}
			else
			{
				ret=10;
				info_ctrl->data[INFO_DATA_VALUE]=SnrDbNum;
				info_ctrl->data[INFO_DATA_DEN]=SnrDbDen;				
			}		
		break;
		case FE_GET_DEMOD_TR_PPM:
			if(pDemod->GetTrOffsetPpm(pDemod, &tr_ppm) != FUNCTION_SUCCESS)
			{
				ret=-10;
				info_ctrl->data[INFO_DATA_VALUE]=0;
				info_ctrl->data[INFO_DATA_DEN]=1;	
			}
			else
			{
				ret=10;
				info_ctrl->data[INFO_DATA_VALUE]=tr_ppm;
				info_ctrl->data[INFO_DATA_DEN]=1;				
			}	
		break;
		case FE_GET_DEMOD_CR_HZ:
			if(pDemod->GetCrOffsetHz(pDemod, &cr_hz) != FUNCTION_SUCCESS)
			{
				ret=-10;
				info_ctrl->data[INFO_DATA_VALUE]=0;
				info_ctrl->data[INFO_DATA_DEN]=1;	
			}
			else
			{
				ret=10;
				info_ctrl->data[INFO_DATA_VALUE]=cr_hz;
				info_ctrl->data[INFO_DATA_DEN]=1;				
			}					
		break;
	default:
		deb_info("ERROR::rtl2832u ioctl - std info cmd ret= unknow info commond. \n" );	
		ret=-1;	
	break;
	}


	if ((info_cmd != FE_GET_SIGN_STRENGTH) && (info_cmd != FE_GET_SIGN_QUALITY)){
		mutex_unlock(&p_state->i2c_repeater_mutex);
	}

//	deb_info("-%s :ioctl - std info cmd ret= %x,data=%x,den=%x\n", __FUNCTION__,ret,(int)info_ctrl->data[INFO_DATA_VALUE],info_ctrl->data[INFO_DATA_DEN]);

	 return ret;
	
}
int rtl2832u_ioctl_override(struct dvb_frontend *fe,unsigned int cmd, void *parg,unsigned int stage)
{
	int ret = 0;
	deb_info("+%s[20101201_1828] ioctl \n", __FUNCTION__);
	if(stage == DVB_FE_IOCTL_PRE)
	{
		switch(cmd)
		{
			case FE_RW_SID_CMD:

				ret = fe_rw_std_cmd_ioctrl(fe, parg);
			if (ret < 0){
			   	 ret=-10;}
   			else{
	   			 ret=10;}
				
			break;
			case FE_RW_DEMOD_INFO:
				ret = fe_demod_info_ioctrl(fe, parg);
			if (ret < 0)
			   	 	{ret=-10;}
   				else{
	   			 	ret=10;}
			break;	

		}
	
	}
	
	 deb_info("-%s ioctl - ret= %d stage=%d \n", __FUNCTION__,ret,stage);
	 return ret;

}

int 
set_rtl2832_ioctrl_8bit_stdi2c(
	struct dvb_frontend 		*fe,
	unsigned short			dev_i2c_addr,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength)
{
	int i=0;
	int ret = -1;
	unsigned char data_temp[MAX_STD_I2C_BUF_NUM];  
	struct rtl2832_state*	p_state = fe->demodulator_priv;
	struct dvb_usb_device*	d = p_state->d;
	deb_info("+%s dev_i2c_addr = 0x%x reg_addr=0x%x bytelength=%d \n", __FUNCTION__,dev_i2c_addr, reg_addr,bytelength);

	if (bytelength > 128 ){
		deb_info("ERROR::bytelength > 128\n");
		return -1;
	} 
	data_temp[0]=reg_addr;
	for (i=0;i<bytelength;i++)
	{
		data_temp[i+1]=data[i];
	}
	//memcpy(&data_temp[1],data,bytelength);

	deb_info("write data[]=[");
	for(i=0;i<(bytelength+1);i++)
	{
		deb_info("%x,",data_temp[i]);
	}
	deb_info("]\n");
	if ( write_rtl2832_stdi2c( d, dev_i2c_addr , data_temp , bytelength+1 ) ){ 
		ret=-1;
		goto error;
	}
	if( mutex_lock_interruptible(&p_state->i2c_repeater_mutex) )	
		 return -1;


	
	ret=0;
error:
	mutex_unlock(&p_state->i2c_repeater_mutex);
	deb_info("-%s: dev_i2c_addr = 0x%x reg_addr=0x%x bytelength=%d \n", __FUNCTION__,dev_i2c_addr, reg_addr,bytelength);
	return ret;

	
}
int 
get_rtl2832_ioctrl_8bit_stdi2c(
	struct dvb_frontend 		*fe,
	unsigned short			dev_i2c_addr,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength)
{
	int ret =-1;
	int i;
	struct rtl2832_state*	p_state = fe->demodulator_priv;
	struct dvb_usb_device*	d = p_state->d;	
	if (bytelength > 128 ){
		deb_info("ERROR::bytelength > 128\n");
		return -1;
	} 
	deb_info("+%s[1347]: dev_i2c_addr = 0x%x reg_addr=0x%x bytelength=%d \n", __FUNCTION__,dev_i2c_addr, reg_addr,bytelength);	
	if( mutex_lock_interruptible(&p_state->i2c_repeater_mutex) )	
		 return -1;

	if ( write_rtl2832_stdi2c( d, dev_i2c_addr , &reg_addr , LEN_1_BYTE ) )
	{ 
		ret=-1;
		goto error;
	}
	if ( read_rtl2832_stdi2c( d, dev_i2c_addr , data , bytelength ) ) 
	{ 
		ret=-1;
		goto error;
	}

	deb_info("read data[]=[");
	for(i=0;i<bytelength;i++)
	{
		deb_info("%x,",data[i]);
	}
	deb_info("]\n");	
	ret=0;
	
error:
	mutex_unlock(&p_state->i2c_repeater_mutex);
	deb_info("-%s: dev_i2c_addr = 0x%x reg_addr=0x%x bytelength=%d \n", __FUNCTION__,dev_i2c_addr, reg_addr,bytelength);
	return ret;  	
	
}
int 
set_rtl2832_ioctrl_8bit_demodbytes(
	struct dvb_frontend 		*fe,
	unsigned char 			page,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength)
{

	int ret = -1;
 	int i=0;
	struct rtl2832_state*	p_state = fe->demodulator_priv;
	unsigned char 		data_temp[128];
	DVBT_NIM_MODULE 	*pNim;
	DVBT_DEMOD_MODULE 	*pDemod;
	
	deb_info("+%s[1546]: page= %d reg_addr=0x%x bytelength=%d \n", __FUNCTION__,page, reg_addr,bytelength);

	if (bytelength > 128 ){
		deb_info("ERROR::bytelength > 128\n");
		return -1; 
	} 
	
	if (p_state->demod_type != RTL2832){
		deb_info("ERROR::demod type not rtl2832u...\n");
		return -1;
	}
	pNim = p_state->pNim;	
	if (pNim == NULL){
		return -1;
	}
	if( mutex_lock_interruptible(&p_state->i2c_repeater_mutex) ){
		 return -1;
	}	
	pDemod=pNim->pDemod;
	
	if(pDemod->SetRegPage(pDemod, page) != FUNCTION_SUCCESS){
		ret=-1;
		goto error;
	}
	for(i=0;i<bytelength;i++){
		data_temp[i]=data[i];
	}
	if(pDemod->SetRegBytes(pDemod, reg_addr, data_temp, bytelength) != FUNCTION_SUCCESS)	{
		ret=-1;
		goto error;
	}
	deb_info("-%s: data[%d,%x]= \n", __FUNCTION__,page, reg_addr);
	for (i=0;i<bytelength;i++){
		deb_info("%x(%x),",data[i],data_temp[i]);
	}
		
	deb_info("\n");

	ret=0;
error:
	mutex_unlock(&p_state->i2c_repeater_mutex);
	deb_info("-%s: page=%d reg_addr=0x%x bytelength=%d \n", __FUNCTION__,page, reg_addr,bytelength);
	return ret;

	
}
int 
get_rtl2832_ioctrl_8bit_demodbytes(
	struct dvb_frontend 		*fe,
	unsigned char 			page,
	unsigned char 			reg_addr,
	unsigned char*			data,
	unsigned short			bytelength)
{
	
	int ret = -1;
	int i=0;

	struct rtl2832_state*	p_state = fe->demodulator_priv;
	unsigned char data_temp[128]; 

	DVBT_NIM_MODULE 	*pNim;
	DVBT_DEMOD_MODULE 	*pDemod;
	
	deb_info("+%s: page= %d reg_addr=0x%x bytelength=%d \n", __FUNCTION__,page, reg_addr,bytelength);

	if (bytelength > 128 ){
		deb_info("ERROR::bytelength > 128\n");
		return -1; 
	} 
	
	if (p_state->demod_type != RTL2832){
		deb_info("ERROR::demod type not rtl2832u...\n");
		return -1;
	}
	pNim = p_state->pNim;	
	if (pNim == NULL){
		return -1;
	}
	if( mutex_lock_interruptible(&p_state->i2c_repeater_mutex) ){
		 return -1;
	}	
	pDemod=pNim->pDemod;
	
	if(pDemod->SetRegPage(pDemod, page) != FUNCTION_SUCCESS){
		ret=-1;
		goto error;
	}	
	if(pDemod->GetRegBytes(pDemod, reg_addr, data_temp, bytelength) != FUNCTION_SUCCESS)	{
		ret=-1;
		goto error;
	}
	deb_info("->%s: data[%d,%x]= \n", __FUNCTION__,page, reg_addr);
	for (i=0;i<bytelength;i++){
		data[i]=data_temp[i];
		deb_info("%x(%x),",data[i],data_temp[i]);
	}
		
	deb_info("\n");
	ret=0;
error:
	mutex_unlock(&p_state->i2c_repeater_mutex);
	deb_info("-%s: page=%d reg_addr=0x%x bytelength=%d \n", __FUNCTION__,page, reg_addr,bytelength);
	return ret;
	
}
