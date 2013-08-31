#ifndef C_PLUGINS_H
#define C_PLUGINS_H

#include "plugins/plugins.h"

ProfPlugin* c_plugin_create(const char * const filename);

void c_init_hook(ProfPlugin *plugin, const char * const version, const char * const status);
void c_on_start_hook (ProfPlugin *plugin);
void c_on_connect_hook (ProfPlugin *plugin, const char * const account_name, const char * const fulljid);
char * c_on_message_received_hook(ProfPlugin *plugin, const char * const jid, const char *message);
char * c_on_message_send_hook(ProfPlugin *plugin, const char * const jid, const char *message);
void c_close_library (ProfPlugin * plugin);

#endif