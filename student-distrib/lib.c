/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab
 */

#include "paging.h"
#include "lib.h"
#include "task.h"
#include "sched.h"
#include "terminal.h"

#define NUM_COLS 80
#define NUM_ROWS 25
#define ATTRIB 0x7
#define VGA_PORT 0x03D4
#define CURSOR_HI_INDEX 0x0E
#define CURSOR_LOW_INDEX 0x0F

static int screen_x;
static int screen_y;
static char* video_mem = (char *)VIDEO;

/*
*  print_error(char * description, uint32_t error_code, uint32_t instr_ptr, uint32_t pid)
*  Description: Prints error code onto screen
*  Inputs: Description=Error description, error_code=processor error_code, instr_ptr = error_instruction
		   pid = process id
*  Outputs: None
*  Side Effects: Clears screen, writes error onto screen
*/
void print_error(char * description, uint32_t error_code, uint32_t instr_ptr, uint32_t pid)
{
	int32_t str_len, x, y, starty, index;
	uint32_t cr2;
	pid = get_cur_task()->pid;
	//asm volatile("movl %%cr3, %0":"=r"(pd));			
	pd = get_cur_task()->page_directory;
	asm volatile("movl %%cr2, %0":"=r"(cr2));
	
	// Get length of description
	str_len = strlen(description);
	
	//Clear the screen
	clear();
	
	//Find the center of the screen
	x = 0;
	y = 0;
	
	//Set the cursor to the middle of the screen
	set_cursor_pos(x, y);
	
	//Print the error description
	printf("Error       : %s", description);
	
	//Center, and increment line
	y ++;
	
	//Set cursor position, print error code, increment line
	set_cursor_pos(x, y++);
	printf("Error Code  : 0x%#x", error_code);
	
	//Set cursor position, print errored instruction, increment line
	set_cursor_pos(x, y++);
	printf("Instruction : 0x%#x", instr_ptr);
	
	//Set cursor position, print errored process, increment line
	set_cursor_pos(x, y++);
	printf("Process ID  : 0x%#x", pid);

	set_cursor_pos(x, y++);
	printf("Process Name: %s", get_cur_task()->pName);
	
	set_cursor_pos(x, y++);
	printf("Violated Mem: 0x%#x 0x%#x", pd, cr2);
	
	set_cursor_pos(x, y++);
	printf("Violated PDE: 0x%#x", pd[cr2>>22]);
	
	starty = y;
	index = 0;
	for (x = 1; x < NUM_COLS; x += 16)
	for (y = starty; y < NUM_ROWS; y ++)
		{
			if (index >= 90)
				break;
			set_cursor_pos(x, y);
			printf("%d:0x%#x", index, pd[index]);
			++index;
		}
	
}

void
clear(void)
{
    int32_t i;
    for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }
}

void
clear_line(unsigned char * video_memory, int32_t rownum){
	if (rownum < 0 || rownum >= NUM_ROWS){
		return;
	}
	int i;
	unsigned char* temp = video_memory + NUM_COLS * rownum * 2;
	for(i = 0; i < NUM_COLS; i++) {
        *(uint8_t *)(temp + (i << 1)) = ' ';
        *(uint8_t *)(temp + (i << 1) + 1) = ATTRIB;
    }
}

void scroll_up(unsigned char * video_memory){
	memmove(video_memory, video_memory + NUM_COLS*2, NUM_COLS * (NUM_ROWS-1) * 2);
	clear_line(video_memory, NUM_ROWS-1);
}


/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 * */
int32_t
printf(int8_t *format, ...)
{
	/* Pointer to the format string */
	int8_t* buf = format;

	/* Stack pointer for the other parameters */
	int32_t* esp = (void *)&format;
	esp++;

	while(*buf != '\0') {
		switch(*buf) {
			case '%':
				{
					int32_t alternate = 0;
					buf++;

format_char_switch:
					/* Conversion specifiers */
					switch(*buf) {
						/* Print a literal '%' character */
						case '%':
							putc('%');
							break;

						/* Use alternate formatting */
						case '#':
							alternate = 1;
							buf++;
							/* Yes, I know gotos are bad.  This is the
							 * most elegant and general way to do this,
							 * IMHO. */
							goto format_char_switch;

						/* Print a number in hexadecimal form */
						case 'x':
							{
								int8_t conv_buf[64];
								if(alternate == 0) {
									itoa(*((uint32_t *)esp), conv_buf, 16);
									puts(conv_buf);
								} else {
									int32_t starting_index;
									int32_t i;
									itoa(*((uint32_t *)esp), &conv_buf[8], 16);
									i = starting_index = strlen(&conv_buf[8]);
									while(i < 8) {
										conv_buf[i] = '0';
										i++;
									}
									puts(&conv_buf[starting_index]);
								}
								esp++;
							}
							break;

						/* Print a number in unsigned int form */
						case 'u':
							{
								int8_t conv_buf[36];
								itoa(*((uint32_t *)esp), conv_buf, 10);
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a number in signed int form */
						case 'd':
							{
								int8_t conv_buf[36];
								int32_t value = *((int32_t *)esp);
								if(value < 0) {
									conv_buf[0] = '-';
									itoa(-value, &conv_buf[1], 10);
								} else {
									itoa(value, conv_buf, 10);
								}
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a single character */
						case 'c':
							putc( (uint8_t) *((int32_t *)esp) );
							esp++;
							break;

						/* Print a NULL-terminated string */
						case 's':
							puts( *((int8_t **)esp) );
							esp++;
							break;

						default:
							break;
					}

				}
				break;

			default:
				putc(*buf);
				break;
		}
		buf++;
	}

	return (buf - format);
}

/* Printf that writes to a terminal.
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 * */
int32_t
vprintf(terminal * term, int8_t *format, ...)
{
	/* Pointer to the format string */
	int8_t* buf = format;

	/* Stack pointer for the other parameters */
	int32_t* esp = (void *)&format;
	esp++;

	while(*buf != '\0') {
		switch(*buf) {
			case '%':
				{
					int32_t alternate = 0;
					buf++;

format_char_switch:
					/* Conversion specifiers */
					switch(*buf) {
						/* Print a literal '%' character */
						case '%':
							vputc(term, '%');
							break;

						/* Use alternate formatting */
						case '#':
							alternate = 1;
							buf++;
							/* Yes, I know gotos are bad.  This is the
							 * most elegant and general way to do this,
							 * IMHO. */
							goto format_char_switch;

						/* Print a number in hexadecimal form */
						case 'x':
							{
								int8_t conv_buf[64];
								if(alternate == 0) {
									itoa(*((uint32_t *)esp), conv_buf, 16);
									vputs(term,conv_buf);
								} else {
									int32_t starting_index;
									int32_t i;
									itoa(*((uint32_t *)esp), &conv_buf[8], 16);
									i = starting_index = strlen(&conv_buf[8]);
									while(i < 8) {
										conv_buf[i] = '0';
										i++;
									}
									vputs(term,&conv_buf[starting_index]);
								}
								esp++;
							}
							break;

						/* Print a number in unsigned int form */
						case 'u':
							{
								int8_t conv_buf[36];
								itoa(*((uint32_t *)esp), conv_buf, 10);
								vputs(term,conv_buf);
								esp++;
							}
							break;

						/* Print a number in signed int form */
						case 'd':
							{
								int8_t conv_buf[36];
								int32_t value = *((int32_t *)esp);
								if(value < 0) {
									conv_buf[0] = '-';
									itoa(-value, &conv_buf[1], 10);
								} else {
									itoa(value, conv_buf, 10);
								}
								vputs(term,conv_buf);
								esp++;
							}
							break;

						/* Print a single character */
						case 'c':
							vputc(term, (uint8_t) *((int32_t *)esp) );
							esp++;
							break;

						/* Print a NULL-terminated string */
						case 's':
							vputs(term, *((int8_t **)esp) );
							esp++;
							break;

						default:
							break;
					}

				}
				break;

			default:
				vputc(term, *buf);
				break;
		}
		buf++;
	}

	return (buf - format);
}

/* Output a string to the console */
int32_t
puts(int8_t* s)
{
	register int32_t index = 0;
	while(s[index] != '\0') {
		putc(s[index]);
		index++;
	}

	return index;
}

/* Output a string to the terminal */
int32_t
vputs(terminal * term, int8_t* s)
{
	register int32_t index = 0;
	while(s[index] != '\0') {
		vputc(term, s[index]);
		index++;
	}

	return index;
}

void
putc(uint8_t c)
{
    if(c == '\n' || c == '\r') {
        screen_y++;
        screen_x=0;

        if (screen_y >= NUM_ROWS){ //we are on last line
      		scroll_up((unsigned char *)video_mem);
      		screen_y--;
    	}
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y + screen_x) << 1) + 1) = ATTRIB;
        screen_x++;
        screen_x %= NUM_COLS;
        screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
    }
}

/* Output a character to the terminal*/
void
vputc(terminal * term, uint8_t c)
{
    if(c == '\n' || c == '\r') {
        term->screen_y++;
        term->screen_x=0;

        if (term->screen_y >= NUM_ROWS){ //we are on last line
      		scroll_up((unsigned char *)term->video_buffer);
      		term->screen_y--;
    	}
    } else {
        *(uint8_t *)(term->video_buffer + ((NUM_COLS*term->screen_y + term->screen_x) << 1)) = c;
        *(uint8_t *)(term->video_buffer + ((NUM_COLS*term->screen_y + term->screen_x) << 1) + 1) = ATTRIB;
        term->screen_x++;
        term->screen_x %= NUM_COLS;
        term->screen_y = (term->screen_y + (term->screen_x / NUM_COLS)) % NUM_ROWS;
    }
}

void get_cursor_pos(int32_t* x, int32_t* y){
	if (x == NULL || y == NULL){
		return;
	}
	*x = screen_x;
	*y = screen_y;
}

void
set_cursor_pos(int32_t x, int32_t y)
{
	if (x >= 0 && x < NUM_COLS)
		screen_x = x;
	if (y >= 0 && y < NUM_ROWS)
		screen_y = y;
}

void 
set_blinking_cursor_pos(int32_t x, int32_t y){
	//position of cursor is based on where it is in contiguous video memory
	//in order to write the blinking cursor to vga memrory, it must be split into
	//2 16 bit chunks
	if (x == 0){
		y++;
	}
	int32_t fullNumber = (y * NUM_COLS) + x;
	int16_t lowEight = fullNumber & 0xFF;
	int16_t highEight = (fullNumber >> 8)  & 0xFF;
	int16_t toOutput = (highEight << 8) + CURSOR_HI_INDEX;
	outw(toOutput,VGA_PORT);
	toOutput = (lowEight << 8) + CURSOR_LOW_INDEX;
	outw(toOutput, VGA_PORT);
	return;
}


/* Convert a number to its ASCII representation, with base "radix" */
int8_t*
itoa(uint32_t value, int8_t* buf, int32_t radix)
{
	static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	int8_t *newbuf = buf;
	int32_t i;
	uint32_t newval = value;

	/* Special case for zero */
	if(value == 0) {
		buf[0]='0';
		buf[1]='\0';
		return buf;
	}

	/* Go through the number one place value at a time, and add the
	 * correct digit to "newbuf".  We actually add characters to the
	 * ASCII string from lowest place value to highest, which is the
	 * opposite of how the number should be printed.  We'll reverse the
	 * characters later. */
	while(newval > 0) {
		i = newval % radix;
		*newbuf = lookup[i];
		newbuf++;
		newval /= radix;
	}

	/* Add a terminating NULL */
	*newbuf = '\0';

	/* Reverse the string and return */
	return strrev(buf);
}

/* In-place string reversal */
int8_t*
strrev(int8_t* s)
{
	register int8_t tmp;
	register int32_t beg=0;
	register int32_t end=strlen(s) - 1;

	while(beg < end) {
		tmp = s[end];
		s[end] = s[beg];
		s[beg] = tmp;
		beg++;
		end--;
	}

	return s;
}

/* String length */
uint32_t
strlen(const int8_t* s)
{
	register uint32_t len = 0;
	while(s[len] != '\0')
		len++;

	return len;
}

/* Optimized memset */
void*
memset(void* s, int32_t c, uint32_t n)
{
	c &= 0xFF;
	asm volatile("                  \n\
			.memset_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memset_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memset_aligned \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memset_top     \n\
			.memset_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     stosl           \n\
			.memset_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memset_done    \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%edx       \n\
			jmp     .memset_bottom  \n\
			.memset_done:           \n\
			"
			:
			: "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/* Optimized memset_word */
void*
memset_word(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosw           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/* Optimized memset_dword */
void*
memset_dword(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosl           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/* Optimized memcpy */
void*
memcpy(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			.memcpy_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memcpy_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memcpy_aligned \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memcpy_top     \n\
			.memcpy_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     movsl           \n\
			.memcpy_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memcpy_done    \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%edx       \n\
			jmp     .memcpy_bottom  \n\
			.memcpy_done:           \n\
			"
			:
			: "S"(src), "D"(dest), "c"(n)
			: "eax", "edx", "memory", "cc"
			);

	return dest;
}

/* Optimized memmove (used for overlapping memory areas) */
void*
memmove(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			cmp     %%edi, %%esi    \n\
			jae     .memmove_go     \n\
			leal    -1(%%esi, %%ecx), %%esi    \n\
			leal    -1(%%edi, %%ecx), %%edi    \n\
			std                     \n\
			.memmove_go:            \n\
			rep     movsb           \n\
			"
			:
			: "D"(dest), "S"(src), "c"(n)
			: "edx", "memory", "cc"
			);

	return dest;
}

/* Standard strncmp */
int32_t
strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
{
	int32_t i;
	for(i=0; i<n; i++) {
		if( (s1[i] != s2[i]) ||
				(s1[i] == '\0') /* || s2[i] == '\0' */ ) {

			/* The s2[i] == '\0' is unnecessary because of the short-circuit
			 * semantics of 'if' expressions in C.  If the first expression
			 * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
			 * s2[i], then we only need to test either s1[i] or s2[i] for
			 * '\0', since we know they are equal. */

			return s1[i] - s2[i];
		}
	}
	return 0;
}

/* Standard strcpy */
int8_t*
strcpy(int8_t* dest, const int8_t* src)
{
	int32_t i=0;
	while(src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0';
	return dest;
}

/* Standard strncpy */
int8_t*
strncpy(int8_t* dest, const int8_t* src, uint32_t n)
{
	int32_t i=0;
	while(src[i] != '\0' && i < n) {
		dest[i] = src[i];
		i++;
	}

	while(i < n) {
		dest[i] = '\0';
		i++;
	}

	return dest;
}

void
test_interrupts(void)
{
	int32_t i;
	for (i=0; i < NUM_ROWS*NUM_COLS; i++) {
		video_mem[i<<1]++;
	}
}
