REPORT FOR Q3.

Assumptions:-

	1. We can use 2 threads for a singer musician and singer.
	2. Optimization of any sort is not required.

Logic:-

	1. The bases of implementation are:
		a) Sem_wait can schedule for me as well.
		b) Having granted semaphore access need not mean we have entered a particular stage.

	2. The flow of code is:
		a) There is a separate function for acoustic and electric stages for musicians.
		   Singers have separate functions for solo performances at both the stages as well as joining.
		   I have used semaphores for electric stage, acoustic stage, CS(binary semaphore) and shirts(coordinators).

		b) Each void* function is accessed by different threads that may have originated for the same artist.
		   I hope this is allowed.

		c) When we see a musician, we send it to both, acoustic and electric, stage lines and then they wait
		   until they are allowed to enter a stage first. They raise a flag and the next entry for the same musician
		   in another stage shall know that it must exit immediately.

		d) If we see a singer, we send them to both stages again but now they also look to join some singer
		   if they may. If they get to perform solo, well and good. If not, they look to join and if they can,
		   they secretely tell the musician so that he can extend the time by 2 seconds. Meanwhile the singer too
		   waits for this whole period and exits.

		e) The musician was performing and just checks if it was joined.
		   If it was it will perform for 2 seconds more. After the performance gets over, the musicians go
		   to the "shirt wala" function and they wait for there to recieve the shirt. They then exit the event.

		f) To check for anger, singers simply sleep in their function for a time t whereas stages have a timedwait function
		   looking thing up for them.

Questions?
