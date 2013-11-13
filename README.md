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
