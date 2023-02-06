<<<<<<< HEAD
#include "main.h"

int main(void){
	HAL_init();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpioinit;

	gpioinit.Pin = GPIO_PIN_0;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gpioinit);

	gpioinit.Pin = GPIO_PIN_1;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gpioinit);

	gpioinit.Pin = GPIO_PIN_2;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gpioinit);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

	for(;;){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_Delay(250);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_Delay(250);
	}
}
=======
#include "main.h"

int main(void){
	HAL_init();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpioinit;

	gpioinit.Pin = GPIO_PIN_0;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gpioinit);

	gpioinit.Pin = GPIO_PIN_1;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gpioinit);

	gpioinit.Pin = GPIO_PIN_2;
	gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gpioinit);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

	for(;;){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_Delay(250);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_Delay(250);
	}
}
>>>>>>> 57cca87149b6a2c58469570993efa5876b986ef6
