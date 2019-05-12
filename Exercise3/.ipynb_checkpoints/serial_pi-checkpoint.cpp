#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main()
{
    unsigned niter = 1000000;
    // double x,y;
    int i;
    unsigned count=0;
    // double z;
    double pi;
    //srand(time(NULL));

    double time = -omp_get_wtime();
    //main loop
    #pragma omp parallel
      {
        unsigned int myseed = omp_get_thread_num();
        double x,y,z;
        #pragma omp for private(x,y,z,i) reduction(+:count) schedule(static,4)
        for (int i=0; i < niter; i++)
        {
            //get random points
            x = (double)rand_r(&myseed)/RAND_MAX;
            y = (double)rand_r(&myseed)/RAND_MAX;
            z = sqrt((x*x)+(y*y));
            //check to see if point is in unit circle
            if (z<=1)
            {
                count +=1;
            }
        }

      }
    time += omp_get_wtime();
    pi = ((double)count/(double)niter)*4.0;          //p = 4(m/n)
    printf("Pi: %f\n", pi);
    printf("Error: %f\n", M_PI-pi);
    printf("Time: %f\n", time);
    return 0;
}
