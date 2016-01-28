/**
 * Initialize CAN Controller
 */
bool can_init(uint8_t lp_enable){
	GPIO_InitTypeDef GPIO_InitDef;
	CAN_InitTypeDef CAN_InitStruct;
    CAN_FilterInitTypeDef CAN_FilterInitStruct;
    uint8_t result;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE );
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB  | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitDef.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
	GPIO_InitDef.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_InitDef.GPIO_OType= GPIO_OType_PP;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitDef);

	GPIO_SetBits(GPIOE,GPIO_Pin_9 | GPIO_Pin_11); ///< Standby Control of CAN transceiver chip
	/*
	 * If you want to enable the test code below that initializes the CAN pins as
	 * GPIOs, then comments out the call to can_write() in main.c and uncomment
	 * the call to can_toggle() in sensors.c.
	 */
#if 0
	GPIO_InitTypeDef GPIO_InitType;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitType.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitType.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_InitType.GPIO_OType= GPIO_OType_PP;
	GPIO_InitType.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitType.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitType);

	GPIO_SetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_13); ///< RGB LEDs are wired opposite.
#endif
//#if 0
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2);

	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	//GPIO_InitDef.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitDef.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_13; //Select PB12 and PB13 for CAN 2;
	GPIO_Init(GPIOB, &GPIO_InitDef);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE );

    ///< De-initialize to default settings
	CAN_DeInit(CAN1);
	CAN_DeInit(CAN2);

	CAN_InitStruct.CAN_TTCM = DISABLE;
	CAN_InitStruct.CAN_ABOM = DISABLE;
	CAN_InitStruct.CAN_AWUM = DISABLE;
	CAN_InitStruct.CAN_NART = DISABLE;
	CAN_InitStruct.CAN_RFLM = DISABLE;
	CAN_InitStruct.CAN_TXFP = DISABLE;
	if(lp_enable == 1)
	{
		CAN_InitStruct.CAN_Mode = CAN_Mode_LoopBack; ///<
	}
	else
	{
		CAN_InitStruct.CAN_Mode = CAN_Mode_Normal; ///<
	}

	///< Setting the baud rate
	RCC_ClocksTypeDef     RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	//CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
	//CAN_InitStruct.CAN_BS1 = CAN_BS1_6tq;
	//CAN_InitStruct.CAN_BS2 = CAN_BS2_7tq;
	CAN_InitStruct.CAN_SJW = CAN_SJW_3tq;
	CAN_InitStruct.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitStruct.CAN_BS2 = CAN_BS2_2tq;
	//CAN_InitStruct.CAN_Prescaler = 3; // Baud rate 1 Mbps
	printf("PCLK1 = %i\r\n", RCC_Clocks.PCLK1_Frequency);
	//CAN_InitStruct.CAN_Prescaler = RCC_Clocks.PCLK1_Frequency / (14 * 125000); // Baud rate 125 Kbps
	CAN_InitStruct.CAN_Prescaler = ((RCC_Clocks.PCLK1_Frequency/7) / 500000) - 1; //
	printf("CAN_InitStruct.CAN_Prescaler = %i\r\n", CAN_InitStruct.CAN_Prescaler);

	vTaskDelay(500);

	CAN_Init(CAN1, &CAN_InitStruct);

	CAN_Init(CAN2, &CAN_InitStruct);

    CAN_FilterInitStruct.CAN_FilterNumber = 0; //
    CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;

    CAN_FilterInit(&CAN_FilterInitStruct);

    CAN_FilterInitStruct.CAN_FilterNumber = 14; //
    CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_16bit;
    CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;

    CAN_FilterInit(&CAN_FilterInitStruct);
//#endif
	return true;
}



//#if 0
bool can_read(can_message* receivedMessage){
	CanRxMsg RxMessage;
	uint32_t state = 0;
	uint8_t messagePending = CAN_MessagePending(CAN1, CAN_FIFO0);
//	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

    state = CAN1->MSR;
    printf("MSR = 0x%x\r\n", state);

    state = CAN1->ESR;
    printf("ESR = 0x%x\r\n", state);

	while (messagePending > 0){
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		receivedMessage->stdid = (uint16_t)(0xFF & RxMessage.StdId); //message id

		for (int8_t i = 7; i>=0; i-=1){
			receivedMessage->payload |= RxMessage.Data[i]; //fill in uint8_t payload to 8 byte can_message* buffer
			if(i > 0)
			{
				receivedMessage->payload <<= 8;
			}
		}
		CAN_FIFORelease (CAN1, CAN_FIFO0);
		messagePending -= 1;

		if(messagePending == 0){
			return true;
		}
	}
	return false;
}