There are 3 types of threads:
1) N counter threads (N is a constant having a value of 5)
2) 1 monitor thread
3) 1 collector thread

Dividing the problem into 2 subproblems:

•Subproblem 1:
-Counter threads increment the shared integer variable (counter) an sleep for a random peroid of time
-Monitor thread reads and saves the shared variable and resets the counter
*Only ONE thread allowed to access counter at a time.

•Subproblem 2: Bounded-buffer producer-consumer problem (buffer implemented as FIFO queue)
-Producer: Monitor thread enqueues saved value to buffer.
-Consumer: Collector thread takes data out of buffer.
*Only ONE thread allowed to access counter at a time.

