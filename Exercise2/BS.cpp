/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * This file contains routines to serially compute the call and
 * put price of an European option.
 *
 * Simon Scheidegger -- 06/17.
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include <algorithm>    // Needed for the "max" function
#include <cmath>
#include <iostream>
#include <vector>
#include <numeric>  // Necessary for std::accumulate
#include <chrono>
#include <omp.h>
using namespace std::chrono;
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
// Pricing a European vanilla call option with a Monte Carlo method

double monte_carlo_call_price(const int& num_sims, const double& S, const double& K, const double& r, const double& v, const double& T) {
  double S_adjust = S * exp(T*(r-0.5*v*v));
  double S_cur = 0.0;
  double payoff_sum = 0.0;

  for (int i=0; i<num_sims; i++) {
    double gauss_bm = gaussian_box_muller();
    S_cur = S_adjust * exp(sqrt(v*v*T)*gauss_bm);
    payoff_sum += std::max(S_cur - K, 0.0);
  }

  return (payoff_sum / static_cast<double>(num_sims)) * exp(-r*T);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Pricing a European vanilla put option with a Monte Carlo method

double monte_carlo_put_price(const int& num_sims, const double& S, const double& K, const double& r, const double& v, const double& T) {
  double S_adjust = S * exp(T*(r-0.5*v*v));
  double S_cur = 0.0;
  double payoff_sum = 0.0;

  for (int i=0; i<num_sims; i++) {
    double gauss_bm = gaussian_box_muller();
    S_cur = S_adjust * exp(sqrt(v*v*T)*gauss_bm);
    payoff_sum += std::max(K - S_cur, 0.0);
  }

  return (payoff_sum / static_cast<double>(num_sims)) * exp(-r*T);
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
  // Then we calculate the call/put values via Monte Carlo
  // double call = monte_carlo_call_price(num_sims, S, K, r, v, T);
  // double put = monte_carlo_put_price(num_sims, S, K, r, v, T);
  // time measure
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  // pre-parallel initializations
  typedef std::vector<double> dbl_vector;
  typedef std::vector<dbl_vector> dbl_double_vector;
  dbl_double_vector dp(num_sims, dbl_vector(num_intervals));
  // generating seeds
  for (int a=0; a<num_sims; a++) {
    for (int b=1; b<spot_prices.size(); b++) {
      dp[a][b] = gaussian_box_muller();
    }
  }

  double dt = T/static_cast<double>(spot_prices.size());
  double drift = exp(dt*(r-0.5*v*v));
  double vol = sqrt(v*v*dt);

  double payoff_sum = 0.0;

  int i,j;


  #pragma omp parallel
    {
      int threadid;
      std::vector<double> vec;  //copy of the vector (one per thread)
      std::copy(begin(spot_prices), end(spot_prices), std::back_inserter(vec));
      unsigned num_times = 0.0;
      double arith_mean = 0.0;
      double gauss_bm = 0.0;
      double sum = 0.0;


      #pragma omp for private(j, gauss_bm, arith_mean, num_times, sum) reduction(+:payoff_sum)
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
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  // Finally we output the parameters and prices
  std::cout << "Number of Paths: " << num_sims << std::endl;
  std::cout << "Underlying:      " << S << std::endl;
  std::cout << "Strike:          " << K << std::endl;
  std::cout << "Risk-Free Rate:  " << r << std::endl;
  std::cout << "Volatility:      " << v << std::endl;
  std::cout << "Maturity:        " << T << std::endl;

  std::cout << "Asian     :      " << discount_payoff_avg << std::endl;
  std::cout << "Time      :      " << duration_cast<microseconds>( t2 - t1 ).count() << std::endl;

  return 0;
}
