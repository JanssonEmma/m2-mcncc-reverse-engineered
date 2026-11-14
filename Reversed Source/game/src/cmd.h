#ifndef MCNCC_GAME_CMD_H
#define MCNCC_GAME_CMD_H
class CHARACTER;
void interpret_command(CHARACTER* character, const char* argument);
void interpreter_set_privilege(const char* command, int privilege);
const char* GetCommandName(int index);
void SendLog(const char* text);
#endif
