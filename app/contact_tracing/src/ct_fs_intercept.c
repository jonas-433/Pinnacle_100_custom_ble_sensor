/**
 * @file ct_fs_intercept.c
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>
LOG_MODULE_REGISTER(ct_fs_intercept, CONFIG_CT_FS_INTERCEPT_LOG_LEVEL);

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <kernel.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "mgmt/mgmt.h"
#include "fs_mgmt/fs_mgmt_config.h"

#include "ble_aws_service.h"
#include "nv.h"
#include "ct_ble.h"
#include "lcz_qrtc.h"

#include "ct_fs_intercept.h"

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
typedef struct smp_to_nv_map {
	const char *const smp_file_path;
	int (*const map_fn)(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
} smp_to_nv_map_t;

#define SMP_TO_NV_MAP_ITEM_COUNT                                               \
	(sizeof(SMP_TO_NV_MAP) / sizeof(smp_to_nv_map_t))

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static char *trim(char *str, int len);
static char *terminate_and_trim(char *str, int len);

static int smp_nv_mapper_aws_topic_prefix(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_ble_network_id(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_mqtt_client_id(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_mqtt_endpoint(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_mqtt_root_ca(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_mqtt_client_cert(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_mqtt_client_key(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_mqtt_save_clear(fs_mgmt_ctxt_t *fs_mgmt_ctxt);
static int smp_nv_mapper_aes_key(fs_mgmt_ctxt_t *fs_mgmt_ctxt);

/******************************************************************************/
/* Local Data Definitions                                                     */
/******************************************************************************/
static const smp_to_nv_map_t SMP_TO_NV_MAP[] = {
	{ .smp_file_path = "/nv/aws_topic_prefix.txt",
	  .map_fn = smp_nv_mapper_aws_topic_prefix },
	{ .smp_file_path = "/nv/ble_network_id.txt",
	  .map_fn = smp_nv_mapper_ble_network_id },
	{ .smp_file_path = "/nv/mqtt/client_id.txt",
	  .map_fn = smp_nv_mapper_mqtt_client_id },
	{ .smp_file_path = "/nv/mqtt/endpoint.txt",
	  .map_fn = smp_nv_mapper_mqtt_endpoint },
	{ .smp_file_path = "/nv/mqtt/root_ca.pem.crt",
	  .map_fn = smp_nv_mapper_mqtt_root_ca },
	{ .smp_file_path = "/nv/mqtt/certificate.pem.crt",
	  .map_fn = smp_nv_mapper_mqtt_client_cert },
	{ .smp_file_path = "/nv/mqtt/private.pem.key",
	  .map_fn = smp_nv_mapper_mqtt_client_key },
	{ .smp_file_path = "/nv/mqtt/save_clear.txt",
	  .map_fn = smp_nv_mapper_mqtt_save_clear },
	{ .smp_file_path = "/nv/aes_key.bin", .map_fn = smp_nv_mapper_aes_key },
};

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
int ct_fs_intercept_nv(char *path, fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	int i;

	LOG_DBG("Receiving '%s' %4d/%d", path,
		(uint32_t)(fs_mgmt_ctxt->off + fs_mgmt_ctxt->data_len),
		(uint32_t)fs_mgmt_ctxt->len);

	for (i = 0; i < SMP_TO_NV_MAP_ITEM_COUNT; i++) {
		if (strcmp(path, SMP_TO_NV_MAP[i].smp_file_path) == 0) {
			return SMP_TO_NV_MAP[i].map_fn(fs_mgmt_ctxt);
		}
	}

	LOG_ERR("SMP File to Parameter mapping not found");
	return MGMT_ERR_EINVAL;
}

int ct_fs_intercept_test_publish(void)
{
	return ct_ble_publish_dummy_data_to_aws();
}

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
/**
 * Utility function to trim text string parameters
 */
static char *trim(char *str, int len)
{
	char *frontp = str;
	char *endp = NULL;

	if (str == NULL) {
		return NULL;
	}
	if (str[0] == '\0') {
		return str;
	}

	endp = str + len;

	/* Move the front and back pointers to address the first non-whitespace
	 * characters from each end.
	 */
	while (isspace((unsigned char)*frontp)) {
		++frontp;
	}
	if (endp != frontp) {
		while (isspace((unsigned char)*(--endp)) && endp != frontp) {
		}
	}

	if (frontp != str && endp == frontp)
		*str = '\0';
	else if (str + len - 1 != endp)
		*(endp + 1) = '\0';

	/* Shift the string so that it starts at str so that if it's dynamically
   * allocated, we can still free it on the returned pointer.  Note the reuse
   * of endp to mean the front of the string buffer now.
   */
	endp = str;
	if (frontp != str) {
		while (*frontp) {
			*endp++ = *frontp++;
		}
		*endp = '\0';
	}

	return str;
}

static char *terminate_and_trim(char *str, int len)
{
	/* first null-terminate the string */
	str[len] = '\0';
	/* trim whitespace if any */
	return trim(str, len);
}

/**
 * Maps SMP upload of AWS_TOPIC_PREFIX to nv param
 */
static int smp_nv_mapper_aws_topic_prefix(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	int rc;

	fs_mgmt_ctxt->uploading = false;
	fs_mgmt_ctxt->off = fs_mgmt_ctxt->data_len;

	if (fs_mgmt_ctxt->data_len > 0 &&
	    fs_mgmt_ctxt->data_len < AWS_TOPIC_PREFIX_MAX_LENGTH &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* null-terminate and remove whitespace */
		terminate_and_trim(fs_mgmt_ctxt->file_data,
				   fs_mgmt_ctxt->data_len);
		rc = nvStoreAwsTopicPrefix(fs_mgmt_ctxt->file_data,
					   fs_mgmt_ctxt->data_len);
		aws_svc_set_topic_prefix((const char *)fs_mgmt_ctxt->file_data);
		if (rc) {
			return MGMT_ERR_EINVAL;
		}
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

/**
 * Maps SMP upload of BLE_NETWORK_ID to nv param
 */
static int smp_nv_mapper_ble_network_id(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	int rc;
	uint16_t nwkId;

	fs_mgmt_ctxt->uploading = false;
	fs_mgmt_ctxt->off = fs_mgmt_ctxt->data_len;

	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* null-terminate and remove whitespace */
		terminate_and_trim(fs_mgmt_ctxt->file_data,
				   fs_mgmt_ctxt->data_len);
		nwkId = strtoul(fs_mgmt_ctxt->file_data, NULL, 16) & 0xFFFF;
		rc = nvStoreBleNetworkId(&nwkId);
		ct_ble_set_network_id(nwkId);
		if (rc) {
			return MGMT_ERR_EINVAL;
		}
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

/**
 * Maps SMP upload of AWS_CLIENT_ID to nv param
 */
static int smp_nv_mapper_mqtt_client_id(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	fs_mgmt_ctxt->uploading = false;
	fs_mgmt_ctxt->off = fs_mgmt_ctxt->data_len;

	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* null-terminate and remove whitespace */
		terminate_and_trim(fs_mgmt_ctxt->file_data,
				   fs_mgmt_ctxt->data_len);
		LOG_DBG(">> client_id: %s", fs_mgmt_ctxt->file_data);
		aws_svc_set_client_id(fs_mgmt_ctxt->file_data);
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

static int smp_nv_mapper_mqtt_endpoint(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	fs_mgmt_ctxt->uploading = false;
	fs_mgmt_ctxt->off = fs_mgmt_ctxt->data_len;

	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* null-terminate and remove whitespace */
		terminate_and_trim(fs_mgmt_ctxt->file_data,
				   fs_mgmt_ctxt->data_len);
		LOG_DBG(">> mqtt_endpoint: %s", fs_mgmt_ctxt->file_data);
		aws_svc_set_endpoint(fs_mgmt_ctxt->file_data);
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

static int smp_nv_mapper_mqtt_root_ca(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* Make sure this file can fit in the root ca value buffer */
		if (fs_mgmt_ctxt->off == 0) {
			if (fs_mgmt_ctxt->len > (AWS_ROOT_CA_MAX_LENGTH + 1)) {
				return MGMT_ERR_ENOMEM;
			}
		}

		/* copy in this chunk to the root ca value buffer */
		aws_svc_set_root_ca_partial(fs_mgmt_ctxt->file_data,
					    fs_mgmt_ctxt->off,
					    fs_mgmt_ctxt->data_len);
		fs_mgmt_ctxt->off += fs_mgmt_ctxt->data_len;
		if (fs_mgmt_ctxt->off >= fs_mgmt_ctxt->len) {
			fs_mgmt_ctxt->uploading = false;
			LOG_DBG("updated root_ca %d bytes", fs_mgmt_ctxt->len);
		}
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

static int smp_nv_mapper_mqtt_client_cert(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* Make sure this file can fit in the client cert value buffer */
		if (fs_mgmt_ctxt->off == 0) {
			if (fs_mgmt_ctxt->len >
			    (AWS_CLIENT_CERT_MAX_LENGTH + 1)) {
				return MGMT_ERR_ENOMEM;
			}
		}

		/* copy in this chunk to the client cert value buffer */
		aws_svc_set_client_cert_partial(fs_mgmt_ctxt->file_data,
						fs_mgmt_ctxt->off,
						fs_mgmt_ctxt->data_len);
		fs_mgmt_ctxt->off += fs_mgmt_ctxt->data_len;
		if (fs_mgmt_ctxt->off >= fs_mgmt_ctxt->len) {
			fs_mgmt_ctxt->uploading = false;
			LOG_DBG("updated client_cert %d bytes",
				fs_mgmt_ctxt->len);
		}
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

static int smp_nv_mapper_mqtt_client_key(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* Make sure this file can fit in the client key value buffer */
		if (fs_mgmt_ctxt->off == 0) {
			if (fs_mgmt_ctxt->len >
			    (AWS_CLIENT_KEY_MAX_LENGTH + 1)) {
				return MGMT_ERR_ENOMEM;
			}
		}

		/* copy in this chunk to the client key value buffer */
		aws_svc_set_client_key_partial(fs_mgmt_ctxt->file_data,
					       fs_mgmt_ctxt->off,
					       fs_mgmt_ctxt->data_len);
		fs_mgmt_ctxt->off += fs_mgmt_ctxt->data_len;
		if (fs_mgmt_ctxt->off >= fs_mgmt_ctxt->len) {
			fs_mgmt_ctxt->uploading = false;
			LOG_DBG("updated client_key %d bytes",
				fs_mgmt_ctxt->len);
		}
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

static int smp_nv_mapper_mqtt_save_clear(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	fs_mgmt_ctxt->uploading = false;
	fs_mgmt_ctxt->off = fs_mgmt_ctxt->data_len;

	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		/* null-terminate and remove whitespace */
		terminate_and_trim(fs_mgmt_ctxt->file_data,
				   fs_mgmt_ctxt->data_len);
		LOG_DBG(">> mqtt_save_clear: %s", fs_mgmt_ctxt->file_data);
		bool save = strtoul(fs_mgmt_ctxt->file_data, NULL, 10);
		aws_svc_save_clear_settings(save);

		if (save) {
			awsSvcEvent(AWS_SVC_EVENT_SETTINGS_SAVED);
		} else {
			awsSvcEvent(AWS_SVC_EVENT_SETTINGS_CLEARED);
		}

	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}

/**
 * Maps SMP upload of BLE_NETWORK_ID to nv param
 */
static int smp_nv_mapper_aes_key(fs_mgmt_ctxt_t *fs_mgmt_ctxt)
{
	int rc;
	uint8_t aesKey[AES_KEY_SIZE];

	fs_mgmt_ctxt->uploading = false;
	fs_mgmt_ctxt->off = fs_mgmt_ctxt->data_len;

	if (fs_mgmt_ctxt->data_len > 0 &&
	    (fs_mgmt_ctxt->data_len + 1) < FS_MGMT_DL_CHUNK_SIZE) {
		if (fs_mgmt_ctxt->data_len >= AES_KEY_SIZE) {
			memcpy(aesKey, fs_mgmt_ctxt->file_data, AES_KEY_SIZE);
			rc = nvStoreAesKey(aesKey);
			if (rc) {
				return MGMT_ERR_EINVAL;
			}
		}
	} else {
		return MGMT_ERR_EINVAL;
	}
	return 0;
}