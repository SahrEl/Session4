/** 
 Programming SS 2019 - Problem Set 4
 Exercise 6
 Authors: Elena Pfefferlé, Pascal Schenk, Álvaro Morales
*/

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <algorithm>    // Needed for the "max" function
#include <cmath>
#include <iostream>
#include <vector>       // Needed for vectors construction
#include <numeric>      // Necessary for std::accumulate
#include <mpi.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 A simple implementation of the Box-Muller algorithm, used to
generate gaussian random numbers; necessary for the Monte Carlo
method below. */

double gaussian_box_muller() {
  double x = 0.0;
  double y = 0.0;
  double euclid_sq = 0.0;

  // Continue generating two uniform random variables
  // until the square of their "euclidean distance"
  // is less than unity
  do {
    x = 2.0 * rand() / static_cast<double>(RAND_MAX)-1;
    y = 2.0 * rand() / static_cast<double>(RAND_MAX)-1;
    euclid_sq = x*x + y*y;
  } while (euclid_sq >= 1.0);

  return x*sqrt(-2*log(euclid_sq)/euclid_sq);
}

int main (int argc, char *argv[])
{
    // asian options vars
    unsigned num_sims = 1000000;   // Number of simulated asset paths
    unsigned num_intervals = 100;  // Number of intervals for the asset path to be sampled
    double S = 100.0;  // Option price
    double K = 100.0;  // Strike prices
    double r = 0.05;   // Risk-free rate (5%)
    double v = 0.2;    // Volatility of the underlying (20%)
    double T = 1.0;    // One year until expiry
    std::vector<double> spot_prices(num_intervals, S);  // The vector of spot prices

    // pre-parallel initializations
    typedef std::vector<double> dbl_vector;
    typedef std::vector<dbl_vector> dbl_double_vector;
    dbl_double_vector dp(num_sims, dbl_vector(num_intervals));
    // generating random numbers in an array of arrays
    for (int a=0; a<num_sims; a++) {
      for (int b=1; b<spot_prices.size(); b++) {
        dp[a][b] = gaussian_box_muller();
      }
    }
    // asian option computed values
    double dt = T/static_cast<double>(spot_prices.size());
    double drift = exp(dt*(r-0.5*v*v));
    double vol = sqrt(v*v*dt);
    // asian options variables to compute
    float local_payoff_sum = 0.0;
    double arith_mean = 0.0;
    double sum = 0.0;
    // MPI reduce results
    float global_payoff_sum = 0.0;
    // --------------------------- parallel -----------------------------------
    // MPI initialization
    int process, proc_qty;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &process);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_qty);

    // Time measure variables
    double t1, t2;
    t1 = MPI_Wtime();
    // Loops trough all the simulated path
    for (int i = 0; i < num_sims; i++)
    {
        /*
         If the result of the index modulus number of allocated process
         is not equal to to the process number, the turn is passed.
        */
        if (i%proc_qty != process) continue;
        /*
         Computes the asian option, the computation is done
         locally; at the process level.
        */
        for (int j=1; j<spot_prices.size(); j++)
        {
          spot_prices[j] = spot_prices[j-1] * drift * exp(vol*dp[i][j]);
        }
        sum = std::accumulate(spot_prices.begin(), spot_prices.end(), 0);
        arith_mean = sum / static_cast<double>(spot_prices.size());
        local_payoff_sum += std::max(arith_mean-K, 0.0);
    }

    // Reduces the local_payoff_sums into a global payoff sum
    MPI_Reduce(&local_payoff_sum, &global_payoff_sum, 1, MPI_FLOAT, MPI_SUM, 0,
               MPI_COMM_WORLD);

    // The following is executed only by the root process
    if (process == 0) {
      double discount_payoff_avg = (global_payoff_sum / static_cast<double>(num_sims)) * exp(-r*T);
      // time measure
      t2 = MPI_Wtime();
      // Finally we output the parameters and prices
      std::cout << "MPI Processes  : " << proc_qty << std::endl;
      std::cout << "Number of Paths: " << num_sims << std::endl;
      std::cout << "Underlying:      " << S << std::endl;
      std::cout << "Strike:          " << K << std::endl;
      std::cout << "Risk-Free Rate:  " << r << std::endl;
      std::cout << "Volatility:      " << v << std::endl;
      std::cout << "Maturity:        " << T << std::endl;
      std::cout << "Asian     :      " << discount_payoff_avg << std::endl;
      std::cout << "Time      :      " << t2 - t1 << std::endl;

    }
    // Waits till all processes finished
    MPI_Barrier(MPI_COMM_WORLD);
    // Processes finalization
    MPI_Finalize();
}
