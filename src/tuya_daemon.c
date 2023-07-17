#include <sdk/cJSON.h>
#include <sdk/tuya_cacert.h>
#include <sdk/tuya_log.h>
#include <sdk/tuya_error_code.h>
#include <sdk/system_interface.h>
#include <sdk/mqtt_client_interface.h>
#include <sdk/tuyalink_core.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <syslog.h>
#include <signal.h>
#include <sys/sysinfo.h>

#include <utils/become_daemon.h>
#include <utils/argp.h>

int daemonize	   = 1;
volatile sig_atomic_t running = 1;

char *productId	   = "";
char *deviceId	   = "";
char *deviceSecret = "";

tuya_mqtt_context_t client_instance;

void on_connected(tuya_mqtt_context_t *context, void *user_data)
{
	TY_LOGI("on connected");
	if (daemonize) {
		syslog(LOG_USER | LOG_INFO, "on connected");
	}
}

void on_disconnect(tuya_mqtt_context_t *context, void *user_data)
{
	TY_LOGI("on disconnect");
	if (daemonize) {
		syslog(LOG_USER | LOG_INFO, "on disconnect");
	}
}

void on_messages(tuya_mqtt_context_t *context, void *user_data, const tuyalink_message_t *msg)
{
	TY_LOGI("on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
	if (daemonize) {
		syslog(LOG_USER | LOG_INFO, "on message id:%s, type:%d, code:%d", msg->msgid, msg->type,
		       msg->code);
	}
	switch (msg->type) {
	default:
		break;
	}
}

void send_data(tuya_mqtt_context_t *context, void *user_data)
{
	char data[300];
	struct sysinfo info;

	if (sysinfo(&info) != 0) {
		TY_LOGW("sysinfo could not gather system information");
		if (daemonize) {
			syslog(LOG_USER | LOG_WARNING, "sysinfo could not gather system information");
		}
	} else {
		float free_ram	= (info.freeram * 1.0) / 1024 / 1024;
		float total_ram = (info.totalram * 1.0) / 1024 / 1024;

		snprintf(data, sizeof(data), "{\"device_status\": {\"value\": \"Ram usage: %.2fMB/%.2fMB\"}}",
			 free_ram, total_ram);

		tuyalink_thing_property_report(context, deviceId, data);
	}
}

void signal_handler(int sig)
{
	TY_LOGI("signal %d received, stopping", sig);
	if (daemonize) {
		syslog(LOG_USER | LOG_INFO, "signal %d received, stopping", sig);
	}
	running = 0;
}

int main(int argc, char **argv)
{
	// cover the default signal handler
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	// initialize argp
	struct arguments arguments;

	arguments.args[0]     = "";
	arguments.args[1]     = "";
	arguments.args[2]     = "";
	arguments.daemon_flag = 0;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	deviceId     = arguments.args[0];
	deviceSecret = arguments.args[1];
	productId    = arguments.args[2];
	daemonize    = arguments.daemon_flag;

	// return value
	int ret;

	// turn this process into a daemon
	if (daemonize) {
		ret = become_daemon(0);
		if (ret) {
			syslog(LOG_USER | LOG_ERR, "error starting tuya daemon");
			closelog();
			return ret;
		}
		const char *LOGNAME = "TUYA DAEMON";
		openlog(LOGNAME, LOG_PID, LOG_USER);
		syslog(LOG_USER | LOG_INFO, "starting");
	}

	tuya_mqtt_context_t *client = &client_instance;

	// initialize mqtt client
	ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t){ .host		 = "m1.tuyacn.com",
								  .port		 = 8883,
								  .cacert	 = tuya_cacert_pem,
								  .cacert_len	 = sizeof(tuya_cacert_pem),
								  .device_id	 = deviceId,
								  .device_secret = deviceSecret,
								  .keepalive	 = 100,
								  .timeout_ms	 = 2000,
								  .on_connected	 = on_connected,
								  .on_disconnect = on_disconnect,
								  .on_messages	 = on_messages });

	if (ret) {
		TY_LOGE("tuya_mqtt_init failed");
		if (daemonize) {
			syslog(LOG_USER | LOG_ERR, "tuya_mqtt_init failed");
		}
		goto deinit;
	}

	// connect to mqtt server
	ret = tuya_mqtt_connect(client);
	if (ret) {
		TY_LOGE("tuya_mqtt_connect failed");
		if (daemonize) {
			syslog(LOG_USER | LOG_ERR, "tuya_mqtt_connect failed");
		}
		goto disconnect;
	}

	// mqtt loop
	while (running) {
		tuya_mqtt_loop(client);
		send_data(client, NULL);
	}

// cleanup: disconnect and deinit
disconnect:;
	int check;
	check = tuya_mqtt_disconnect(client);
	if (check) {
		TY_LOGE("tuya_mqtt_disconnect failed");
		if (daemonize) {
			syslog(LOG_USER | LOG_ERR, "tuya_mqtt_disconnect failed");
		}
	} else {
		TY_LOGI("tuya_mqtt_disconnect success");
		if (daemonize) {
			syslog(LOG_USER | LOG_INFO, "tuya_mqtt_disconnect success");
		}
	}
deinit:;
	check = tuya_mqtt_deinit(client);
	if (check) {
		TY_LOGE("tuya_mqtt_deinit failed");
		if (daemonize) {
			syslog(LOG_USER | LOG_ERR, "tuya_mqtt_deinit failed");
		}
	} else {
		TY_LOGI("tuya_mqtt_deinit success");
		if (daemonize) {
			syslog(LOG_USER | LOG_INFO, "tuya_mqtt_deinit success");
		}
	}
	if (daemonize) {
		closelog();
	}

	return ret;
}
