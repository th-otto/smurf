#define MAX_PLUGINS 10

extern BASPAG *plugin_bp[MAX_PLUGINS + 1];
extern PLUGIN_DATA *plg_data[MAX_PLUGINS + 1];
extern PLUGIN_INFO *plg_info[MAX_PLUGINS + 1];
extern char *plugin_paths[MAX_PLUGINS + 1];
extern EXT_MODCONF *modconfs[20];			/* Strukturen fÅr Modul-Notifying */
extern short anzahl_plugins;
extern signed char menu2plugin[128];		/* feste MenÅeintrÑge, in die Plugins eingehÑngt sind */
extern BOOLEAN printplug_found;

/* Funktionsprototypen */
short start_plugin(BASPAG *bp, short message, short plg_id, PLUGIN_DATA *data);
void call_plugin(WORD menuentry);
void scan_plugins(void);
void terminate_plugin(short index);

