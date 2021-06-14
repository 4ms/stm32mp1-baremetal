#include "hal_callback.hh"

static void HALCallbackHandler(HALCallbackID cbnum)
{
	HALCallbackManager::callHALCB(cbnum);
}

extern "C" void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	HALCallbackHandler(HALCallbackID::I2C_MemTxCplt);
}

extern "C" void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
	HALCallbackHandler(HALCallbackID::SAI_TxCplt);
}
extern "C" void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
	HALCallbackHandler(HALCallbackID::SAI_TxHalfCplt);
}

// Todo: allow for registration of mulitple periphs per HAL callback:
//
// I2C_HandleTypeDef *registered_hi2c[kMaxObjectsPerHALCB];
// SAI_HandleTypeDef *registered_hsai[kMaxObjectsPerHALCB];
//
// template<typename T>
// void call_handler(HALCallbackID cbid, T registry[], T thisobj) {
//
// }
// //or:
// void call_handler(HALCallbackID cbid, void **registry, void *thisobj) {
//
// }
//
// auto halcb_id = get_cb_int(HALCallbackID::I2C_MemTxCplt);
// for (uint8_t i=0; i<kMaxObjectsPerHALCB; i++) {
// 		if (registered_hi2c[i] == hi2c) {
// 			func_id = halcb_id * kMaxObjectsPerHALCB + i;
//			HALCallbackHandler(func_id);
// 		}
// 	}
//
