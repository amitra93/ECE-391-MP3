#ifndef _SYS__CALLS_H
#define _SYS__CALLS_H

extern system_call();
extern int32_t halt (uint8_t status);
extern int32_t execute (const uint8_t* command);
extern int32_t read (int32_t _fd, void* buf, int32_t _nbytes);
extern int32_t write (int32_t _fd, const void* buf, int32_t _nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t _fd);
extern int32_t getargs (uint8_t* buf, int32_t _nbytes);
extern int32_t vidmap (uint8_t** screen_start);
extern int32_t set_handler (int32_t _signum, void* handler_address);
extern int32_t sigreturn (void);

#endif
