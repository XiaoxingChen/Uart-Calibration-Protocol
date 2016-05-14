/**
  ******************************************************************************
  * @file    CUcp.h
  * @author  Chenxx
  * @version V1.1
  * @date    2016-02-01
  * @brief   This file defines the uart calibration protocol
  ******************************************************************************/
#ifndef __CUCP_H
#define __CUCP_H
#include "stm32f10x.h"
#include "list.h"
#include "CaliVar.h"
#include "Singleton.h"

//BSP MACROS
#define UCP_USE_UART1		1									//change UARTx
#define UCP_IOGROUP_A9	1									//change IOGROUP_xx
#define UCP_TX_DMACH		DMA1_Channel4 		//change DMAx_Channely
#define UCP_RX_DMACH		DMA1_Channel5 		//change DMAx_Channely
#define UCP_RX_DMA_IRQ	DMA1_Channel5_IRQn//change DMAx_Channely

#ifdef 	UCP_USE_UART1 
	#define 	UCP_UART	USART1	
#elif 	UCP_USE_UART2
	#define 	UCP_UART	USART2	
#elif 	UCP_USE_UART3
	#define 	UCP_UART	USART3
#elif 	UCP_USE_UART4
	#define 	UCP_UART	UART4
#elif 	UCP_USE_UART5
	#define 	UCP_UART	UART5
#endif

#define UCP_DMA				((DMA_TypeDef *)((uint32_t)UCP_TX_DMACH&0xFFFFFC00))

class CUcp
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
	
	void update();
	void OnMessageReceive();
	uint8_t pop_RxdQue_();
	uint8_t send_Message();
	void set_sendFreq(uint16_t);
	uint16_t get_sendFreq()const;
	CUcp();
	void reboot_rxDMA();
private:
	
	uint8_t send_period_;
	uint32_t TimeStamp_;
	CAccs_base** DataIdx_;
	static uint8_t RxdBuf_[sizeof(RxdPkgTyp)];
	static TxdPkgTyp TxdBuf_;
	static list<RxdPkgTyp> RxdQue_;
	static CAccs_base* Registry_[];

	void InitSciGpio();
	void InitSci();
	void sendArray(uint8_t*, uint8_t);
};

//
//CAccs_dataIdx
//
class CAccs_dataIdx:
	public CUcp::CAccs_base
{
	public:
		CAccs_dataIdx():Idx_(2)
		{}
		virtual void set(uint16_t Idx)
		{
			if(Idx < clv::VAR_NUM)
				Idx_ = Idx;
		}
		virtual uint16_t get() const
		{
			return Idx_;
		}
	private:
		uint16_t Idx_;
};

typedef NormalSingleton<CUcp> ucp;
#endif
//end of file
