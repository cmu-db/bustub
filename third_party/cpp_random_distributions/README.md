# C++ Random Distributions

This repository provides header-only classes with additional statistical distributions to be used by C++11 random generators.

Example:
```c++
#include <random>
#include "selfsimilar_int_distribution.h"

int main() {
    std::default_random_engine generator;
    selfsimilar_int_distribution<int> distribution(1, 10, 0.2);
    int i = distribution(generator);
}
```

Most formulas are taken from: ["Quickly Generating Billion-Record Synthetic Databases", Jim Gray et al, SIGMOD 1994".](https://dl.acm.org/doi/10.1145/191843.191886)

## Self-Similar
The self-similar distribution has a skew factor `h` represented by a double in the range `(0.0,1.0)`.
For a domain of size `N`, the distribution of probability is such that the first `(N*h)` elements are generated `(1-h)` of the times.
In other words, for a domain of 25 elements and skew of 0.2, the first 5 elements (20% of 25) will be generated 80% (1-0.2) of the times.

This distribution also has the property that the skew is the same within any region of the domain.
Therefore, the first element (20% of 20% of 25) will be generated 64% (80% of 80%) of the times.

In the plots below we generate random integers in the range `[1,25]` (X axis) and plot the percentage of times they were generated (Y axis) with different skew factors.

<p align="center">
  <img src="https://user-images.githubusercontent.com/7251387/72623904-3c46fa80-3946-11ea-8781-6814c284d616.png">
</p>

## Zipfian
The Zipfian distribution is a power law distribution.
It has a parameter `h` indicating the skew factor (higher means more skew).
Since the Zipfian distribution is based on the zeta distribution, it requires calculating the zeta value upon initialization.
Calculating the zeta value can be unpractical for very large domains, therefore the user has the option of passing a pre-calculated zeta value to the distribution.

In the plots below we generate random integers in the range `[1,25]` (X axis) and plot the percentage of times they were generated (Y axis) with different skew factors.
Note that the Zipfian distribution is less skewed than the self-similar one, as the percentages on the Y axis are lower.

<p align="center">
  <img src="https://user-images.githubusercontent.com/7251387/72624553-7795f900-3947-11ea-8cf2-d53d00cf5196.png">
</p>

