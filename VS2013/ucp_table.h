/**
  ******************************************************************************
  * @file    ucp_table.h
  * @author  Chenxx
  * @version V1.0
  * @date    2016-02-02
  * @brief   This file defines access interface of ucp::access
  ******************************************************************************/
#ifndef __UCP_TABLE_H
#define __UCP_TABLE_H
#include "ucp.h"
#include "caliVar.h"
#include <math.h>

//
//	class CAccs_CpuUsage
//
class CAccs_CpuUsage:
	public ucp::CAccs_base
{
	public:
		virtual uint16_t get() const
		{
			return 1;
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

//
// class CAccs_MotorPos
//
#include "BLDC_Driver.h"
class CAccs_MotorPos:
	public ucp::CAccs_base
{
	public:
		virtual uint16_t get() const
		{
			return driver.Get_Pos_();
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

//
// class CAccs_MotorVel
//
class CAccs_MotorVel:
	public ucp::CAccs_base
{
	public:
		virtual uint16_t get() const
		{
			return driver.Get_Vel_();
		}
		virtual uint8_t get_sign() const
		{
			return 1;
		}
};

//
// class CAccs_MotorAcc
//
class CAccs_MotorAcc:
	public ucp::CAccs_base
{
	public:
		virtual uint16_t get() const
		{
			return 32767;
		}
		virtual uint8_t get_sign() const
		{
			return 1;
		}
};

//
//class CAccs_CoilCur
//
class CAccs_CoilCur :
	public ucp::CAccs_base
{
public:
	virtual uint16_t get() const
	{
		return driver.Get_Cur_();
	}
	virtual uint8_t get_sign() const
	{
		return 1;
	}
};

//
// class CAccs_MotorPwmDuty
//
class CAccs_MotorPwmDuty:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t duty_phy)
		{
			driver.Set_PwmDuty10k(duty_phy);
		}
		virtual uint16_t get() const
		{
			return driver.Get_PwmDuty10k();
		}
		virtual uint8_t get_sign() const
		{
			return 1;
		}
};

//
// class CAccs_MotorRotateDir
//
class CAccs_MotorRotateDir:
	public ucp::CAccs_base
{
	public:
		virtual uint16_t get() const
		{
			return 1;
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

//
// class CAccs_MotorCtrlMode
//
class CAccs_MotorCtrlMode:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t mode)
		{
			driver.ctrl_mode_ = (uint8_t)mode;
		}
		virtual uint16_t get() const
		{
			return driver.ctrl_mode_;
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

CAccs_MotorCtrlMode accs_motor_ctrlmode;

//
// class CAccs_MotorTarVal
//
class CAccs_MotorTarVal:
	public ucp::CAccs_base
{
	public:
		CAccs_MotorTarVal()
		{}
		virtual void set(uint16_t value)
		{
			driver.tar_ctrl_val_ = value;
		}
		virtual uint16_t get() const
		{
			return driver.tar_ctrl_val_;
		}
		virtual uint8_t get_sign() const
		{
			uint8_t mode = (uint8_t)accs_motor_ctrlmode.get();
			if(mode == CBLDC_Driver::open_loop) //open loop
				return 0;
			else if (mode == CBLDC_Driver::pos_ctrl) //pCtrl sign
				return 0;
			else if (mode == CBLDC_Driver::vel_ctrl) //vCtrl sign
				return 1;
			else if (mode == CBLDC_Driver::cur_ctrl) //iCtrl sign
				return 1;
			else if(mode == 4) //pinc_Ctrl sign
				return 1;
			else
				return 0;
		}
};


//
// class CAccs_VCtrlKp
//

class CAccs_VCtrlKp:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t value)
		{
			driver.Get_vctrl_()->Set_Kp(value);
		}
		virtual uint16_t get() const
		{
			return (driver.Get_vctrl_()->Get_m_().Kp);
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

//
// class CAccs_VCtrlKi
//

class CAccs_VCtrlKi:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t value)
		{
			driver.Get_vctrl_()->Set_Ki(value);
		}
		virtual uint16_t get() const
		{
			return (driver.Get_vctrl_()->Get_m_().Ki);
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

//
// class CAccs_VCtrlKd
//

class CAccs_VCtrlKd:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t value)
		{
			driver.Get_vctrl_()->Set_Kd(value);
		}
		virtual uint16_t get() const
		{
			return (driver.Get_vctrl_()->Get_m_().Kd);
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};


//
// class CAccs_PCtrlKp
//

class CAccs_PCtrlKp:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t value)
		{
			
		}
		virtual uint16_t get() const
		{
			return 65535;
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

//
// class CAccs_PCtrlKi
//

class CAccs_PCtrlKi:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t value)
		{
			
		}
		virtual uint16_t get() const
		{
			return 65535;
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};

//
// class CAccs_PCtrlKd
//

class CAccs_PCtrlKd:
	public ucp::CAccs_base
{
	public:
		virtual void set(uint16_t value)
		{
			
		}
		virtual uint16_t get() const
		{
			return 65535;
		}
		virtual uint8_t get_sign() const
		{
			return 0;
		}
};
//
// declaration
//
CAccs_CpuUsage accs_cpuusage;
CAccs_MotorPos accs_motorpos;
CAccs_MotorVel accs_motorvel;
CAccs_MotorAcc accs_motoracc;
CAccs_CoilCur	acc_coilcur;
CAccs_MotorPwmDuty accs_motor_pwmduty;
CAccs_MotorRotateDir accs_motor_rotatedir;

CAccs_MotorTarVal accs_motor_tarval;
CAccs_VCtrlKp	accs_vctrl_kp;
CAccs_VCtrlKi	accs_vctrl_ki;
CAccs_VCtrlKd	accs_vctrl_kd;
CAccs_PCtrlKp	accs_pctrl_kp;
CAccs_PCtrlKi	accs_pctrl_ki;
CAccs_PCtrlKd	accs_pctrl_kd;

void ucp_table_Init()
{
	ucp::instance()->Registry_[clv::CPUUSAGE] = &accs_cpuusage;					//Cpu Usage				[0, 10000]
	ucp::instance()->Registry_[clv::MOTORPOS] = &accs_motorpos;					//Pos of Motor		[0, 65535]
	ucp::instance()->Registry_[clv::MOTORVEL] = &accs_motorvel;					//Vel of Motor		[-32768, 32767]		
	ucp::instance()->Registry_[clv::MOTORACC] = &accs_motoracc;					//Acc of Motor		[-32768, 32767]
	ucp::instance()->Registry_[clv::COILCUR] = &acc_coilcur;						//Cur of Coil			[0, 65535]
	ucp::instance()->Registry_[clv::PWMDUTY] = &accs_motor_pwmduty;			//PWM Duty				[0, 10000]
	ucp::instance()->Registry_[clv::ROTATEDIR] = &accs_motor_rotatedir;	//Dir of Rotate		[0, 1]
	ucp::instance()->Registry_[clv::CTRLMODE] = &accs_motor_ctrlmode;		//Mode of Control	[0, 2]
	ucp::instance()->Registry_[clv::TARVAL] = &accs_motor_tarval;				//Target Value		[?]
	ucp::instance()->Registry_[clv::VCTRL_KP] = &accs_vctrl_kp;					//Kp of vControl	[0, 65535]	
	ucp::instance()->Registry_[clv::VCTRL_KI] = &accs_vctrl_ki;					//Ki of vControl	[0, 65535]
	ucp::instance()->Registry_[clv::VCTRL_KD] = &accs_vctrl_kd;					//Kd of vControl	[0, 65535]
	ucp::instance()->Registry_[clv::PCTRL_KP] = &accs_pctrl_kp;					//Kp of pControl	[0, 65535]	
	ucp::instance()->Registry_[clv::PCTRL_KI] = &accs_pctrl_ki;					//Ki of pControl	[0, 65535]
	ucp::instance()->Registry_[clv::PCTRL_KD] = &accs_pctrl_kd;					//Kd of pControl	[0, 65535]
	
	ucp::instance()->Registry_[0]->set(clv::MOTORVEL); 
	ucp::instance()->Registry_[1]->set(clv::PWMDUTY); 
	ucp::instance()->Registry_[2]->set(clv::MOTORPOS); //Target Value
	ucp::instance()->Registry_[3]->set(clv::CPUUSAGE); //Target Value
	
	
}
#endif
//end of file
