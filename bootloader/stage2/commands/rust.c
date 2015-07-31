#include <cli.h>

extern int rust(void);

REGISTER_CLI_COMMAND("rust", "Calling rust", rust);
