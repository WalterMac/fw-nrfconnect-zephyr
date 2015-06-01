/* gatt.h - Generic Attribute Profile handling */

/*
 * Copyright (c) 2015 Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3) Neither the name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __BT_GATT_H
#define __BT_GATT_H

/* GATT attribute permission bitfield values */
#define BT_GATT_PERM_READ			0x01
#define BT_GATT_PERM_WRITE			0x02
#define BT_GATT_PERM_READ_ENCRYPT		0x04
#define BT_GATT_PERM_WRITE_ENCRYPT		0x08
#define BT_GATT_PERM_READ_AUTHEN		0x10
#define BT_GATT_PERM_WRITE_AUTHEN		0x20
#define BT_GATT_PERM_AUTHOR			0x40

/* GATT Attribute structure */
struct bt_gatt_attr {
	uint16_t		handle;
	const struct bt_uuid	*uuid;
	uint8_t			perm;
	int			(*read)(const bt_addr_le_t *peer,
					const struct bt_gatt_attr *attr,
					void *buf, uint8_t len,
					uint16_t offset);
	int			(*write)(const bt_addr_le_t *peer,
					 const struct bt_gatt_attr *attr,
					 const void *buf, uint8_t len,
					 uint16_t offset);
	void			*user_data;
};

/* Service Attribute Value */
struct bt_gatt_service {
	const struct bt_uuid	*uuid;
};

/* Include Attribute Value */
struct bt_gatt_include {
	uint16_t		start_handle;
	uint16_t		end_handle;
	const struct bt_uuid	*uuid;
};

/* Characteristic Properties Bitfield values */
#define BT_GATT_CHRC_BROADCAST			0x01
#define BT_GATT_CHRC_READ			0x02
#define BT_GATT_CHRC_WRITE_WITHOUT_RESP		0x04
#define BT_GATT_CHRC_WRITE			0x08
#define BT_GATT_CHRC_NOTIFY			0x10
#define BT_GATT_CHRC_INDICATE			0x20
#define BT_GATT_CHRC_AUTH			0x40
#define BT_GATT_CHRC_EXT_PROP			0x80

/* Characteristic Attribute Value */
struct bt_gatt_chrc {
	uint8_t			properties;
	uint16_t		value_handle;
	const struct bt_uuid	*uuid;
};

/* Characteristic Extended Properties Bitfield values */
#define BT_GATT_CEP_RELIABLE_WRITE		0x01
#define BT_GATT_CEP_WRITABLE_AUX		0x02

/* Characteristic Extended Properties Attribute Value */
struct bt_gatt_cep {
	uint8_t			properties;
};

/* Characteristic User Description Attribute Value */
struct bt_gatt_cud {
	char			*string;
};

/* Client Characteristic Configuration.user_datas */
#define BT_GATT_CCC_NOTIFY			0x0001
#define BT_GATT_CCC_INDICATE			0x0002

/* Client Characteristic Configuration Attribute Value */
struct bt_gatt_ccc {
	uint16_t		flags;
};

/* Server API */

/** @brief Register attribute database
 *
 *  Register GATT attribute database table. Applications can make use of
 *  macros such as BT_GATT_PRIMARY_SERVICE, BT_GATT_CHARACTERISTIC,
 *  BT_GATT_DESCRIPTOR, etc.
 *
 *  @param attrs dabase table containing the available attributes
 *  @param count size of database table
 */
void bt_gatt_register(const struct bt_gatt_attr *attrs, size_t count);

enum {
	BT_GATT_ITER_STOP = 0,
	BT_GATT_ITER_CONTINUE,
};

/** @brief Attribute iterator callback
 *
 *  @param attr attribute found
 *  @param user_data data given
 *
 *  @return BT_GATT_ITER_CONTINUE if should continue to the next attribute
 *   or BT_GATT_ITER_STOP to stop.
 */
typedef uint8_t (*bt_gatt_attr_func_t)(const struct bt_gatt_attr *attr,
				       void *user_data);

/** @brief Attribute iterator
 *
 *  Iterate attributes in the given range.
 *
 *  @param start_handle uint16_t start handle
 *  @param end_handle uint16_t end handle
 *  @param func bt_gatt_attr_func_t callback function
 *  @param user_data void * data to pass to the callback
 */
void bt_gatt_foreach_attr(uint16_t start_handle, uint16_t end_handle,
			  bt_gatt_attr_func_t func, void *user_data);

/** @brief Generic Read Attribute value helper
 *
 *  Read attribute value storing the result into buffer.
 *
 *  @param peer remote address
 *  @param attr attribute to read
 *  @param buf buffer to store the value
 *  @param buf_len buffer length
 *  @param offset start offset
 *  @param value attribute value
 *  @param value_len length of the attribute value
 *
 *  @return int number of bytes read in case of success or negative values in
 *  case of error.
 */
int bt_gatt_attr_read(const bt_addr_le_t *peer, const struct bt_gatt_attr *attr,
		      void *buf, uint8_t buf_len, uint16_t offset,
		      const void *value, uint8_t value_len);

/** @brief Read Service Attribute helper
 *
 *  Read service attribute value storing the result into buffer after
 *  enconding it.
 *  NOTE: Only use this with attributes which user_data is a bt_uuid.
 *
 *  @param peer remote address
 *  @param attr attribute to read
 *  @param buf buffer to store the value read
 *  @param len buffer length
 *  @param offset start offset
 *
 *  @return int number of bytes read in case of success or negative values in
 *  case of error.
 */
int bt_gatt_attr_read_service(const bt_addr_le_t *peer,
			      const struct bt_gatt_attr *attr,
			      void *buf, uint8_t len, uint16_t offset);

/** @brief Generic Service Declaration Macro
 *
 *  Helper macro to declare a service attribute.
 *
 *  @param _handle service attribute handle
 *  @param _uuid service attribute type
 *  @param _data service attribute value
 */
#define BT_GATT_SERVICE(_handle, _uuid, _service)			\
{									\
	.handle = _handle,						\
	.uuid = _uuid,							\
	.perm = BT_GATT_PERM_READ,					\
	.read = bt_gatt_attr_read_service,				\
	.user_data = _service,						\
}

/** @brief Primary Service Declaration Macro
 *
 *  Helper macro to declare a primary service attribute.
 *
 *  @param _handle service attribute handle
 *  @param _service service attribute value
 */
#define BT_GATT_PRIMARY_SERVICE(_handle, _service)			\
{									\
	.handle = _handle,						\
	.uuid = (&(struct bt_uuid) { .type = BT_UUID_16,		\
				     .u16 = BT_UUID_GATT_PRIMARY }),	\
	.perm = BT_GATT_PERM_READ,					\
	.read = bt_gatt_attr_read_service,				\
	.user_data = _service,						\
}

/** @brief Secondary Service Declaration Macro
 *
 *  Helper macro to declare a secondary service attribute.
 *
 *  @param _handle service attribute handle
 *  @param _service service attribute value
 */
#define BT_GATT_SECONDARY_SERVICE(_handle, _service)			\
{									\
	.handle = _handle,						\
	.uuid = (&(struct bt_uuid) { .type = BT_UUID_16,		\
				     .u16 = BT_UUID_GATT_SECONDARY }),	\
	.perm = BT_GATT_PERM_READ,					\
	.read = bt_gatt_attr_read_service,				\
	.user_data = _service,						\
}

/** @brief Read Include Attribute helper
 *
 *  Read include service attribute value storing the result into buffer after
 *  enconding it.
 *  NOTE: Only use this with attributes which user_data is a bt_gatt_include.
 *
 *  @param peer remote address
 *  @param attr attribute to read
 *  @param buf buffer to store the value read
 *  @param len buffer length
 *  @param offset start offset
 *
 *  @return int number of bytes read in case of success or negative values in
 *  case of error.
 */
int bt_gatt_attr_read_included(const bt_addr_le_t *peer,
			       const struct bt_gatt_attr *attr,
			       void *buf, uint8_t len, uint16_t offset);

/* Include Service Declaration */
#define BT_GATT_INCLUDE_SERVICE(_handle, _service)			\
{									\
	.handle = _handle,						\
	.uuid = (&(struct bt_uuid) { .type = BT_UUID_16,		\
				     .u16 = BT_UUID_GATT_INCLUDE }),	\
	.perm = BT_GATT_PERM_READ,					\
	.read = bt_gatt_attr_read_included,				\
	.user_data = _service,						\
}

/** @brief Read Characteristic Attribute helper
 *
 *  Read characteristic attribute value storing the result into buffer after
 *  enconding it.
 *  NOTE: Only use this with attributes which user_data is a bt_gatt_chrc.
 *
 *  @param peer remote address
 *  @param attr attribute to read
 *  @param buf buffer to store the value read
 *  @param len buffer length
 *  @param offset start offset
 *
 *  @return number of bytes read in case of success or negative values in
 *  case of error.
 */
int bt_gatt_attr_read_chrc(const bt_addr_le_t *peer,
			   const struct bt_gatt_attr *attr, void *buf,
			   uint8_t len, uint16_t offset);

/** @brief Characteristic Declaration Macro
 *
 *  Helper macro to declare a characteristic attribute.
 *
 *  @param _handle characteristic attribute handle
 *  @param _value characteristic attribute value
 */
#define BT_GATT_CHARACTERISTIC(_handle, _value)				\
{									\
	.handle = _handle,						\
	.uuid = (&(struct bt_uuid) { .type = BT_UUID_16,		\
				     .u16 = BT_UUID_GATT_CHRC }),	\
	.perm = BT_GATT_PERM_READ,					\
	.read = bt_gatt_attr_read_chrc,					\
	.user_data = _value,						\
}

struct bt_gatt_ccc_cfg {
	bt_addr_le_t		peer;
	uint16_t		value;
};

struct _bt_gatt_ccc {
	struct bt_gatt_ccc_cfg	*cfg;
	size_t			cfg_len;
	uint16_t		value;
	uint16_t		value_handle;
	void			(*cfg_changed)(uint16_t value);
};

/** @brief Read Client Characteristic Configuration Attribute helper
 *
 *  Read CCC attribute value storing the result into buffer after
 *  enconding it.
 *  NOTE: Only use this with attributes which user_data is a _bt_gatt_ccc.
 *
 *  @param peer remote address
 *  @param attr attribute to read
 *  @param buf buffer to store the value read
 *  @param len buffer length
 *  @param offset start offset
 *
 *  @return number of bytes read in case of success or negative values in
 *  case of error.
 */
int bt_gatt_attr_read_ccc(const bt_addr_le_t *peer,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint8_t len, uint16_t offset);

/** @brief Write Client Characteristic Configuration Attribute helper
 *
 *  Write value in the buffer into CCC attribute.
 *  NOTE: Only use this with attributes which user_data is a _bt_gatt_ccc.
 *
 *  @param peer remote address
 *  @param attr attribute to read
 *  @param buf buffer to store the value read
 *  @param len buffer length
 *  @param offset start offset
 *
 *  @return number of bytes written in case of success or negative values in
 *  case of error.
 */
int bt_gatt_attr_write_ccc(const bt_addr_le_t *peer,
			   const struct bt_gatt_attr *attr, const void *buf,
			   uint8_t len, uint16_t offset);

/** @brief Client Characteristic Configuration Declaration Macro
 *
 *  Helper macro to declare a CCC attribute.
 *
 *  @param _handle descriptor attribute handle
 *  @param _value_handle characteristic attribute value handle
 *  @param _cfg initial configuration
 *  @param _cfg_changed configuration changed callback
 */
#define BT_GATT_CCC(_handle, _value_handle, _cfg, _cfg_changed)		\
{									\
	.handle = _handle,						\
	.uuid = (&(struct bt_uuid) { .type = BT_UUID_16,		\
				     .u16 = BT_UUID_GATT_CCC }),	\
	.perm = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,			\
	.read = bt_gatt_attr_read_ccc,					\
	.write = bt_gatt_attr_write_ccc,				\
	.user_data = (&(struct _bt_gatt_ccc) { .cfg = _cfg,		\
					       .cfg_len = ARRAY_SIZE(_cfg), \
					       .value_handle = _value_handle, \
					       .cfg_changed = _cfg_changed, }),\
}

/** @brief Descriptor Declaration Macro
 *
 *  Helper macro to declare a descriptor attribute.
 *
 *  @param _handle descriptor attribute handle
 *  @param _value descriptor attribute value
 */
#define BT_GATT_DESCRIPTOR(_handle, _uuid, _perm, _read, _write, _value) \
{									\
	.handle = _handle,						\
	.uuid = _uuid,							\
	.perm = _perm,							\
	.read = _read,							\
	.write = _write,						\
	.user_data = _value,						\
}

/** @brief Notify attribute value change
 *
 *  Send notification of attribute value change.
 *  Note: This function should only be called if CCC is declared with
 *  BT_GATT_CCC otherwise it cannot find a valid peer configuration.
 *
 *  @param handle attribute handle
 *  @param value attribute value
 *  @param len attribute value length
 */
void bt_gatt_notify(uint16_t handle, const void *data, size_t len);

#endif /* __BT_GATT_H */
