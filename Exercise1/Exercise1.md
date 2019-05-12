# Exercise 1
######  Programming SS 2019 - Problem Set 4
Author: *Elena Pfefferlé*, *Pascal Schenk*, *Àlvaro Morales*

#### Problem:

We are asked to calculate the theoretical maximum speed-up a code with 0.4% serial instructions could achieve with 100 CPUs.

#### Resolution:

If a program of size *W* has a serial component *Ws*, the speedup *S* of the program is

<img src="https://latex.codecogs.com/svg.latex?\Large&space;S=\frac{W}{\frac{W-Ws}{p}+Ws}"/>

In our case *Ws = 0.4%* and *W-Ws = 99.6%*, thefore:

<img src="https://latex.codecogs.com/svg.latex?\Large&space;S=\frac{1}{\frac{0.996}{p}+0.004}"/>

But since  <img src="https://latex.codecogs.com/svg.latex?\Large&space;p \rightarrow \infty"/> :

<img src="https://latex.codecogs.com/svg.latex?\Large&space;S=\frac{1}{0.004} = 250 "/>

No matter how many processors are used, the speedup can not be greater than 250.
