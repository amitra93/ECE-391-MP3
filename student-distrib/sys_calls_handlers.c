#include "types.h"

int32_t do_halt (uint8_t status) { return 0; }
int32_t do_execute (const uint8_t* command) { return 0; }
int32_t do_read (int32_t _fd, void* buf, int32_t _nbytes) { return 0; }
int32_t do_write (int32_t _fd, const void* buf, int32_t _nbytes) { return 0; }
int32_t do_open (const uint8_t* filename) { return 0; }
int32_t do_close (int32_t _fd) { return 0; }
int32_t do_getargs (uint8_t* buf, int32_t _nbytes) { return 0; }
int32_t do_vidmap (uint8_t** screen_start) { return 0; }
int32_t do_set_handler (int32_t _signum, void* handler_address) { return 0; }
int32_t do_sigreturn (void) { return 0; }
