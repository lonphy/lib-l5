#include "hw_crc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CRC_HandleTypeDef hcrc;

/* CRC init function */
void hw_crc_init(void) {

    hcrc.Instance = CRC;
    if (HAL_CRC_Init(&hcrc) != HAL_OK) {
        Error_Handler();
    }

}

void HAL_CRC_MspInit(CRC_HandleTypeDef *crcHandle) {

    if (crcHandle->Instance == CRC) {
        /* USER CODE BEGIN CRC_MspInit 0 */

        /* USER CODE END CRC_MspInit 0 */
        /* CRC clock enable */
        __HAL_RCC_CRC_CLK_ENABLE();
        /* USER CODE BEGIN CRC_MspInit 1 */

        /* USER CODE END CRC_MspInit 1 */
    }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef *crcHandle) {

    if (crcHandle->Instance == CRC) {
        /* USER CODE BEGIN CRC_MspDeInit 0 */

        /* USER CODE END CRC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_CRC_CLK_DISABLE();
        /* USER CODE BEGIN CRC_MspDeInit 1 */

        /* USER CODE END CRC_MspDeInit 1 */
    }
}
