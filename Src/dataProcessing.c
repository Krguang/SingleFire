#include "dataProcessing.h"
#include "usart.h"

uint8_t key1Flag = 0;
uint8_t key2Flag = 0;
uint8_t key3Flag = 0;

uint8_t key1Temp = 0;
uint8_t key2Temp = 0;
uint8_t key3Temp = 0;

uint8_t key1Read = 0;
uint8_t key2Read = 0;
uint8_t key3Read = 0;

uint8_t key1InFlag = 0;
uint8_t key2InFlag = 0;
uint8_t key3InFlag = 0;

uint8_t temp1[3] = { 0xff,0xff,0xff };
uint8_t temp0[3] = { 0,0,0 };
uint8_t backAddress[3] = { 0,0,0 };

uint8_t usart_tx_temp[256];
uint8_t usart3_tx_temp[128];
uint8_t usart3_tx_temp2[128];

void cmdProcessing(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma, unsigned char *buffer, unsigned int length) {

	if (buffer[0] == 0x24 && buffer[1] == 0xff && buffer[2] == 0xff)
	{
		if (length == 6)
		{
			if (buffer[3] == 1)
			{
				HAL_GPIO_WritePin(m0_GPIO_Port, m0_Pin, GPIO_PIN_SET);
			}
			else if (buffer[3] == 0)
			{
				HAL_GPIO_WritePin(m0_GPIO_Port, m0_Pin, GPIO_PIN_RESET);
			}

			if (buffer[4] == 1)
			{
				HAL_GPIO_WritePin(m1_GPIO_Port, m1_Pin, GPIO_PIN_SET);
			}
			else if (buffer[4] == 0)
			{
				HAL_GPIO_WritePin(m1_GPIO_Port, m1_Pin, GPIO_PIN_RESET);
			}

			if (buffer[5] == 1)
			{
				HAL_GPIO_WritePin(m2_GPIO_Port, m2_Pin, GPIO_PIN_SET);
			}
			else if (buffer[5] == 0)
			{
				HAL_GPIO_WritePin(m2_GPIO_Port, m2_Pin, GPIO_PIN_RESET);
			}

			dma_send(huart, hdma, temp1, 3);
		}
		else
		{
			dma_send(huart, hdma, temp0, 3);
		}
	}
	else if (buffer[0] == 0x25 && buffer[1] == 0xff && buffer[2] == 0xff)
	{
		
		for (uint8_t i = 0; i < 128 - 3; i++)
		{
			usart3_tx_temp[i] = buffer[i + 3];
		}
		dma_send(&huart3, &hdma_usart3_tx, usart3_tx_temp, length - 3);
	}
	else
	{
		if (huart == &huart1)
		{
	
			usart_tx_temp[0] = backAddress[0];
			usart_tx_temp[1] = backAddress[1];
			usart_tx_temp[2] = backAddress[2];

			for (uint8_t i = 0; i < usart1_tx_len; i++)
			{
				usart_tx_temp[i + 3] = usart1_tx_buffer[i];
			}

			dma_send(&huart3, &hdma_usart3_tx, usart_tx_temp, usart1_tx_len + 3);
			
		}

		if (huart == &huart3) {
			
			backAddress[0] = buffer[0];
			backAddress[1] = buffer[1];
			backAddress[2] = buffer[2];

			for (size_t i = 0; i < length-3; i++)
			{
				usart3_tx_temp2[i] = buffer[i + 3];
			}
			uint8_t a[5] = { 0,0,0,0,0 };

			dma_send(&huart1, &hdma_usart1_tx, usart3_tx_temp2, length-3);
		
			if (length == 6)
			{
				if (usart3_tx_buffer[0] == 1)
				{
					key1Flag = !key1Flag;

					if (key1Flag)
					{
						HAL_GPIO_WritePin(key1_set_GPIO_Port, key1_set_Pin, GPIO_PIN_SET);
						HAL_Delay(100);
						HAL_GPIO_WritePin(key1_set_GPIO_Port, key1_set_Pin, GPIO_PIN_RESET);
					}
					else
					{
						HAL_GPIO_WritePin(key1_rset_GPIO_Port, key1_rset_Pin, GPIO_PIN_SET);
						HAL_Delay(100);
						HAL_GPIO_WritePin(key1_rset_GPIO_Port, key1_rset_Pin, GPIO_PIN_RESET);
					}
				}

				if (usart3_tx_buffer[1] == 1)
				{
					key2Flag = !key2Flag;

					if (key2Flag)
					{
						HAL_GPIO_WritePin(key2_set_GPIO_Port, key2_set_Pin, GPIO_PIN_SET);
						HAL_Delay(100);
						HAL_GPIO_WritePin(key2_set_GPIO_Port, key2_set_Pin, GPIO_PIN_RESET);
					}
					else
					{
						HAL_GPIO_WritePin(key2_rset_GPIO_Port, key2_rset_Pin, GPIO_PIN_SET);
						HAL_Delay(100);
						HAL_GPIO_WritePin(key2_rset_GPIO_Port, key2_rset_Pin, GPIO_PIN_RESET);
					}
				}

				if (usart3_tx_buffer[2] == 1)
				{
					key3Flag = !key3Flag;

					if (key2Flag)
					{
						HAL_GPIO_WritePin(key2_set_GPIO_Port, key2_set_Pin, GPIO_PIN_SET);
						HAL_Delay(100);
						HAL_GPIO_WritePin(key2_set_GPIO_Port, key2_set_Pin, GPIO_PIN_RESET);
					}
					else
					{
						HAL_GPIO_WritePin(key2_rset_GPIO_Port, key2_rset_Pin, GPIO_PIN_SET);
						HAL_Delay(100);
						HAL_GPIO_WritePin(key2_rset_GPIO_Port, key2_rset_Pin, GPIO_PIN_RESET);
					}

					if (key3Flag)
					{
						HAL_GPIO_WritePin(key3_set_GPIO_Port, key3_set_Pin, GPIO_PIN_SET);
						HAL_Delay(100);
						HAL_GPIO_WritePin(key3_set_GPIO_Port, key3_set_Pin, GPIO_PIN_RESET);
					}
				}
			}
		}
	}
}


void dataProcessing() {

	if (usart1_rx_flag == 1)
	{
		cmdProcessing(&huart1, &hdma_usart1_tx, usart1_tx_buffer, usart1_tx_len);
		usart1_rx_flag = 0;
	}

	if (usart3_rx_flag == 1)
	{
		cmdProcessing(&huart3, &hdma_usart3_tx, usart3_tx_buffer, usart3_tx_len);
		usart3_rx_flag = 0;
	}

	key1Read = HAL_GPIO_ReadPin(in1_GPIO_Port, in1_Pin);
	if (key1Read == key1Temp)
	{
		key1Temp = !key1Temp;
		key1InFlag = !key1InFlag;
		if (key1InFlag)
		{
			HAL_GPIO_WritePin(key1_set_GPIO_Port, key1_set_Pin, GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(key1_set_GPIO_Port, key1_set_Pin, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(key1_rset_GPIO_Port, key1_rset_Pin, GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(key1_rset_GPIO_Port, key1_rset_Pin, GPIO_PIN_RESET);
		}
	}

	key2Read = HAL_GPIO_ReadPin(in2_GPIO_Port, in2_Pin);
	if (key2Read == key2Temp)
	{
		key2Temp = !key2Temp;
		key2InFlag = !key2InFlag;
		if (key2InFlag)
		{
			HAL_GPIO_WritePin(key2_set_GPIO_Port, key2_set_Pin, GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(key2_set_GPIO_Port, key2_set_Pin, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(key2_rset_GPIO_Port, key2_rset_Pin, GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(key2_rset_GPIO_Port, key2_rset_Pin, GPIO_PIN_RESET);
		}
	}

	key3Read = HAL_GPIO_ReadPin(in3_GPIO_Port, in3_Pin);
	if (key3Read == key3Temp)
	{
		key3Temp = !key3Temp;
		key3InFlag = !key3InFlag;
		if (key3InFlag)
		{
			HAL_GPIO_WritePin(key3_set_GPIO_Port, key3_set_Pin, GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(key3_set_GPIO_Port, key3_set_Pin, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(key3_rest_GPIO_Port, key3_rest_Pin, GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(key3_rest_GPIO_Port, key3_rest_Pin, GPIO_PIN_RESET);
		}
	}
}