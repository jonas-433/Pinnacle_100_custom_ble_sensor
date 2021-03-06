/**
 * @file nv.h
 * @brief Non-volatile storage
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __NV_H__
#define __NV_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/
#define NV_FLASH_DEVICE DT_CHOSEN_ZEPHYR_FLASH_CONTROLLER_LABEL

#define NUM_FLASH_SECTORS 4

#define AES_KEY_SIZE 16
#define AES_CBC_IV_SIZE 16
#define AES_CBC_BLK_SIZE 16
#define AES_BLANK_KEY_BYTE_VALUE 0xFF

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
int nvInit(void);
int nvReadCommissioned(bool *commissioned);
int nvStoreCommissioned(bool commissioned);
int nvStoreDevCert(uint8_t *cert, uint16_t size);
int nvStoreDevKey(uint8_t *key, uint16_t size);
int nvReadDevCert(uint8_t *cert, uint16_t size);
int nvReadDevKey(uint8_t *key, uint16_t size);
int nvDeleteDevCert(void);
int nvDeleteDevKey(void);
int nvStoreAwsEndpoint(uint8_t *ep, uint16_t size);
int nvReadAwsEndpoint(uint8_t *ep, uint16_t size);
int nvDeleteAwsEndpoint(void);
int nvStoreAwsClientId(uint8_t *id, uint16_t size);
int nvReadAwsClientId(uint8_t *id, uint16_t size);
int nvDeleteAwsClientId(void);
int nvStoreAwsRootCa(uint8_t *cert, uint16_t size);
int nvReadAwsRootCa(uint8_t *cert, uint16_t size);
int nvDeleteAwsRootCa(void);
int nvInitLwm2mConfig(void *data, void *init_value, uint16_t size);
int nvWriteLwm2mConfig(void *data, uint16_t size);

#ifdef CONFIG_BOARD_MG100
int nvReadBatteryLow(uint16_t *batteryData);
int nvReadBatteryAlarm(uint16_t *batteryData);
int nvReadBattery4(uint16_t *batteryData);
int nvReadBattery3(uint16_t *batteryData);
int nvReadBattery2(uint16_t *batteryData);
int nvReadBattery1(uint16_t *batteryData);
int nvReadBattery0(uint16_t *batteryData);
int nvStoreBatteryLow(uint16_t *batteryData);
int nvStoreBatteryAlarm(uint16_t *batteryData);
int nvStoreBattery4(uint16_t *batteryData);
int nvStoreBattery3(uint16_t *batteryData);
int nvStoreBattery2(uint16_t *batteryData);
int nvStoreBattery1(uint16_t *batteryData);
int nvStoreBattery0(uint16_t *batteryData);
int nvStoreAccelODR(int Value);
int nvStoreAccelThresh(int Value);
int nvStoreAccelScale(int Value);
int nvStoreSDLogMaxSize(int Value);
int nvReadAccelODR(int *Value);
int nvReadAccelThresh(int *Value);
int nvReadAccelScale(int *Value);
int nvReadSDLogMaxSize(int *Value);
#endif /* CONFIG_BOARD_MG100 */

#ifdef CONFIG_APP_AWS_CUSTOMIZATION
int nvStoreAwsEnableCustom(bool Value);
int nvReadAwsEnableCustom(bool *Value);
#endif

#ifdef CONFIG_CONTACT_TRACING
int nvStoreAwsTopicPrefix(uint8_t *prefix, uint16_t size);
int nvReadAwsTopicPrefix(uint8_t *prefix, uint16_t size);
int nvStoreBleNetworkId(uint16_t *networkId);
int nvReadBleNetworkId(uint16_t *networkId);
int nvStoreAesKey(uint8_t *key);
int nvReadAesKey(uint8_t *key);
int nvDeleteAesKey(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __NV_H__ */
