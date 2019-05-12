# Exercise 6  
######  Programming SS 2019 - Problem Set 4
Author: *Elena Pfefferlé*, *Pascal Schenk*, *Àlvaro Morales*

We are asked to check the speed-up for combinations of MPI processes and sample points using one node on *Alphacruncher*.

Speed-up in latency is calculated as follow:

<img src="https://latex.codecogs.com/svg.latex?\Large&space;S=\frac{t_{old}}{t_{new}}"/>

In our case, *t<sub>old</sub>* is the runtime with **one** thread.

![alt text](speed-up.png)

**Note1** : The exercise asks to try combination of MPI processes between 1 & 20. However, we noticed that jobs submitted with more that 16 processes would not start;
- status *PD* (pending) and node reason *Ressources*.
- We assume that Alphacruncher doesn't allow us to go above 16 tasks in parallel.

**Note2**: Execution outputs are stored in *Exercise6/output/*.
