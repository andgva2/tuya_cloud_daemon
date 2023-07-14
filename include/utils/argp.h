#include <argp.h>
#include <string.h>
#define ARG_COUNT 3

const char *argp_program_version     = "Developement v0.9";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

static char doc[] =
	"Program to control your IoT products and devices in the cloud, using Tuya IoT Core SDK in C.";

static char args_doc[] = "DeviceID Device_Secret ProductID";

static struct argp_option options[] = { { "daemon_flag", 'd', "[ 1 | 0 ]", 0,
					  "Make the program run as daemon (1 - on, 0 - off, default - 1)" },
					{ 0 } };

/* Used by main to communicate with parse_opt. */
struct arguments {
	char *args[ARG_COUNT]; /* DeviceID, DeviceSecret ProductID */
	int daemon_flag;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
	case 'd':
		if (strcmp(arg, "0") == 0) {
			arguments->daemon_flag = 0;
		} else if (strcmp(arg, "1") == 0) {
			arguments->daemon_flag = 1;
		} else {
			/* Invalid daemon flag, default fallback */
			argp_usage(state);
		}
		break;

	case ARGP_KEY_ARG:
		if (state->arg_num >= ARG_COUNT)
			/* Too many arguments. */
			argp_usage(state);

		arguments->args[state->arg_num] = arg;

		break;
	case ARGP_KEY_END:
		if (state->arg_num < ARG_COUNT)
			/* Not enough arguments. */
			argp_usage(state);
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

static error_t parse_opt(int key, char *arg, struct argp_state *state);