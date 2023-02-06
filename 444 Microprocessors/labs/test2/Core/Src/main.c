<<<<<<< HEAD
////pushbuttons
//
//#include "main.h"
//
//int main(void) {
//	GPIO_InitTypeDef gpioinit;
//	uint8_t pbState=0, pbNewState=0;
//	uint8_t ledIdx=0;
//	uint8_t tmp=0;
//
//	HAL_Init();
//	gpioinit.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
//	gpioinit.Mode =  GPIO_MODE_OUTPUT_PP;
//	gpioinit.Speed = GPIO_SPEED_FREQ_LOW;
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	HAL_GPIO_Init(GPIOA, &gpioinit);
//
////	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2,GPIO_PIN_SET);
//
//	gpioinit.Pin = GPIO_PIN_3;
//	gpioinit.Mode = GPIO_MODE_INPUT;
//	gpioinit.Pull = GPIO_NOPULL;
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	HAL_GPIO_Init(GPIOA, &gpioinit);
//
//	for(;;){
//		pbNewState = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET)?1:0;
//		if(pbState != pbNewState){
//
//		} else{
//			ledIdx=(ledIdx+1)%3;
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2,GPIO_PIN_RESET);
//		}
//		pbState = pbNewState;
//		if(HAL_GetTick()%250==0){
//			switch(ledIdx){
//			case 0:
//				tmp = GPIO_PIN_0;
//				break;
//			case 1:
//				tmp = GPIO_PIN_1;
//				break;
//			case 2:
//				tmp = GPIO_PIN_2;
//				break;
//			default:
//				break;
//			}
//			HAL_GPIO_TogglePin(GPIOA,tmp);
//		}
//		HAL_Delay(1);
//	}
//}

//clock
#include "main.h"
void System_Clock();
int main(void){
	GPIO_InitTypeDef gpioinit;
	HAL_Init();
	System_Clock();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	gpioinit.Pin = GPIO_PIN_0;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpioinit);
	uint32_t n;
	while(1){
		n = 1000000;
		do {
			__NOP();
		}
		while(n--);
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_0);
	}
}
void System_Clock(){
	RCC_OscInitTypeDef oscinit;
	RCC_ClkInitTypeDef clkinit;
	oscinit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscinit.HSEState = RCC_HSE_ON;
	oscinit.PLL.PLLState = RCC_PLL_ON;
	oscinit.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscinit.PLL.PLLN = 9;
	HAL_RCC_OscConfig(&oscinit);

	clkinit.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK;
	clkinit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinit.AHBCLKDivider =  RCC_SYSCLK_DIV1;
	HAL_RCC_ClockConfig(&clkinit,FLASH_LATENCY_2);
}
=======
////pushbuttons
//
//#include "main.h"
//
//int main(void) {
//	GPIO_InitTypeDef gpioinit;
//	uint8_t pbState=0, pbNewState=0;
//	uint8_t ledIdx=0;
//	uint8_t tmp=0;
//
//	HAL_Init();
//	gpioinit.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
//	gpioinit.Mode =  GPIO_MODE_OUTPUT_PP;
//	gpioinit.Speed = GPIO_SPEED_FREQ_LOW;
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	HAL_GPIO_Init(GPIOA, &gpioinit);
//
////	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2,GPIO_PIN_SET);
//
//	gpioinit.Pin = GPIO_PIN_3;
//	gpioinit.Mode = GPIO_MODE_INPUT;
//	gpioinit.Pull = GPIO_NOPULL;
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	HAL_GPIO_Init(GPIOA, &gpioinit);
//
//	for(;;){
//		pbNewState = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET)?1:0;
//		if(pbState != pbNewState){
//
//		} else{
//			ledIdx=(ledIdx+1)%3;
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2,GPIO_PIN_RESET);
//		}
//		pbState = pbNewState;
//		if(HAL_GetTick()%250==0){
//			switch(ledIdx){
//			case 0:
//				tmp = GPIO_PIN_0;
//				break;
//			case 1:
//				tmp = GPIO_PIN_1;
//				break;
//			case 2:
//				tmp = GPIO_PIN_2;
//				break;
//			default:
//				break;
//			}
//			HAL_GPIO_TogglePin(GPIOA,tmp);
//		}
//		HAL_Delay(1);
//	}
//}

//clock
#include "main.h"
void System_Clock();
int main(void){
	GPIO_InitTypeDef gpioinit;
	HAL_Init();
	System_Clock();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	gpioinit.Pin = GPIO_PIN_0;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpioinit);
	uint32_t n;
	while(1){
		n = 1000000;
		do {
			__NOP();
		}
		while(n--);
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_0);
	}
}
void System_Clock(){
	RCC_OscInitTypeDef oscinit;
	RCC_ClkInitTypeDef clkinit;
	oscinit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscinit.HSEState = RCC_HSE_ON;
	oscinit.PLL.PLLState = RCC_PLL_ON;
	oscinit.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscinit.PLL.PLLN = 9;
	HAL_RCC_OscConfig(&oscinit);

	clkinit.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK;
	clkinit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinit.AHBCLKDivider =  RCC_SYSCLK_DIV1;
	HAL_RCC_ClockConfig(&clkinit,FLASH_LATENCY_2);
}
>>>>>>> 57cca87149b6a2c58469570993efa5876b986ef6
