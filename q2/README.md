REPORT FOR Q2.	

Assumptions:
	1. A student can have their antibodies test right after being vaccinated.
	2. Busy waiting is allowed for this question.
	3. Any company can supply any zone and each company can reset the no. of batches and the batchsize for that particular round of manufacturing.

Logic:
	1. The bases of my logic are two things:-
		a) Use mutex lock to control CS access.
		b) Busy wait at any place where a certain condition has to be met before moving further.
	
	2. The flow of the program is:-
		
		a) Initiate students, zones and companies at the beginning with a thread for each.
		
		b) Each kind of a thread calls corresponding functions like Students call kids,
		   companies call company and zones call zone.
		
		c) Company starts producing whenever the vaccines it has made have vanished(therefore at the
		   beginning too). The company then assigns its batches to whichever zone it can and must now
		   wait until it has completely exhausted its stock(as well as its companies have).
		   Then it can resume production again.
		
		d) The zone thread has been waiting to be assigned a batch of vaccines and then must again
		   wait until it has filled all its slots or all the students have been assigned a slot in
		   some zone. Now the zone starts vaccinating and checking for antibodies right after that.
		   The zone shall now wait until it has all the slots for next round filled or the students
		   have been alloted something.
		
		e) Students arrive randomly and then check if they can grab a slot somewhere and wait until
		   they have. The function must exit after they have been assigned and this function shall
		   be called again incase a student wants to take another round of vaccination. We also keep
		   a check on the no. of rounds a students has taken so that we can get rid of him when need be.

Questions?
