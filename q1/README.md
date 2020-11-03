REPORT FOR Q1.

Assumptions:-
	1. Length of input be humane(like...<1e7).

Inferences:-
	1. For n = 10, threads were 300 times slower and procs were 700 times slower than the normal one.
	   For n = 100, threads were 320 times slower and procs were 800 times slower than the normal one.
	   For n = 1000, threads were 120 times slower and procs were 200 times slower than the normal one.
	   For n = 1000, threads were 600 times slower and procs were 1000 times slower than the normal one.
	2. From the stats above, it can be easily inferenced that threaded sorting costs more than 
	   normal one because of the extra costs for making and maintaining the thread.
	   Also multiprocess sorting costs more than a normal one due to reasons similar to mentioned above.
	   Threads seem to work faster than multiprocess sorting due to the extra time required to copy the existing data for the newly made process.
	   What is interesting is that n = 1000 tells us that this is a near optimal length of no. to sort by threads and procs due the fact that threads and procs do have a plus point where they can sort subarrays concurrently.

	3. It is clearly visible that even though threads and multiprocesses have a few edges, they can not
	   make up for the time lost in making and maintainance of threads and procs. Maybe, these will be able
	   to speed up some other process
