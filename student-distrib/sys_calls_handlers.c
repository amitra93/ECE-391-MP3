#include "types.h"

int32_t _halt (uint8_t status) { return 0; }
int32_t _execute (const uint8_t* command) { return 0; }
int32_t _read (int32_t _fd, void* buf, int32_t _nbytes) { return 0; }
int32_t _write (int32_t _fd, const void* buf, int32_t _nbytes) { return 0; }
int32_t _open (const uint8_t* filename) { return 0; }
int32_t _close (int32_t _fd) { return 0; }
int32_t _getargs (uint8_t* buf, int32_t _nbytes) { return 0; }
int32_t _vidmap (uint8_t** screen_start) { return 0; }
int32_t _set_handler (int32_t _signum, void* handler_address) { return 0; }
int32_t _sigreturn (void) { return 0; }
