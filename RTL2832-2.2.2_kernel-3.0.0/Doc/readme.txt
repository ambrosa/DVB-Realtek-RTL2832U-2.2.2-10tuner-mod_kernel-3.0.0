Driver Installation in Linux

1.  Download "v4l-dvb-(version)" source code from http://linuxtv.org/. 

2.	Copy the folder 'rtl2832u_linux_driver' and "v4l-dvb-(version)" source code to the desktop.

3.	Click 'Applications' -> 'Accessories' -> 'Terminal' to enter the console mode. 

4.	Type 'cd /root/Desktop/rtl2832u_linux_driver' to enter the folder.

5.	In the folder 'rtl2832u_linux_driver', type the following command to compile & install. 

	a. Type 'cp -f *.* /root/Desktop/v4l-dvb-(version)/linux/drivers/media/dvb/dvb-usb' to copy all files into v4l-dvb-(version) code.

	b. add the following lines to Makefile in v4l-dvb-(version)/linux/drivers/media/dvb/dvb-usb.
dvb-usb-rtl2832u-objs = demod_rtl2832.o	dvbt_demod_base.o dvbt_nim_base.o foundation.o math_mpi.o nim_rtl2832_mxl5007t.o nim_rtl2832_fc2580.o nim_rtl2832_mt2266.o rtl2832u.o rtl2832u_fe.o rtl2832u_io.o tuner_mxl5007t.o tuner_fc2580.o tuner_mt2266.o tuner_tua9001.o nim_rtl2832_tua9001.o tuner_fc0012.o nim_rtl2832_fc0012.o demod_rtl2836.o dtmb_demod_base.o dtmb_nim_base.o nim_rtl2836_fc2580.o nim_rtl2836_mxl5007t.o tuner_e4000.o nim_rtl2832_e4000.o tuner_mt2063.o demod_rtl2840.o tuner_max3543.o nim_rtl2832_mt2063.o nim_rtl2832_max3543.o nim_rtl2840_mt2063.o nim_rtl2840_max3543.o qam_demod_base.o qam_nim_base.o tuner_tda18272.o nim_rtl2832_tda18272.o rtl2832u_ioctl.o nim_rtl2832_fc0013.o tuner_fc0013.o
obj-$(CONFIG_DVB_USB_RTL2832U) += dvb-usb-rtl2832u.o

	c. add the following lines to Kconfig in v4l-dvb-(version)/linux/drivers/media/dvb/dvb-usb.	
config DVB_USB_RTL2832U
	tristate "Realtek RTL2832U DVB-T USB2.0 support"
	depends on DVB_USB
	help
	  Realtek RTL2832U DVB-T driver.
				
   	d. Type 'make clean'
   	d. Type 'make'   	
   	f. Type 'make install'

	#if compile have error:

		  CC [M]  v4l..../v4l/firedtv-1394.o
			  v4l..../v4l/firedtv-1394.c:21:17: error: dma.h: No such file or directory
			  v4l..../v4l/firedtv-1394.c:22:21: error: csr1212.h: No such file or directory
			  v4l..../v4l/firedtv-1394.c:23:23:.....
			  ......................................
	to solve:
	[a]
	 please modify following lines to the v4l....\linux\drivers\media\dvb\Kconfig file 

		#comment "Supported FireWire (IEEE 1394) Adapters"
		#	depends on DVB_CORE && IEEE1394
	 	#source "drivers/media/dvb/firewire/Kconfig"
	 and 	modify following lines to the v4l....\linux\drivers\media\dvb\Makefile file
	
		#obj-$(CONFIG_DVB_FIREDTV)	+= firewire/
	[b]modify .config file 
	   In the folder 'v4l.....' ,type the following command to modify .config
	   'sudo gedit v4l/.config'
	   modify  "CONFIG_DVB_FIREDTV=m" change to "CONFIG_DVB_FIREDTV=n"	

		  	

#obj-$(CONFIG_DVB_FIREDTV)	+= firewire/
		
6.	Plug in our DVB-T USB device;

7.	Type 'lsmod | grep dvb', and it will show
   	dvb_usb_rtl2832u
   	dvb_usb
   	dvb_core
   	i2c_core
    
    Your driver has been installed successfully.

8. Install the applications --'Xine' and 'linuxtv-dvb-apps'. 


=================================================================================================================

Some Description 
1  After Version 1.3, the driver can support DVB-T, DTMB, and DVB-C. 
   Use the parameter "demod" to choose one of tv modes when driver is probed.
   If demod = 0,  DVB-T mode is enabled. 
   If demod = 1,  DTMB mode is enabled if only rtl2836 is checked present. Or DVB-T mode is enabled.
   If demod = 2,  DVB-C mode is enabled if only rtl2840 is checked present. Or DVB-T mode is enabled.
	
	ex: #sudo insmod dvb-usb-rtl2832u.ko demod=2 (for DVB-C)
   

2  Use the parameter "dtmb_err_discard" to choose whether to discard error TS packet or not when device is on dtmb mode.
   If dtmb_err_discard = 0, when signal is weak, error TS packets will not be discard, and AP will get them. 
   In dtmb_err_discard = 1, error packet will be discard by the device, AP will not get them.
   In some AP(ex. kaffeine), error packets seems to disturb the AP, and AP will not resume playing even when the signal goes better afterwards. You can use "dtmb_err_discard = 1" to solve the problem. But in some other AP(ex. mplayer), the situation will not happen, there's no need to use this parameter.


3  If any problems, please set the parameter "debug=1" to show debug messages when driver is probed, and
   send "/var/log/messages" file to us. 
   
	ex: #sudo insmod dvb-usb-rtl2832u.ko debug=1
	
	
   
=================================================================================================================
rc function use 
1.After Version 2.2.2 the driver can support rc function 

2.Use the parameter "rtl2832u_rc_mode" to select rc protocol
  	if rtl2832u_rc_mode   0     use RC6 protocol
	if rtl2832u_rc_mode   1     use RC5 protocol
	if rtl2832u_rc_mode   2     use NEC protocol  (Default)
	if rtl2832u_rc_mode   3     disable rc function

3.Must to edit KEY_MAP, KEY_MAP is define in "rtl2832u_rc_keys_map_table" in "rtl2832u.c"

	static struct dvb_usb_rc_key rtl2832u_rc_keys_map_table[] = { 	
		{ 0x0400, KEY_0 },           // 0 
		{ 0x0401, KEY_1 },           // 1 
		{ 0x0402, KEY_2 },           // 2 
		.....................
		....................
	};
	
	The first column(two byte values) of this table is rc protocal key.
	When driver receives the rc key that is equal to the value of the first column, it will transform it to the keybord event of the second column.
	Take an example. If driver receives rc key "0x400", it will set "KEY_0" keyboard event to control player.
	
	We show the rc key for each button of the remote controller in log.
	1. install this driver module.
	2. press one button in remote controller.
	3. check /var/log/messages file and get the following line
	"-rtl2832u_rc_query scan code 00 00 04 00,(0x0400) -- len=4" 
	It means the rc key of this button is "0x0400".
	

4.If any problems or view rc scancode, please set the parameter "debug=3" to show debug messages
  and send "/var/log/messages" file to us. 