/**
 Programming SS 2019 - Problem Set 4
 Exercise 3d
 Authors: Elena Pfefferlé, Pascal Schenk, Álvaro Morales
*/

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include <algorithm>    // Needed for the "max" function
#include <cmath>
#include <iostream>
#include <vector>
#include <numeric>  // Necessary for std::accumulate
#include <omp.h>
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main(int argc, char **argv) {
  int num_threads = omp_get_max_threads();
  // Parameters
  unsigned num_sims = 100000;   // Number of simulated asset paths
  unsigned num_intervals = 100;  // Number of intervals for the asset path to be sampled
  double S = 100.0;  // Option price
  double K = 100.0;  // Strike price
  double r = 0.05;   // Risk-free rate (5%)
  double v = 0.2;    // Volatility of the underlying (20%)
  double T = 1.0;    // One year until expiry
  std::vector<double> spot_prices(num_intervals, S);  // The vector of spot prices

  // time measure
  double time = -omp_get_wtime();

  /*
  Creates a new type to be able to create arrays of dimension num_sims x
  num_intervals filled with doubles.
  */
  typedef std::vector<double> dbl_vector;
  typedef std::vector<dbl_vector> dbl_double_vector;
  dbl_double_vector dp(num_sims, dbl_vector(num_intervals));
  // Fills the array with  random numbers.
  for (int a=0; a<num_sims; a++) {
    for (int b=1; b<spot_prices.size(); b++) {
      dp[a][b] = gaussian_box_muller();
    }
  }

  //Asian options computed values
  double dt = T/static_cast<double>(spot_prices.size());
  double drift = exp(dt*(r-0.5*v*v));
  double vol = sqrt(v*v*dt);

  // variables initiated before the parallel region
  double payoff_sum = 0.0;
  int i,j;

  // -------------------------parallel region ----------------------------------
  #pragma omp parallel
    {
      /*
      The following variables exist within the scope of each thread, meaning that
      a copy of these variables is created per thread running the code.
      */
      int threadid;
      std::vector<double> vec;
      std::copy(begin(spot_prices), end(spot_prices), std::back_inserter(vec));
      unsigned num_times = 0.0;
      double arith_mean = 0.0;
      double gauss_bm = 0.0;
      double sum = 0.0;
      /*
       Parallel loop, reduction on the sum of the payoff
       j is private to each thread, if not each thread would increase the
       index turn by turn.
      */
      #pragma omp for private(j) reduction(+:payoff_sum)
      for ( i=0; i<num_sims; i++)
          {
              for ( j=1; j<vec.size(); j++)
              {
                gauss_bm = dp[i][j];
                vec[j] = vec[j-1] * drift * exp(vol*gauss_bm);
              }
              num_times = vec.size();
              sum = std::accumulate(vec.begin(), vec.end(), 0);
              arith_mean = sum / static_cast<double>(num_times);
              payoff_sum += std::max(arith_mean-K, 0.0);
      }
    }
  double discount_payoff_avg = (payoff_sum / static_cast<double>(num_sims)) * exp(-r*T);
  // time measure
  time += omp_get_wtime();
  // Finally we output the parameters and prices
  std::cout << "Number of Paths: " << num_sims << std::endl;
  std::cout << "Underlying:      " << S << std::endl;
  std::cout << "Strike:          " << K << std::endl;
  std::cout << "Risk-Free Rate:  " << r << std::endl;
  std::cout << "Volatility:      " << v << std::endl;
  std::cout << "Maturity:        " << T << std::endl;

  std::cout << "Asian     :      " << discount_payoff_avg << std::endl;
  std::cout << "Time      :      " << time << " sec" << std::endl;

  return 0;
}
