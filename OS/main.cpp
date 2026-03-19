#include "../Inc/main.hpp"

feedback clocksInit()
{
	//	Get the CMOS and RCC instances
	CMOS& cmos = CMOS::get();
	RCC& rcc = STM32H753BIT6::get().get_rcc();
	
	
	//	This is the clock tree (the values are just examples, you can change them as you wish but keep the maximum frequencies in mind -> datasheet of the MCU)
	
	//				PLL 1													PLL 2													PLL 3
	//			 960 MHz											 460 MHz											 460 MHz
	//		 -----------									 -----------									 -----------
	//		| 		|			|									|			|			|									|			|			|
	//		P			Q			R									P			Q			R									P			Q			R
	//		2			8			10								6			1			2									1			1			26
	//		|			|			|									|			|			|									|			|			|
	//	 M7		 												 				 													 
	//	 																					 
	
	
	//	The systemclock we want to achieve
	constexpr uint32 systemClock = 480000000;
	
	
	//	Initialize the HSE oscillator (only do so if the PCB has one soldered!)
	if(rcc.HSE_init(true) != OK)
	{
		return(FAIL);
	}
	
	
	//	Set the HSE as the clock source for the PLLs (the PLLs will multiply this frequency to achieve the desired system clock)
	if(rcc.set_clockSource_pll(RCC::e_clockSource_pll::HSE) != OK)
	{
		return(FAIL);
	}
	
	
	//	Initialize the PLLs with the desired frequencies and dividers (see clock tree above)
	if(rcc.pll_1_init(2 * systemClock, 2, 8, 10) != OK)
	{
		return(FAIL);
	}
	if(rcc.pll_2_init(460000000, 6, 1, 2) != OK)
	{
		return(FAIL);
	}
	if(rcc.pll_3_init(460000000, 1, 1, 26) != OK)
	{
		return(FAIL);
	}
	
	
	//	Set HSE as the clock source for the PERCK clock
	if(rcc.set_clockSource(RCC::e_clockSource_perck::HSE) != OK)
	{
		return(FAIL);
	}
	
	
	//	Important: inform the OS about the new system clock frequency (this is needed for the OS to calculate the correct timings for the sleep functions, etc...)
	//	Do this BEFORE changing the clock source, if you increase the systemclock
	//	Do this AFTER changing the clock source, if you decrease the systemclock (to avoid timing issues during the transition)
	if(cmos.set_systemClock(systemClock) != OK)
	{
		return(FAIL);
	}
	
	
	//	Set the PLL1 as the clock source for the system clock
	if(rcc.set_clockSource_system(RCC::e_clockSource_system::PLL1) != OK)
	{
		return(FAIL);
	}
	return(OK);
}




void threadFunction()
{
	//	This is a thread that does nothing, we just need it as an example function to create a thread in main()
	CMOS& cmos = CMOS::get();
	while(1)
	{
		//	Check for a mail with the type "Thread::e_mailType::EXIT" to know when to terminate the thread gracefully
		if(cmos.is_mailAvailable() == true)
		{
			Thread::s_mail mail = cmos.read_mail();
			if(mail.type == Thread::e_mailType::EXIT)
			{
				//	Clean up resources here if needed (free memory, semaphores, shut down bus-systems, etc...)
				//	Then terminate the thread gracefully by simply returning from the thread function
				return;
			}
		}
		
		
		cmos.sleep_ms(100);
	}
}




int main()
{
	//	Initialize the clocks - this is MCU specific
	if(clocksInit() != OK)
	{
		return(FAIL);
	}
	

	//	Get the CMOS instance - this instance is MCU independent and must be used to access the OS functions and the core peripherals (NVIC, etc...)
	CMOS& cmos = CMOS::get();
	
	
	
	
	
	
	
	
	
	//	The examples below do not always check the return values of the functions for the sake of simplicity,
	//	but in a real application you should always check the return values to handle errors properly
	
	
	
	
	
	
	
	
	
	
	
	
	//	Thread System
	
	//	Create a thread
	//	We want to take the above function "threadFunction" and run it as a new thread
	//	Parameters:
	//		Mandatory:
	//		-	The function to run as a thread
	//		-	The name of the thread (for debugging purposes, can be anything you want)
	//		-	The priority of the thread (0-255, 0 is the lowest priority, 255 is the highest priority)
	//		Optional:
	//		-	The stack size of the thread (0 = default is set in the linker script with the symbol "__cmos_stack_blockSize__" in bytes,
	//																		you can change it if your thread needs more or less stack space,
	//																		but it will always be rounded to the nearest full stack block size)
	//		-	The mailbox size of the thread (0 = default takes one heap block and puts as many mailbox messages in it as possible,
	//																			you can change it if your thread needs more or less mailbox space, the unit is the number of messages, not bytes)
	//		-	THE PARAMETER "object" MUST NOT BE USED BY THE USER AND BE LEFT AT DEFAULT VALUE!
	const uint8 threadID = cmos.thread_create(threadFunction, "Example Thread", 100);
	
	
	//	Check if the thread was created successfully
	if(threadID == CMOS::threadID_invalid)
	{
		//	Something went wrong, the thread was not created successfully
		return(FAIL);
	}
	
	
	//	Changing this thread's priority at runtime (we cant change the priority of other threads, only of our own thread)
	cmos.thread_setPriority(150);
	
	
	//	Joining a thread means that the parent thread will wait for the child thread to finish before it continues execution
	if(cmos.thread_joinChildThread(threadID) != OK)
	{
		//	Something went wrong, the thread was not joined successfully
		return(FAIL);
	}
	
	
	//	Lets shut down the child thread that we created, this will terminate the thread and free all its resources
	//	Optional:	Set the parameter "hardShutdown" to "true" to immediately terminate the thread
	//						without giving it the chance to clean up its resources (use this if the thread is stuck in an infinite loop or is not responding)
	//						If set to "false", the child thread will receive a mail with "Thread::e_mailType::EXIT" so that it can clean up its resources and terminate itself gracefully
	if(cmos.thread_shutdown(threadID) != OK)
	{
		//	Something went wrong, the thread was not shut down successfully
		return(FAIL);
	}
	
	
	//	We might want to detach the child thread that we created, so that it can run independently
	//	Child threads get automatically terminated when the parent thread terminates,
	//	but if we detach the child thread, it will continue to run even if the parent thread terminates
	if(cmos.thread_detachChildThread(threadID) != OK)
	{
		//	Something went wrong, the thread was not detached successfully
		return(FAIL);
	}
	
	
	//	Here are some simple info-functions to acquire information about the threads in the system
	
	//	Query by ID or by name
	const bool isThreadRunningID = cmos.thread_doesExist(threadID);
	const bool isThreadRunningName = cmos.thread_doesExist("Non-existing Thread Name");
	
	const bool isChildThreadRunningID = cmos.isChildThread(threadID);
	
	const uint8 threadIDOfThisThread = cmos.get_runningThreadID();
	const uint8 threadIDOfParentThread = cmos.get_parentThreadID();
	
	const String threadName = cmos.thread_getName(threadID);
	const uint8 threadIDQueryByName = cmos.thread_getID("Example Thread");
	
	//	Check if a thread ID is valid
	const bool isThreadIDValid = cmos.isValid_threadID(threadID);
	
	//	Invalidate a threadID Variable
	uint8 threadIDValid = cmos.get_runningThreadID();
	cmos.invalidate(threadIDValid);
	
	
	//	Read out how many threads can be created at maximum in this system
	//	This is a fixed value that is set in the linker script with the symbol "__cmos_numberOfThreads__"
	const uint8 maximumNumberOfThreadsThatCanBeCreated = cmos.get_numberOfThreadsMaximum();
	
	
	
	
	
	
	
	
	
	
	
	
	
	//	Sleep System
	//	The thread with the highest priority keeps running as long as it doesnt use any sleep function
	//	If it uses a sleep function, it will be put to sleep until the sleep condition is not present anymore
	//	and the thread with the next highest priority will run until it also uses a sleep function, etc...
	
	//	Sleep Condition: Sleep for a certain amount of time
	cmos.sleep_sec(1);
	cmos.sleep_ms(1);
	cmos.sleep_100us(1);		//	This function only exists on faster cores like the Cortex-M7, it will not be available on slower cores like the Cortex-M0, etc...
	
	
	//	Sleep Condition: Sleep until a specified event occurs
	
	//	First we need to create an event and subscribe ourself to it, so that we can wait for this event to occur
	//	The event needs to be created only once, but you can subscribe and unsubscribe to it as many times as you want, this allows you to reuse the same event for different purposes at different times in your code
	//	So dont create a new event every time you want to wait for something because you will run out of event IDs then
	const uint16 eventID = cmos.event_create();
	cmos.event_subscribe(eventID);
	
	//	Now we can sleep until this event occurs
	cmos.sleep_untilEvent();
	
	//	To trigger the event and wake up the sleeping thread, we can emit the event from another thread (or from an interrupt, etc...)
	cmos.event_emit(eventID);
	
	//	After we are done with the event, we can unsubscribe from it
	//	We dont necessarily need to unsubscribe, because we will only wake up from an event if we use the "sleep_untilEvent" function
	//	But it is good practice to unsubscribe from events when we are done with them, to avoid accidentally waking up from an event that we are not interested in anymore
	cmos.event_unsubscribe(eventID);
	
	
	//	Here are the "event_listen" and "event_unlisten" functions (their function and the reason for their existence is explained in the CMOS Documentation Rev. 0 -> Chapter 3.1.8.30)
	cmos.event_listen(eventID);
	cmos.event_unlisten();
	
	
	//	Sleep Condition: Sleep until a mail is received in the thread's mailbox (in this example from the parent thread, but it can also be from any other thread)
	const uint8 senderID = cmos.get_parentThreadID();
	cmos.sleep_untilMail(senderID);
	
	
	
	
	
	
	
	
	
	
	
	
	
	//	Mail System
	//	The mail system allows threads to communicate with other threads by sending and receiving mails through their mailboxes
	
	//	Send Mail (in this example to the parent thread, but you can also send mails to any other thread by specifying the receiver's thread ID)
	const uint8 receiverID = cmos.get_parentThreadID();
	constexpr uint32 mailData = 0xDEADBEEF;
	cmos.send_mail(receiverID, mailData);
	
	//	Processes all mails in the mailbox
	while(cmos.is_mailAvailable() == true)
	{
		//	Read mail
		const Thread::s_mail mail = cmos.read_mail();
		
		//	Process mail here
		const Thread::e_mailType mailType = mail.type;
		const uint8 mailSenderID = mail.senderID;
		const uint32 mailDataReceived = mail.data;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	//	Semaphore System
	//	The semaphore system allows threads to synchronize with each other and to protect shared resources from concurrent access
	
	//	Create a semaphore
	//	The variable "objectToProtect" represents a shared resource that we want to protect with a semaphore, it can be anything (a variable, a peripheral, etc...)
	uint8 objectToProtect = 0;
	if(cmos.semaphore_create(&objectToProtect) != OK)
	{
		//	Something went wrong, the semaphore was not created successfully
		return(FAIL);
	}
	
	
	//	Lock the semaphore before accessing the shared resource
	if(cmos.semaphore_lock(&objectToProtect) != OK)
	{
		//	Something went wrong, the semaphore was not locked successfully
		return(FAIL);
	}
	
	
	//	Access the shared resource here
	objectToProtect++;
	
	
	//	Unlock the semaphore after we are done accessing the shared resource
	if(cmos.semaphore_unlock(&objectToProtect) != OK)
	{
		//	Something went wrong, the semaphore was not unlocked successfully
		return(FAIL);
	}
	
	
	//	Erase the semaphore if it is not needed anymore
	if(cmos.semaphore_erase(&objectToProtect) != OK)
	{
		//	Something went wrong, the semaphore was not erased successfully
		return(FAIL);
	}
	
	
	//	Check if a semaphore is currently locked or not
	const bool isLocked = cmos.semaphore_isLocked(&objectToProtect);
	
	
	//	Check if a semaphore already exists for a certain object
	const bool doesExist = cmos.semaphore_doesExist(&objectToProtect);
	
	
	//	Transfer the ownership of the semaphore to the parent thread
	//	This is useful when the child thread needs to terminate and the parent thread needs to take over the responsibility of unlocking and erasing the semaphore, etc...
	cmos.semaphore_transferAllOwnershipsToParentThread();
	
	
	//	Some additional Queries
	const uint8 semaphoreOwner = cmos.semaphore_getOwner(&objectToProtect);
	const bool isOwnedByRunningThread = cmos.semaphore_isOwnedByRunningThread(&objectToProtect);
	
	
	
	
	
	
	
	
	
	
	
	
	//	System time
	
	//	Read the system time in ticks since the system started
	const uint64 systemTimeTicks = cmos.get_ticks();
	
	//	Convert ticks to seconds, milliseconds, or microseconds
	const uint64 systemTimeSeconds			=           systemTimeTicks / cmos.c_clock_systick;
	const uint64 systemTimeMilliseconds =    1000 *	systemTimeTicks / cmos.c_clock_systick;
	const uint64 systemTimeMicroseconds = 1000000 *	systemTimeTicks / cmos.c_clock_systick;
	
	
	//	Read the system time as time object
	const Time time = cmos.get_time();
	const uint8 hours = time.hour;
	const uint8 minutes = time.minute;
	const uint8 seconds = time.second;
	
	
	
	
	
	
	
	
	while(1)
	{
		
	}
	return(OK);
}
