/**
******************************************************************************
* @file    ucp.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-02-18
* @brief   This file achieve the uart calibration protocol.
*					This version is for PC.
******************************************************************************/
#include "ucp.h"
#include "CUart.h"
#include <iostream>
#include <vector>
/******************************************************************************
* @brief   This part for ucp
******************************************************************************/
//
// static variable
//
const uint8_t ucp::REG_SIZE = 24;
//ucp::CAccs_base* Registry[ucp::REG_SIZE] = {(ucp::CAccs_base*)0xaa55};
uint32_t ucp::TimeStamp_ = 0x000000;
uint8_t ucp::RxdBuf_[sizeof(ucp::RxdPkgTyp)];
ucp::TxdPkgTyp ucp::TxdBuf_ = { 0x5AA5 };
list<ucp::RxdPkgTyp> ucp::RxdQue_;
uint16_t ucp::sendPeriod_ = 0;

ucp::CAccs_base* ucp::Registry_[ucp::REG_SIZE] = { (ucp::CAccs_base*)0xaa55 };
ucp::CAccs_base** ucp::DataIdx_ = Registry_;
ucp* ucp::instance_ = NULL;
CUsart ucp_Usart("COM2");

//
//local
//
ucp::CAccs_base accs_empty;
CAccs_dataIdx accs_dataIdx[4];


//
//	instance()
//
ucp* ucp::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new ucp;
	}
	return instance_;
}

//
// constructor
//
ucp::ucp():
	Usart_(&ucp_Usart)
{
	CUsart_Init();
	for (int i = 0; i < 4; i++)
	{
		ucp::DataIdx_[i] = &(accs_dataIdx[i]);
		//		ucp::DataIdx_[i] = accs_dataIdx+i;
	}
	for (int i = 4; i < REG_SIZE; i++)
	{
		if (Registry_[i] == NULL)
			Registry_[i] = &accs_empty;
	}

	HANDLE hThread_1 = CreateThread(NULL, 0, run, 0, 0, NULL);
	CloseHandle(hThread_1);
}

//
// CUsart_Init
//
void ucp::CUsart_Init()
{
	if (Usart_ == NULL) std::cout << "Usart_ is NULL" << std::endl;
	Usart_->Init(115200);
}

//
// updata()
//
void ucp::update(uint8_t send_period)
{
	TimeStamp_++;
	sendPeriod_ = send_period;
	//ucp::instance()->send_Message(send_period);
	/*ucp::instance()->pop_RxdQue_();*/
}

//
//void run()
//
DWORD WINAPI ucp::run(LPVOID lpParameter)
{
	static uint32_t prevTime = 0;
	while (true)
	{
		while (prevTime == TimeStamp_) Sleep(1);
		prevTime = TimeStamp_;
		ucp::instance()->send_Message(sendPeriod_);
		ucp::instance()->pop_RxdQue_();
	}

}

//
// OnMessageReceive()
//
//RSV

//
// pop_RxdQue_()
//
#if 0
uint8_t ucp::pop_RxdQue_()
{
	static std::vector<char> temp;
	uint16_t tempsize = temp.size();
	
#if 1
	if (ucp_Usart.Get_cbInQue() > 0)
	{		
		temp.resize(tempsize + Usart_->Get_cbInQue(), 0);
		ucp_Usart.PopRxdQueue(&(temp[0]) + tempsize);
	}
#else
	if (Usart_->Get_cbInQue() > 0)
	{
		//temp.resize(tempsize + Usart_->Get_cbInQue(), 0);
		//Usart_->PopRxdQueue(&(temp[0]) + tempsize);
		Usart_->PopRxdQueue((char*)RxdBuf_);
		for (int i = 0; i < min(test, 20); i++)
		{
			std::cout << std::hex << (uint16_t)(uint8_t)RxdBuf_[i] << " ";
		}
		std::cout << std::endl;
	}
#endif
	

	RxdPkgTyp tempPack;
	if (temp.size() < sizeof(tempPack))
		return 0;

	while (true)
	{
		if ((uint8_t)temp[0] == 0x55 &&
			(uint8_t)temp[1] == 0xAA &&
			(uint8_t)temp[sizeof(tempPack)-1] == 0x00)
		{
			/* copy full pack data */
			for (uint16_t i = 0; i < sizeof(tempPack); i++)
			{
				((uint8_t*)&tempPack)[i] = temp[i];
			}
			temp.erase(temp.begin(), temp.begin() + sizeof(tempPack) - 1);

			/* execute */
			Registry_[tempPack.DataIdx]->set(tempPack.Data);
		}
		if (temp.size() <= sizeof(tempPack))
		{
			break;
			// unpack finished
		}
		
		temp.erase(temp.begin());	// pop front
	}

	if (temp.capacity() > 1)
	{
		std::vector<char> rls = temp; //release the memory that allocated by temp
		temp.swap(rls);
#if 0
		if (temp.size() > 0)
		{
			std::cout << (int)&temp[0] << std::endl;
		}
#endif
	}
	return 1;
}
#endif
uint8_t ucp::pop_RxdQue_()
{
	enum unpackMode_Type
	{
		CHECK_SIZE = 0,		//check left array size
		SRCH_HEAD = 1,		//search pack head
		CHECK_END = 2,		//check pack end
		RSERCH_TO_CHECK = 3,	//re-search pack head
		UNPACK = 4
	}unpackMode;

	unpackMode = RSERCH_TO_CHECK;
	static RxdPkgTyp tempPack;

	while (true)	//unpack state machine
	{
		/* CHECK_SIZE */
		if (unpackMode == CHECK_SIZE)	//to "break" or "SRCH_HEAD"
		{
			if (ucp_Usart.Get_cbInQue() < sizeof(tempPack))
				break;
			else
				unpackMode = SRCH_HEAD;
		}

		/* SRCH_HEAD */
		else if (unpackMode == SRCH_HEAD)	//to "SRCH_HEAD" or "CHECK_END"
		{
			ucp_Usart.PopRxdQueue((char*)&tempPack, 1);
			if (*((uint8_t*)&tempPack) == 0x55)	unpackMode = CHECK_END;
		} 

		/* CHECK_END */
		else if (unpackMode == CHECK_END)	// to "UNPACK" or "RSERCH_TO_CHECK"
		{
			ucp_Usart.PopRxdQueue((char*)&tempPack + 1, sizeof(tempPack)-1);
			if (tempPack.PackHead == 0xAA55 && tempPack.PackEnd == 0x00)
				unpackMode = UNPACK;
			else
				unpackMode = RSERCH_TO_CHECK;
		} 

		/* RSERCH_TO_CHECK */
		else if (unpackMode == RSERCH_TO_CHECK)
		{
			uint8_t i;
			/* re-search the head in tempPack */
			for (i = 1; i < sizeof(tempPack); i++)
			{
				if (((uint8_t*)&tempPack)[i] == 0x55) break;
			}
			char* leftPartAddr = ((char*)&tempPack) + sizeof(tempPack)-1 - i;
			uint16_t leftPartSize = sizeof(tempPack)-i;
			if (leftPartSize > 0)
			{
				/* check size */
				if (ucp_Usart.Get_cbInQue() < leftPartSize) break;

				/* check pack end */
				memmove(&tempPack, ((uint8_t*)&tempPack) + i, sizeof(tempPack)-i);
				ucp_Usart.PopRxdQueue(leftPartAddr, leftPartSize);
				if (tempPack.PackHead == 0xAA55 && tempPack.PackEnd == 0x00)
					unpackMode = UNPACK;
				else
					unpackMode = RSERCH_TO_CHECK;
			}
			else //leftPartSize == 0, no pack head in tempPack
				unpackMode = CHECK_SIZE;
			
		}

		/* UNPACK */
		else if (unpackMode == UNPACK) // to "break"
		{
			Registry_[tempPack.DataIdx]->set(tempPack.Data);
			break;
		}
		else std::cout << "unpack enum error" << std::endl;
	}
	return 1;
}
//
// send_Message()
//
uint8_t ucp::send_Message(uint8_t period)
{
	static uint16_t counter = 0;
	if ((++counter) % period != 0)
		return 0;

	if (true) //judge the buff
	{
		TxdBuf_.TimeStamp_H = TimeStamp_ >> 16;
		TxdBuf_.TimeStamp_L = TimeStamp_ & 0xffff;
		TxdBuf_.ErrorCode = 0;

		uint16_t temp;
		TxdBuf_.DataSign = 0x00;
		for (int i = 0; i < 4; i++)
		{
			temp = DataIdx_[i]->get();
			TxdBuf_.DataIdx[i] = (uint8_t)temp;
			TxdBuf_.Data[i] = Registry_[temp]->get();
			TxdBuf_.DataSign |= (Registry_[temp]->get_sign()) << i;
		}
		/*Usart_->DmaSendArray((u8*)&TxdBuf_, sizeof(TxdBuf_));*/
		Usart_->SendArray((uint8_t*)&TxdBuf_, sizeof(TxdBuf_));
	}
	return 1;
}

//
// Get_Usart_()
//
//RSV

ucp::~ucp()
{
}

/******************************************************************************
* @brief   This part for ucp::access
******************************************************************************/
uint16_t ucp::CAccs_base::get() const
{
	return 0x0001;
}
void ucp::CAccs_base::set(uint16_t)
{}
uint8_t ucp::CAccs_base::get_sign() const
{
	return 1;
}
//end of file
