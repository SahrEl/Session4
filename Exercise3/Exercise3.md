# Exercise 3  
######  Programming SS 2019 - Problem Set 4
Author: *Elena Pfefferlé*, *Pascal Schenk*, *Àlvaro Morales*

#### Exercise 3B-C:

We are asked to experiment with the number of random numbers created and to run the code with varying number of threads (1,2,4,8).

Here is how the execution time varies:

![alt text](exectime.png)

We can observe that with 1000 random numbers the quantity of thread is not accelerating code execution but slowing it down. This is because for such a little amount of variables, the code is naturally very quickly executed and adding parallel instruction only adds load to the processors thus slowing them down.

**Note**: Execution outputs are stored in *Exercise3/output/*.
