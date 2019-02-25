/*
 * Copyright Runtime.io 2018. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 * @brief Bluetooth transport for the mcumgr SMP protocol.
 */

#include <logging/log>
LOG_MODULE_REGISTER(smp_sock_tcp, CONFIG_SMP_SOCKET_LOG_LEVEL);
#include <errno.h>

#include <zephyr.h>
#include <init.h>

#include <mgmt/smp_bt.h>
#include <mgmt/buf.h>
#include <mgmt/smp.h>

#include <net/socket.h>

struct device;
/*
struct smp_bt_user_data {
	struct bt_conn *conn;
};*/

static struct zephyr_smp_transport socket_smp_transport;


/**
 * Write handler for the SMP characteristic; processes an incoming SMP request.
 */
static ssize_t smp_bt_chr_write(struct bt_conn *conn,
				const struct bt_gatt_attr *attr,
				const void *buf, u16_t len, u16_t offset,
				u8_t flags)
{
	struct smp_bt_user_data *ud;
	struct net_buf *nb;

	nb = mcumgr_buf_alloc();
	net_buf_add_mem(nb, buf, len);

	ud = net_buf_user_data(nb);
	ud->conn = bt_conn_ref(conn);

	zephyr_smp_rx_req(&smp_bt_transport, nb);
	

	return len;
}

static int smp_tcp_write(struct socket * sock, const u8_t *data, u32_t data_len)
{
	struct net_buf * net_buffer = mcumgr_buf_alloc();
	net_buf_add_mem(net_buffer, data, data_len);
	user_data = net_buf_user_data(net_buffer);
	zephyr_smp_rx_req(&socket_smp_transport, net_buffer);
	return len;
}


static void smp_bt_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
}

static struct bt_gatt_ccc_cfg smp_bt_ccc[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_attr smp_bt_attrs[] = {
	/* SMP Primary Service Declaration */
	BT_GATT_PRIMARY_SERVICE(&smp_bt_svc_uuid),

	BT_GATT_CHARACTERISTIC(&smp_bt_chr_uuid.uuid,
			       BT_GATT_CHRC_WRITE_WITHOUT_RESP |
			       BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_WRITE,
			       NULL, smp_bt_chr_write, NULL),
	BT_GATT_CCC(smp_bt_ccc, smp_bt_ccc_changed),
};

static struct bt_gatt_service smp_bt_svc = BT_GATT_SERVICE(smp_bt_attrs);

/**
 * Transmits an SMP response over the specified Bluetooth connection.
 */
static int smp_bt_tx_rsp(struct bt_conn *conn, const void *data, u16_t len)
{
	return bt_gatt_notify(conn, smp_bt_attrs + 2, data, len);
}

/**
 * Extracts the Bluetooth connection from a net_buf's user data.
 */
static struct bt_conn *smp_bt_conn_from_pkt(const struct net_buf *nb)
{
	struct smp_bt_user_data *ud = net_buf_user_data(nb);

	if (!ud->conn) {
		return NULL;
	}

	return bt_conn_ref(ud->conn);
}

/**
 * Calculates the maximum fragment size to use when sending the specified
 * response packet.
 */
static u16_t smp_bt_get_mtu(const struct net_buf *nb)
{
	struct bt_conn *conn;
	u16_t mtu;

	conn = smp_bt_conn_from_pkt(nb);
	if (conn == NULL) {
		return 0;
	}

	mtu = bt_gatt_get_mtu(conn);
	bt_conn_unref(conn);

	/* Account for the three-byte notification header. */
	return mtu - 3;
}

static void smp_bt_ud_free(void *ud)
{
	struct smp_bt_user_data *user_data = ud;

	if (user_data->conn) {
		bt_conn_unref(user_data->conn);
		user_data->conn = NULL;
	}
}

static int smp_bt_ud_copy(struct net_buf *dst, const struct net_buf *src)
{
	struct smp_bt_user_data *src_ud = net_buf_user_data(src);
	struct smp_bt_user_data *dst_ud = net_buf_user_data(dst);

	if (src_ud->conn) {
		dst_ud->conn = bt_conn_ref(src_ud->conn);
	}

	return 0;
}

/**
 * Transmits the specified SMP response.
 */
static int smp_bt_tx_pkt(struct zephyr_smp_transport *zst, struct net_buf *nb)
{
	struct bt_conn *conn;
	int rc;

	conn = smp_bt_conn_from_pkt(nb);
	if (conn == NULL) {
		rc = -1;
	} else {
		rc = smp_bt_tx_rsp(conn, nb->data, nb->len);
		bt_conn_unref(conn);
	}

	smp_bt_ud_free(net_buf_user_data(nb));
	mcumgr_buf_free(nb);

	return rc;
}


int smp_socket_connect(struct dfu_client *client)
{
	int ret;
	const struct sockaddr *server = client->server;
	client.tcp_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);	
}	

static int smp_bt_init(struct device *dev)
{
	ARG_UNUSED(dev);

	zephyr_smp_transport_init(&smp_bt_transport, smp_bt_tx_pkt,
				  smp_bt_get_mtu, smp_bt_ud_copy,
				  smp_bt_ud_free);
	return 0;
}

SYS_INIT(smp_bt_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
