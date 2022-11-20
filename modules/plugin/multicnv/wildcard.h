extern char *wildcards[64];
extern short num_of_wildcards;

char *mystrtok(char *s, char c);
void tokenize_wildcards(char *string);
int check_wildcard(char *filename, char *wildcard);
