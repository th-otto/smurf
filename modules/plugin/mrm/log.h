extern OBJECT *maintree;
extern WINDOW win;
extern void (*redraw_window)(WINDOW *window, GRECT *mwind, WORD startob, WORD flags);
extern SERVICE_FUNCTIONS *services;

void log_line(short message, short module_number, const char *mod_name);

short start_imp_module(char *modpath, SMURF_PIC *imp_pic);
BASPAG *start_edit_module(char *modpath, BASPAG *edit_basepage, short mode, short mod_id, GARGAMEL *sm_struct);
long get_proclen(BASPAG *baspag);
