/**
  ******************************************************************************
  * @file    CUcp.cpp
  * @author  Chenxx
  * @version V2.1
  * @date    2016-05-14
  * @brief   This file achieve the uart calibration protocol
  ******************************************************************************/
#include "ucp.h"
#include "CSysTick.h"

/******************************************************************************
* @brief   This part for CUcp
******************************************************************************/
//
// static variable
//
uint8_t CUcp::RxdBuf_[sizeof(CUcp::RxdPkgTyp)];
CUcp::TxdPkgTyp CUcp::TxdBuf_ = {0x5AA5};
list<CUcp::RxdPkgTyp> CUcp::RxdQue_;

CUcp::CAccs_base* CUcp::Registry_[clv::VAR_NUM] = {(CUcp::CAccs_base*)0xaa55};
//CUcp::CAccs_base** CUcp::DataIdx_ = Registry_;

//
//local
//
CUcp::CAccs_base accs_empty;
CAccs_dataIdx accs_dataIdx[4];

/**
  * @brief  Constructor
  * @param  None
  * @retval None
  */
CUcp::CUcp()
	:send_period_(3),DataIdx_(Registry_)
{
	for(int i = 0; i < 4; i++)
	{
		DataIdx_[i] = &(accs_dataIdx[i]);
	}
	for(int i = 4; i < clv::VAR_NUM; i++)
	{
		if(Registry_[i] == NULL)
			Registry_[i] = &accs_empty;
	}
	InitSciGpio();
	InitSci();
}

/**
  * @brief  Initializes the UART of ucp
  * @param  None
  * @retval None
  */
void CUcp::InitSci()
{
	/* init clock of USART */
	#if  UCP_USE_UART1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	#elif  UCP_USE_UART2 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	#elif  UCP_USE_UART3  
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	#else 
		#error
	#endif
	
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(UCP_UART, &USART_InitStructure); 

	USART_Cmd(UCP_UART, ENABLE); 
	
	USART_DMACmd(UCP_UART, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(UCP_UART, USART_DMAReq_Rx, ENABLE);
	
	/* DMA Clock Config */
	uint32_t RCC_AHB1Periph;
	if(UCP_DMA == DMA1) RCC_AHB1Periph = RCC_AHBPeriph_DMA1;
	else if(UCP_DMA == DMA2) RCC_AHB1Periph = RCC_AHBPeriph_DMA2;
	else while(1); //error
	RCC_AHBPeriphClockCmd(RCC_AHB1Periph, ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	/* TX DMA Config */	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UCP_UART->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)"a";
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 0;
	
	DMA_Cmd(UCP_TX_DMACH, DISABLE);
	DMA_Init(UCP_TX_DMACH, &DMA_InitStructure);
	
	/* RX DMA Config */	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UCP_UART->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxdBuf_;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = sizeof(RxdPkgTyp);
	
	DMA_Cmd(UCP_RX_DMACH, DISABLE);
	DMA_Init(UCP_RX_DMACH, &DMA_InitStructure);
	
	/* Init DMA rxd INTE */
	NVIC_InitTypeDef NVIC_InitStructure;
													
	NVIC_InitStructure.NVIC_IRQChannel = UCP_RX_DMA_IRQ;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(UCP_RX_DMACH, DMA_IT_TC, ENABLE);
	
	DMA_Cmd(UCP_RX_DMACH, ENABLE);
}

/**
  * @brief  Initializes the GPIO of ucp UART
  * @param  None
  * @retval None
  */
void CUcp::InitSciGpio()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	uint32_t RCC_APB2Periph_GPIOx;
	uint8_t GPIO_PinSource_BASE;
//	uint8_t GPIO_AF_USARTx;
	GPIO_TypeDef *GPIOx;
	
	#if UCP_IOGROUP_A2
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
		GPIOx = GPIOA;
		GPIO_PinSource_BASE = GPIO_PinSource2;
	#elif UCP_IOGROUP_A9
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
		GPIOx = GPIOA;
		GPIO_PinSource_BASE = GPIO_PinSource9;
	#elif UCP_IOGROUP_B10
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
		GPIOx = GPIOB;
		GPIO_PinSource_BASE = GPIO_PinSource10;
	#else 
		#error
	#endif

	/* open clock of gpio */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
	
	/* Config Pin: RXD TXD */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 << GPIO_PinSource_BASE; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOx, &GPIO_InitStructure);	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 << GPIO_PinSource_BASE; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	GPIO_Init(GPIOx, &GPIO_InitStructure);
	
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/**
  * @brief  Updates ucp at each time step
  * @param  None
  * @retval None
  */
void CUcp::update()
{
	TimeStamp_++;
	send_Message();
	pop_RxdQue_();
}

/**
  * @brief  Work when new message received.
	* 				Push message into RxdQue_.
  * @param  None
  * @retval None
  */
#define MAX_RXQUE_SIZE 3
void CUcp::OnMessageReceive()
{
	if(RxdQue_.size() <= MAX_RXQUE_SIZE)
	{	
		RxdQue_.push_back(*((RxdPkgTyp*)RxdBuf_));
	}
}

/**
  * @brief  Pop messages from RxdQue_.
	* 				Then unpack the messages.
  * @param  None
  * @retval Whether more than one messages poped.
  */
uint8_t CUcp::pop_RxdQue_()
{
	if(RxdQue_.size() == 0)
		return 0;
	list<RxdPkgTyp>::Iterator it;
	it = RxdQue_.front();
	if(it->elem_.PackHead == 0xAA55 && it->elem_.PackEnd == 0x00)
	{
		Registry_[it->elem_.DataIdx]->set(it->elem_.Data);
	}
	RxdQue_.pop_front();
		return 1;
}

/**
  * @brief  Pack data and send message with DMA
  * @param  None
  * @retval Whether a message was sent.
  */
uint8_t CUcp::send_Message()
{
	static u16 counter = 0;
	if((++counter)%send_period_ != 0)
		return 0;
		
	if(UCP_TX_DMACH->CNDTR == 0)
	{
		TxdBuf_.TimeStamp_H = TimeStamp_>>16;
		TxdBuf_.TimeStamp_L = TimeStamp_&0xffff;
		TxdBuf_.ErrorCode = 0;
		
		uint16_t temp;
		TxdBuf_.DataSign = 0x00;
		for(int i = 0; i < 4; i++)
		{
			temp = DataIdx_[i]->get();
			TxdBuf_.DataIdx[i] = temp;	
			TxdBuf_.Data[i] = Registry_[temp]->get();
			TxdBuf_.DataSign |= (Registry_[temp]->get_sign())<<i;
		}
		sendArray((u8*)&TxdBuf_, sizeof(TxdBuf_));
	}
	return 1;
}

/**
  * @brief  Change the ucp broadcase frequence
	* @param  freq:	Frequence for message sending
  * @retval None
  */
void CUcp::set_sendFreq(uint16_t freq)
{
	if(freq >= 500 || freq == 0) return;
	send_period_ = CSysTick::get_Freq()/freq;
}

/**
  * @brief  Get the ucp broadcase frequence
	* @param  None
  * @retval freq:	Frequence for message sending
  */
uint16_t CUcp::get_sendFreq() const
{
	return CSysTick::get_Freq()/send_period_;
}

/**
  * @brief  Reboots the UART rxd DMA
	* @param  None
  * @retval None
  */
void CUcp::reboot_rxDMA()
{
	DMA_Cmd(UCP_RX_DMACH, DISABLE);//data left in buff will be cut
	UCP_RX_DMACH->CNDTR = sizeof(RxdPkgTyp);
	DMA_Cmd(UCP_RX_DMACH, ENABLE);
}

/**
  * @brief  Reboots the UART rxd DMA
	* @param  buf: The address of target array
	* @param  size: bytes that need to be sent
  * @retval None
  */
void CUcp::sendArray(uint8_t* buf, uint8_t size)
{
	DMA_Cmd(UCP_TX_DMACH, DISABLE);//data left in buff will be cut
	UCP_TX_DMACH->CNDTR = size;
	UCP_TX_DMACH->CMAR = (uint32_t)buf;
	
	DMA_Cmd(UCP_TX_DMACH, ENABLE);
}
/******************************************************************************
* @brief   This part for CUcp::access
******************************************************************************/
uint16_t CUcp::CAccs_base::get() const
{
	return 0x0001;
}
void CUcp::CAccs_base::set(uint16_t)
{}
uint8_t CUcp::CAccs_base::get_sign() const
{
	return 1;
}
//end of file
