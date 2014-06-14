#include "debugger.h"

#include <stdlib.h>
#include <string.h>

debugger_list_t *gDebuggerList = 0;

int debugger_list_commands(struct debugger_state *state, int argc, char **argv) {
	if (argc != 1) {
		state->print(state,
			"list_commands - List all registered commands\nThis command takes no arguments.\n");
		return 0;
	}

	int i = 0;
	for (i = 0; i < gDebuggerList->count; i++) {
		state->print(state, "%d. %s\n", i, gDebuggerList->commands[i]->name);
	}
	return 0;
}

debugger_command_t list_command = {
	"list_commands", debugger_list_commands
};

void init_debugger() {
	gDebuggerList = malloc(sizeof(debugger_list_t));
	gDebuggerList->count = 1;
	gDebuggerList->capacity = 10;
	gDebuggerList->commands = malloc(sizeof(debugger_command_t *) * 10);

	gDebuggerList->commands[0] = &list_command;
}

int compare_strings(const char *a, const char *b) {
	int i = 0;
	while (*a != 0 && *b != 0 && *(a++) == *(b++)) {
		i++;
	}
	return i;
}

int find_best_command(const char *f_command, debugger_command_t ** pointer) {
	if (gDebuggerList == 0) {
		return 0;
	}

	int command_length = strlen(f_command);

	int i;
	int max_match = 0;
	int match_numbers = 0;
	debugger_command_t *best_command = 0;

	for (i = 0; i < gDebuggerList->count; i++) {
		debugger_command_t *cmd = gDebuggerList->commands[i];
		int match = compare_strings(f_command, cmd->name);
		if (match > max_match && command_length == match) {
			max_match = match;
			match_numbers = 1;
			best_command = cmd;
		} else if (match > 0 && match == max_match) {
			match_numbers++;
		}
	}

	*pointer = best_command;
	if (max_match && match_numbers == 1) {
		return 1;
	}

	if (!max_match) {
		return 0;
	}

	return match_numbers > 1 ? -1 : 0;
}

void register_command(const char *name, debugger_function_t function, void *state) {
	if (gDebuggerList == 0) {
		init_debugger();
	}

	debugger_command_t *command = malloc(sizeof(debugger_command_t));
	command->name = name;
	command->function = function;
	command->state = state;

	if (gDebuggerList->count >= gDebuggerList->capacity) {
		gDebuggerList->commands = realloc(gDebuggerList->commands,
			sizeof(debugger_command_t *) * (gDebuggerList->capacity + 10));
		gDebuggerList->capacity += 10;
	}

	gDebuggerList->count++;
	gDebuggerList->commands[gDebuggerList->count - 1] = command;
}
