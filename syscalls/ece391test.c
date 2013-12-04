#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main()
{
	uint8_t pname [32] = "test";
	ece391_write(1, "testing max tasks\n", 18);
	return ece391_execute(pname);
}
