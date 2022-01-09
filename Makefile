CCOPTS=-g -Wall -Wextra -std=gnu99
portforward: portforward.c 
	$(CC) $(CCOPTS) -o $@ $(filter-out %.h,$^) 

