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
#include <argp.h>
#include <stdatomic.h>
#include <syslog.h>
#include <signal.h>

#include <become_daemon/become_daemon.h>

char *productId	   = "";
char *deviceId	   = "";
char *deviceSecret = "";

atomic_int daemonize = 1;

tuya_mqtt_context_t client_instance;

const char *argp_program_version     = "Developement v0.9";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

static char doc[] =
	"Program to control your IoT products and devices in the cloud, using Tuya IoT Core SDK in C.";

/* A description of the arguments we accept. */
static char args_doc[] = "DeviceID Device_Secret ProductID";

/* Used by main to communicate with parse_opt. */
struct arguments {
	char *args[3]; /* DeviceID, DeviceSecret ProductID */
};

static struct argp_option options[] = { { "daemon_flag", 'd', "[ 1 | 0 ]", 0,
					  "Make the program run as daemon (1 - on, 0 - off, default - 1)" },
					{ 0 } };

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
	case 'd':
		if (strcmp(arg, "0") == 0) {
			daemonize = 0;
		} else if (strcmp(arg, "1") == 0) {
			daemonize = 1;
		} else {
			/* Invalid daemon flag, default fallback */
			argp_usage(state);
		}
		break;

	case ARGP_KEY_ARG:
		if (state->arg_num >= 3)
			/* Too many arguments. */
			argp_usage(state);

		arguments->args[state->arg_num] = arg;

		break;
	case ARGP_KEY_END:
		if (state->arg_num < 3)
			/* Not enough arguments. */
			argp_usage(state);
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

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
	case THING_TYPE_PROPERTY_REPORT_RSP:
		TY_LOGI("property report response:%s", msg->data_string);
		if (daemonize) {
			syslog(LOG_USER | LOG_INFO, "property report response:%s", msg->data_string);
		}
		break;
	default:
		break;
	}
}

void connected(tuya_mqtt_context_t *context, void *user_data)
{
	TY_LOGI("connected");
	if (daemonize) {
		syslog(LOG_USER | LOG_INFO, "connected");
	}

	char data[300];

	snprintf(data, sizeof(data), "{}");

	tuyalink_thing_property_report(context, deviceId, data);
}

void signal_handler(int sig)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		TY_LOGI("signal %d received, exiting...", sig);
		if (daemonize) {
			syslog(LOG_USER | LOG_INFO, "signal %d received, exiting...", sig);
		}
		int ret = tuya_mqtt_disconnect(&client_instance);
		if (ret) {
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
		ret = tuya_mqtt_deinit(&client_instance);
		if (ret) {
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
		closelog();
		exit(EXIT_SUCCESS);
		break;
	default:
		exit(EXIT_FAILURE);
		break;
	}
}

int main(int argc, char **argv)
{
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	struct arguments arguments;

	arguments.args[0] = "";
	arguments.args[1] = "";
	arguments.args[2] = "";

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	deviceId     = arguments.args[0];
	deviceSecret = arguments.args[1];
	productId    = arguments.args[2];

	int ret;

	// turn this process into a daemon
	if (daemonize) {
		ret = become_daemon(0);
		if (ret) {
			syslog(LOG_USER | LOG_ERR, "error starting tuya daemon");
			closelog();
			return EXIT_FAILURE;
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
		syslog(LOG_USER | LOG_ERR, "tuya_mqtt_init failed");
		goto deinit;
	}

	// connect to mqtt server
	ret = tuya_mqtt_connect(client);
	if (ret) {
		TY_LOGE("tuya_mqtt_connect failed");
		syslog(LOG_USER | LOG_ERR, "tuya_mqtt_connect failed");
		goto disconnect;
	}

	// mqtt loop
	while (true) {
		tuya_mqtt_loop(client);
		connected(client, NULL);
	}

disconnect:
	ret = tuya_mqtt_disconnect(client);
	if (ret) {
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
deinit:
	ret = tuya_mqtt_deinit(client);
	if (ret) {
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

	return EXIT_SUCCESS;
}