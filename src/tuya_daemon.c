#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <argp.h>
#include <become_daemon/become_daemon.h>

#include <sdk/cJSON.h>
#include <sdk/tuya_cacert.h>
#include <sdk/tuya_log.h>
#include <sdk/tuya_error_code.h>
#include <sdk/system_interface.h>
#include <sdk/mqtt_client_interface.h>
#include <sdk/tuyalink_core.h>

char *productId	   = "";
char *deviceId	   = "";
char *deviceSecret = "";

const char *argp_program_version     = "Developement v0.9";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

static char doc[] = "Daemon program, that uses Tuya IoT Core SDK in C to connect and control your IoT products and devices in the cloud.";

/* A description of the arguments we accept. */
static char args_doc[] = "DeviceID Device_Secret ProductID";

/* Used by main to communicate with parse_opt. */
struct arguments {
	char *args[3]; /* DeviceID, DeviceSecret & ProductID */
};

static struct argp_option options[] = { { 0 } };

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
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

tuya_mqtt_context_t client_instance;

void on_connected(tuya_mqtt_context_t *context, void *user_data)
{
	TY_LOGI("on connected");

	char data[300];

	snprintf(data, sizeof(data), "{}");
	
	tuyalink_thing_property_report(context, deviceId, data);
}

void on_disconnect(tuya_mqtt_context_t *context, void *user_data)
{
	TY_LOGI("on disconnect");
}

void on_messages(tuya_mqtt_context_t *context, void *user_data, const tuyalink_message_t *msg)
{
	TY_LOGI("on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
	switch (msg->type) {
	case THING_TYPE_MODEL_RSP:
		TY_LOGI("model response:%s", msg->data_string);
		break;

	case THING_TYPE_PROPERTY_REPORT_RSP:
		TY_LOGI("property report response:%s", msg->data_string);
		break;

	case THING_TYPE_PROPERTY_SET_RSP:
		TY_LOGI("property set response:%s", msg->data_string);
		break;

	case THING_TYPE_PROPERTY_DESIRED_GET_RSP:
		TY_LOGI("property desired get response:%s", msg->data_string);
		break;

	case THING_TYPE_PROPERTY_DESIRED_DEL_RSP:
		TY_LOGI("property desired del response:%s", msg->data_string);
		break;

	case THING_TYPE_EVENT_TRIGGER_RSP:
		TY_LOGI("event trigger response:%s", msg->data_string);
		break;

	case THING_TYPE_ACTION_EXECUTE_RSP:
		TY_LOGI("action execute response:%s", msg->data_string);
		break;

	case THING_TYPE_BATCH_REPORT_RSP:
		TY_LOGI("batch report response:%s", msg->data_string);
		break;

	case THING_TYPE_DEVICE_SUB_BIND_RSP:
		TY_LOGI("device sub bind response:%s", msg->data_string);
		break;

	case THING_TYPE_DEVICE_TOPO_ADD_RSP:
		TY_LOGI("device topo add response:%s", msg->data_string);
		break;

	case THING_TYPE_DEVICE_TOPO_DEL_RSP:
		TY_LOGI("device topo del response:%s", msg->data_string);
		break;

	case THING_TYPE_DEVICE_TOPO_GET_RSP:
		TY_LOGI("device topo get response:%s", msg->data_string);
		break;

	case THING_TYPE_OTA_FIRMWARE_REPORT:
		TY_LOGI("ota firmware report:%s", msg->data_string);
		break;

	case THING_TYPE_OTA_GET_RSP:
		TY_LOGI("ota get response:%s", msg->data_string);
		break;

	case THING_TYPE_EXT_CONFIG_GET_RSP:
		TY_LOGI("ext config get response:%s", msg->data_string);
		break;

	case THING_TYPE_OTA_PROGRESS_REPORT:
		TY_LOGI("ota progress report:%s", msg->data_string);
		break;

	case THING_TYPE_EXT_TIME_RESPONSE:
		TY_LOGI("ext time response:%s", msg->data_string);
		break;

	case THING_TYPE_EXT_FILE_UPLOAD_RESPONSE:
		TY_LOGI("ext file upload response:%s", msg->data_string);
		break;

	case THING_TYPE_EXT_FILE_DOWNLOAD_RESPONSE:
		TY_LOGI("ext file download response:%s", msg->data_string);
		break;

	case THING_TYPE_CHANNEL_RPC_RESPONSE:
		TY_LOGI("channel rpc response:%s", msg->data_string);
		break;

	case THING_TYPE_DEVICE_TAG_REPORT_RESPONSE:
		TY_LOGI("device tag report response:%s", msg->data_string);
		break;

	case THING_TYPE_DEVICE_TAG_GET_RESPONSE:
		TY_LOGI("device tag get response:%s", msg->data_string);
		break;

	case THING_TYPE_DEVICE_TAG_DELETE_RESPONSE:
		TY_LOGI("device tag delete response:%s", msg->data_string);
		break;

	default:
		break;
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	struct arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	deviceId     = arguments.args[0];
	deviceSecret = arguments.args[1];
	productId    = arguments.args[2];
	
	int ret;

	// turn this process into a daemon
	ret = become_daemon(0);
	if (ret) {
		return EXIT_FAILURE;
	}
	
	FILE *fl;
	fl = popen("logger", "w");
	dup2(fileno(fl), STDOUT_FILENO);
	dup2(fileno(fl), STDERR_FILENO);

	// we are now a daemon!
	// stderr and stdout redirected to syslog

	tuya_mqtt_context_t *client = &client_instance;

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
	assert(ret == OPRT_OK);
	
	ret = tuya_mqtt_connect(client);
	assert(ret == OPRT_OK);

	for (;;) {
		tuya_mqtt_loop(client);
	}

	pclose(fl);
	return EXIT_SUCCESS;
}
