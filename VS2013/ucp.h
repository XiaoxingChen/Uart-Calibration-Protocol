/**
******************************************************************************
* @file    ucp.h
* @author  Chenxx
* @version V1.0
* @date    2016-02-18
* @brief   This file defines the uart calibration protocol. 
*						This ucp is for PC.
******************************************************************************/
#ifndef __UCP_H
#define __UCP_H
#include <stdint.h>
#include "list.h"
#include <windows.h>
class CUart;
typedef CUart CUsart;
class ucp
{
public:
	class CAccs_base
	{
	public:
		CAccs_base()
		{}
		virtual uint16_t get() const;
		virtual void set(uint16_t);
		virtual uint8_t get_sign() const;
	};
	struct TxdPkgTyp
	{
		uint16_t PackHead;
		uint8_t ErrorCode;
		uint8_t TimeStamp_H;
		uint16_t TimeStamp_L;
		uint8_t DataIdx[4];
		uint16_t Data[4];
		uint8_t DataSign;			//[0..3] sign of data
		uint8_t PackEnd;
	};
	struct RxdPkgTyp
	{
		uint16_t PackHead;
		uint8_t rsv;
		uint8_t DataIdx;
		uint16_t Data;
		uint8_t rsv2;
		uint8_t PackEnd;
	};
	friend void ucp_table_Init();

	static ucp* instance();
	static DWORD WINAPI run(LPVOID lpParameter);
	void update(uint8_t);
	uint8_t pop_RxdQue_();
	uint8_t send_Message(uint8_t);
	/*const CUsart* Get_Usart_()const;*/	//only usable in MCU
	static const uint8_t REG_SIZE;
	~ucp();

private:
	//
	//	statics
	//
	ucp();
	static uint32_t TimeStamp_;
	static uint16_t sendPeriod_;
	static uint8_t RxdBuf_[sizeof(RxdPkgTyp)];
	static TxdPkgTyp TxdBuf_;
	static list<RxdPkgTyp> RxdQue_;
	static CAccs_base* Registry_[];
	static CAccs_base** DataIdx_;
	static ucp* instance_;

	//
	// Usart
	//
	CUsart* Usart_;
	void CUsart_Init();
};

//
//CAccs_dataIdx
//
class CAccs_dataIdx :
	public ucp::CAccs_base
{
public:
	CAccs_dataIdx() :Idx_(2)
	{}
	virtual void set(uint16_t Idx)
	{
		if (Idx < ucp::REG_SIZE)
			Idx_ = Idx;
	}
	virtual uint16_t get() const
	{
		return Idx_;
	}
private:
	uint16_t Idx_;
};
#endif
//end of file
