//intialize semaphores

semaphore empty = 1; //ocupied
semaphore male = 1; //male mutex
semaphore female = 1; //female mutex
semaphore male_count = n; //mutiplex varibale, limit to # in bathroom
semapore femail_count = n;

int fcount = 0, mcount = 0; //count of people who have come

semaphore turnstile = 1;

FEMALE

turnstile.wait()
female.wait()
fcount = fcount + 1
if(fcount == 1)
	empty.wait()
female.signal()
turnstile.signal()
female_count.wait()

//enter bathroom

female_count.signal()
female.wait()
fcount = fcount - 1
if(fcount == 0)
	empty.signal()
female.signal()




MALES

turnstile.wait()
male.wait()
mcount ++
if(mcount == 1)
	empty.wait()
male.signal()
turnstile.signal()
male_count.wait()

//enter bathroom

male_count.signal()
male.wait()
mcount --
if (mcount == 0)
	empty.signal()
male.signal()
