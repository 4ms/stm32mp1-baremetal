#pragma once

#include "norflash/qspi_flash_driver.hh"
#include "usbd_dfu.h"

extern USBD_DFU_MediaTypeDef USBD_DFU_MEDIA_fops;
void dfu_set_qspi_driver(mdrivlib::QSpiFlash *flash_driver);
