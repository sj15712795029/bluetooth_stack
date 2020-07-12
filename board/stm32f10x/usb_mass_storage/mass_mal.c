/**
  ******************************************************************************
  * @file    mass_mal.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Medium Access Layer interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "mass_mal.h"
#include "hw_spi_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Mass_Memory_Size[MAX_LUN+1];
uint32_t Mass_Block_Size[MAX_LUN+1];
uint32_t Mass_Block_Count[MAX_LUN+1];
__IO uint32_t Status = 0;



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : MAL_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Init(uint8_t lun)
{
    uint16_t status = MAL_OK;

    switch (lun)
    {
    case 0:
        break;
    default:
        return MAL_FAIL;
    }
    return status;
}
/*******************************************************************************
* Function Name  : MAL_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{

    switch (lun)
    {
    case 0:
        hw_spi_flash_write((uint8_t *)Writebuff,Memory_Offset,Transfer_Length);
        break;
    default:
        return MAL_FAIL;
    }
    return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{

    switch (lun)
    {
    case 0:
        hw_spi_flash_read((uint8_t *)Readbuff,Memory_Offset,Transfer_Length);
        break;
    default:
        return MAL_FAIL;
    }
    return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_GetStatus
* Description    : Get status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_GetStatus (uint8_t lun)
{
    if (lun == 0)
    {
        Mass_Block_Size[0]  = hw_spi_flash_get_sector_size();
        Mass_Block_Count[0] = hw_spi_flash_get_sector_count();
        Mass_Memory_Size[0] =Mass_Block_Size[0]*Mass_Block_Count[0];
    }

    return MAL_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

