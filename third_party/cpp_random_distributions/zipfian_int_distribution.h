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
 * The zipfian_int_distribution class is intended to be compatible with other
 * distributions introduced in #include <random> by the C++11 standard.
 * 
 * Usage example:
 * #include <random>
 * #include "zipfian_int_distribution.h"
 * int main()
 * {
 *   std::default_random_engine generator;
 *   zipfian_int_distribution<int> distribution(1, 10, 0.99);
 *   int i = distribution(generator);
 * }
 */

/*
 * IMPORTANT: constructing the distribution object requires calculating the zeta
 * value which becomes prohibetively expensive for very large ranges. As an
 * alternative for such cases, the user can pass the pre-calculated values and
 * avoid the calculation every time.
 * 
 * Usage example:
 * #include <random>
 * #include "zipfian_int_distribution.h"
 * int main()
 * {
 *   std::default_random_engine generator;
 *   zipfian_int_distribution<int>::param_type p(1, 1e6, 0.99, 27.000);
 *   zipfian_int_distribution<int> distribution(p);
 *   int i = distribution(generator);
 * }
 */

#include <cmath>
#include <limits>
#include <random>
#include <cassert>

template<typename _IntType = int>
class zipfian_int_distribution
{
  static_assert(std::is_integral<_IntType>::value, "Template argument not an integral type.");

public:
  /** The type of the range of the distribution. */
  typedef _IntType result_type;
  /** Parameter type. */
  struct param_type
  {
    typedef zipfian_int_distribution<_IntType> distribution_type;

    explicit param_type(_IntType __a = 0, _IntType __b = std::numeric_limits<_IntType>::max(), double __theta = 0.99)
    : _M_a(__a), _M_b(__b), _M_theta(__theta),
      _M_zeta(zeta(_M_b - _M_a + 1, __theta)), _M_zeta2theta(zeta(2, __theta))
    {
      assert(_M_a <= _M_b && _M_theta > 0.0 && _M_theta < 1.0);
    }
    
    explicit param_type(_IntType __a, _IntType __b, double __theta, double __zeta)
    : _M_a(__a), _M_b(__b), _M_theta(__theta), _M_zeta(__zeta),
    _M_zeta2theta(zeta(2, __theta))
    {
      __glibcxx_assert(_M_a <= _M_b && _M_theta > 0.0 && _M_theta < 1.0);
    }

    result_type	a() const { return _M_a; }

    result_type	b() const { return _M_b; }

    double theta() const { return _M_theta; }

    double zeta() const { return _M_zeta; }

    double zeta2theta() const { return _M_zeta2theta; }

    friend bool	operator==(const param_type& __p1, const param_type& __p2)
    {
      return __p1._M_a == __p2._M_a
          && __p1._M_b == __p2._M_b
          && __p1._M_theta == __p2._M_theta
          && __p1._M_zeta == __p2._M_zeta
          && __p1._M_zeta2theta == __p2._M_zeta2theta;
    }

  private:
    _IntType _M_a;
    _IntType _M_b;
    double _M_theta;
    double _M_zeta;
    double _M_zeta2theta;

    /**
     * @brief Calculates zeta.
     *
     * @param __n [IN]  The size of the domain.
     * @param __theta [IN]  The skew factor of the distribution.
     */
    double zeta(unsigned long __n, double __theta)
    {
      double ans = 0.0;
      for(unsigned long i=1; i<=__n; ++i)
        ans += std::pow(1.0/i, __theta);
      return ans;
    }
  };

public:
  /**
   * @brief Constructs a zipfian_int_distribution object.
   *
   * @param __a [IN]  The lower bound of the distribution.
   * @param __b [IN]  The upper bound of the distribution.
   * @param __theta [IN]  The skew factor of the distribution.
   */
  explicit zipfian_int_distribution(_IntType __a = _IntType(0), _IntType __b = _IntType(1), double __theta = 0.99)
  : _M_param(__a, __b, __theta)
  { }

  explicit zipfian_int_distribution(const param_type& __p) : _M_param(__p)
  { }

  /**
   * @brief Resets the distribution state.
   *
   * Does nothing for the zipfian int distribution.
   */
  void reset() { }

  result_type a() const { return _M_param.a(); }

  result_type b() const { return _M_param.b(); }

  double theta() const { return _M_param.theta(); }

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
    double alpha = 1 / (1 - __p.theta());
    double eta = (1 - std::pow(2.0 / (__p.b() - __p.a() + 1), 1 - __p.theta())) / (1 - __p.zeta2theta() / __p.zeta());
      
    double u = std::generate_canonical<double, std::numeric_limits<double>::digits, _UniformRandomNumberGenerator>(__urng);
      
    double uz = u * __p.zeta();
    if(uz < 1.0) return __p.a();
    if(uz < 1.0 + std::pow(0.5, __p.theta())) return __p.a() + 1;

    return __p.a() + ((__p.b() - __p.a() + 1) * std::pow(eta*u-eta+1, alpha));
  }

  /**
   * @brief Return true if two zipfian int distributions have
   *        the same parameters.
   */
  friend bool operator==(const zipfian_int_distribution& __d1, const zipfian_int_distribution& __d2)
  { return __d1._M_param == __d2._M_param; }

  private:
  param_type _M_param;
};
