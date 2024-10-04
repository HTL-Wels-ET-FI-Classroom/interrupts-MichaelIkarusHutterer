/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
enum tstates {
	timer1, timer2
} state;

enum colors {
	RED,
	BLUE,
	GREEN,
	YELLOW,
	WHITE
}color;
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState(int *xCoord, int *yCoord);

void EXTI0_IRQHandler(void) {

	if (state == timer1) {
		state = timer2;
	} else {
		state = timer1;

	}
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
}

void EXTI4_IRQHandler(void) {
	swtich(color){

	}
	LCD_SetTextColor(LCD_COLOR_GREEN);

	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
}
/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
	HAL_IncTick();
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();
	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");
	// printf Alternative
	LCD_SetPrintPosition(2, 0);
	printf("   EXTI Interrupt");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "copyright xyz", CENTER_MODE);

	static int cnt = 0;
	static int cnt2 = 0;

	GPIO_InitTypeDef taster;
	taster.Mode = GPIO_MODE_IT_RISING;
	taster.Alternate = 0;
	taster.Pin = GPIO_PIN_0;
	taster.Pull = GPIO_NOPULL;
	taster.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOA, &taster);

	taster.Pull = GPIO_PULLDOWN;
	taster.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB, &taster);


	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);




	LCD_SetTextColor(LCD_COLOR_BLUE);
	state=timer1;
	/* Infinite loop */
	while (1) {
		//execute main loop every 100ms
		HAL_Delay(100);

		// display timer

		switch (state) {
		case timer1:
			cnt++;
			LCD_SetFont(&Font20);

			LCD_SetPrintPosition(5, 0);
			printf("   Timer: %.1f", cnt / 10.0);
			break;
		case timer2:
			cnt2++;
			LCD_SetFont(&Font20);

			LCD_SetPrintPosition(7, 0);
			printf("   Timer: %.1f", cnt2 / 10.0);
			break;
		default:
			printf("error no sir");
			break;
		}

	}
	// test touch interface
	int x, y;
	if (GetTouchState(&x, &y)) {
		LCD_FillCircle(x, y, 5);
	}

}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState(int *xCoord, int *yCoord) {
	void BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}

