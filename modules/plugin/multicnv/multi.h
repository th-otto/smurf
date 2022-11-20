extern SERVICE_FUNCTIONS *services;
extern PLUGIN_FUNCTIONS *smurf_functions;
extern SMURF_VARIABLES *smurf_vars;
extern WINDOW window;
extern OBJECT *dialog;
extern char **alerts;
extern MOD_ABILITY expmabs;
extern EXPORT_CONFIG *exp_conf;
extern short mod_num;
extern WORD exp_depth;
extern char *export_path;

extern GARGAMEL **smurf_struct;
extern BASPAG **edit_bp;

extern EXPORT_CONFIG original_exp_conf;
extern EXPORT_CONFIG my_exp_conf;

extern WORD fix_red[256];
extern WORD fix_green[256];
extern WORD fix_blue[256];

void start_conversion(void);
