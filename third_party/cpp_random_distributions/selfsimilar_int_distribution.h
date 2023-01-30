/*
 * MIT License
 *
 * Copyright (c) 2017 Lucas Lersch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Implementation derived from:
 * "Quickly Generating Billion-Record Synthetic Databases", Jim Gray et al,
 * SIGMOD 1994
 */

/*
 * The selfsimilar_int_distribution class is intended to be compatible with
 * other distributions introduced in #include <random> by the C++11 standard.
 * 
 * The distribution of probability is such that the first (N*skew) elements are
 * generated (1-skew) of the times. This distribution also has the property
 * that the skew is the same within any region of the domain.
 * 
 * Usage example:
 * #include <random>
 * #include "selfsimilar_int_distribution.h"
 * int main()
 * {
 *   std::default_random_engine generator;
 *   std::selfsimilar_int_distribution<int> distribution(1, 10, 0.2);
 *   int i = distribution(generator);
 * }
 */

#include <cmath>
#include <limits>
#include <random>
#include <cassert>

template<typename _IntType = int>
class selfsimilar_int_distribution
{
  static_assert(std::is_integral<_IntType>::value, "Template argument not an integral type.");

public:
  /** The type of the range of the distribution. */
  typedef _IntType result_type;
  /** Parameter type. */
  struct param_type
  {
    typedef selfsimilar_int_distribution<_IntType> distribution_type;

    explicit param_type(_IntType __a = 0, _IntType __b = std::numeric_limits<_IntType>::max(), double __skew = 0.2)
    : _M_a(__a), _M_b(__b), _M_skew(__skew)
    {
      assert(_M_a <= _M_b && _M_skew > 0.0 && _M_skew < 1.0);
    }

    result_type	a() const { return _M_a; }

    result_type	b() const { return _M_b; }

    double skew() const { return _M_skew; }

    friend bool	operator==(const param_type& __p1, const param_type& __p2)
    {
      return __p1._M_a == __p2._M_a
          && __p1._M_b == __p2._M_b
          && __p1._M_skew == __p2._M_skew;
    }

  private:
    _IntType _M_a;
    _IntType _M_b;
    double _M_skew;
  };

public:
  /**
   * @brief Constructs a selfsimilar_int_distribution object.
   *
   * @param __a [IN]  The lower bound of the distribution.
   * @param __b [IN]  The upper bound of the distribution.
   * @param __skew [IN]  The skew factor of the distribution.
   */
  explicit selfsimilar_int_distribution(_IntType __a = _IntType(0), _IntType __b = _IntType(1), double __skew = 0.2)
  : _M_param(__a, __b, __skew)
  { }

  explicit selfsimilar_int_distribution(const param_type& __p) : _M_param(__p)
  { }

  /**
   * @brief Resets the distribution state.
   *
   * Does nothing for the selfsimilar int distribution.
   */
  void reset() { }

  result_type a() const { return _M_param.a(); }

  result_type b() const { return _M_param.b(); }

  double skew() const { return _M_param.skew(); }

  /**
   * @brief Returns the parameter set of the distribution.
   */
  param_type param() const { return _M_param; }

  /**
   * @brief Sets the parameter set of the distribution.
   * @param __param The new parameter set of the distribution.
   */
  void param(const param_type& __param) { _M_param = __param; }

  /**
   * @brief Returns the inclusive lower bound of the distribution range.
   */
  result_type min() const { return this->a(); }

  /**
   * @brief Returns the inclusive upper bound of the distribution range.
   */
  result_type max() const { return this->b(); }

  /**
   * @brief Generating functions.
   */
  template<typename _UniformRandomNumberGenerator>
  result_type operator()(_UniformRandomNumberGenerator& __urng)
  { return this->operator()(__urng, _M_param); }

  template<typename _UniformRandomNumberGenerator>
  result_type operator()(_UniformRandomNumberGenerator& __urng, const param_type& __p)
  {
    double u = std::generate_canonical<double, std::numeric_limits<double>::digits, _UniformRandomNumberGenerator>(__urng);
    unsigned long N = __p.b() - __p.a() + 1;
    return __p.a() + (N *
                std::pow(u, std::log(__p.skew()) / std::log(1.0-__p.skew())));
  }

  /**
   * @brief Return true if two selfsimilar int distributions have
   *        the same parameters.
   */
  friend bool operator==(const selfsimilar_int_distribution& __d1, const selfsimilar_int_distribution& __d2)
  { return __d1._M_param == __d2._M_param; }

  private:
  param_type _M_param;
};