/**
 Programming SS 2019 - Problem Set 4
 Exercise 3bc
 Authors: Elena Pfefferlé, Pascal Schenk, Álvaro Morales
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
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
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int main() {
  int num_threads = omp_get_max_threads();
  // Parameters
  unsigned num_sims = 10000;   // Number of simulated asset paths
  unsigned num_intervals = 100;  // Number of intervals for the asset path to be sampled
  double S = 100.0;  // Option price
  double K = 100.0;  // Strike price
  double r = 0.05;   // Risk-free rate (5%)
  double v = 0.2;    // Volatility of the underlying (20%)
  double T = 1.0;    // One year until expiry
  std::vector<double> spot_prices(num_intervals, S);  // The vector of spot prices

  // time measure
  double time = -omp_get_wtime();
  // Asian option computed values
  double zij = T/static_cast<double>(spot_prices.size());
  double drift = exp(zij*(r-0.5*v*v));
  double vol = sqrt(v*v*zij);
  // Asian option payoff sum
  double payoff_sum = 0.0;
  // iterates the simulated assets path
  for (int i=0; i<num_sims; i++)
    {
      for (int j=1; j<spot_prices.size(); j++)
      {
        double gauss_bm = gaussian_box_muller();
        spot_prices[j] = spot_prices[j-1] * drift * exp(vol*gauss_bm);
      }
      double num_times = spot_prices.size();
      double sum = std::accumulate(spot_prices.begin(), spot_prices.end(), 0);
      double arith_mean = sum / static_cast<double>(num_times);
      payoff_sum += std::max(arith_mean-K, 0.0);
    }
  // asian option pricing
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
