ECE-391-MP3 (Jordan Kravitz, Anarghya Mitra, Chung Rhee, Sam Rohde)
===========

Done so far:
-------------------------
* Initialize GDT
* Initialize IDT 
* Initialize PIC, keyboard, RTC

Todo:
-------------------------

* Paging
* Terminal driver
* Read-only file system
* Complete RTC driver
* System calls
* Tasks
* Loader
* Multiple terminals and active tasks
* Scheduling


Checkpoint 3:
	Tasks
		-Framework 
			-Task Struct
		-Simple scheduler
			-Managing Task linked list
		-Task Switching
			-Change from CR0 to CR3
			-Save and change TSS
		-Task Signals
		
	System Calls
		-Execute Call
		-Everything Else
		
	Loader
		-Load from Memory
		
Can be Concurrrent	
	File Loader & Tasks
	File Loader & System Calls (NOT Execute)
	Tasks & System Calls (NOT Execute)

Cannot be Concurrent
	Order:
		1) File Loader (Have to have the program in program image)
		2) Tasks (Task switching)
		3) Execute System Call

execute(const uint8_t * command);
{
	/*	Parse command into (use the getargs system call)
	* 		1) Command to execute
	* 		2) Get the arguments
	*	
	*	Create page directory
	*	Load program into memory
	*	Create new task struct
	*		1) Get PID
	*		2) FD Array: Sets the Stdin, Stdout
	*		3) Sets children/parent/siblings
	*
	*	Context Switch
	*		1) Switch TSS 
	*			a) Switching the segments
	*			b) Switching CR3
	*		2) Set up Stack
	*		3) IRET
	*/
}
