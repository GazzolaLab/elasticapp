# Distributed under the MIT License.
# See LICENSE.txt for details.

import math
import unittest

import numpy as np
import numpy.testing as npt
from elasticapp.Arrays import Vector


class TestVector(unittest.TestCase):
    def test_len(self):
        a = Vector(5, 6.7)
        self.assertEqual(len(a), 5)

    def test_math_double(self):
        a = Vector(5, 6.7)
        self.assertEqual(a + 2.3, Vector(5, 9.0))
        self.assertEqual(2.3 + a, Vector(5, 9.0))
        self.assertEqual(a - 2.3, Vector(5, 4.4))
        self.assertEqual(2.3 - a, Vector(5, -4.4))
        self.assertEqual(2.0 * a, Vector(5, 2.0 * 6.7))
        self.assertEqual(a * 2.0, Vector(5, 2.0 * 6.7))
        self.assertEqual(a / 2.0, Vector(5, 6.7 / 2.0))
        self.assertEqual(2.0 / a, Vector(5, 2.0 / 6.7))
        self.assertEqual(a**2.0, Vector(5, 6.7**2.0))

        b = Vector([7.2, -3.9])
        self.assertEqual((b + 2.3)[0], 9.5)
        self.assertEqual((b + 2.3)[1], -1.6)
        self.assertEqual((2.3 + b)[0], 9.5)
        self.assertEqual((2.3 + b)[1], -1.6)

        self.assertEqual((b - 2.3)[0], 4.9)
        self.assertAlmostEqual((b - 2.3)[1], -6.2)
        self.assertEqual((2.3 - b)[0], -4.9)
        self.assertAlmostEqual((2.3 - b)[1], 6.2)

        self.assertEqual((2.0 * b)[0], 14.4)
        self.assertEqual((2.0 * b)[1], -7.8)
        self.assertEqual((b * 2.0)[0], 14.4)
        self.assertEqual((b * 2.0)[1], -7.8)

        self.assertEqual((2.0 / b)[0], 0.2777777777777778)
        self.assertEqual((2.0 / b)[1], -0.5128205128205129)
        self.assertEqual((b / 2.0)[0], 3.6)
        self.assertEqual((b / 2.0)[1], -1.95)

    # def test_math_vector(self):
    #     a = Vector(5, 6.7)
    #     b = Vector(5, 8.7)
    #     self.assertEqual(a + b, Vector(5, 6.7 + 8.7))
    #     self.assertEqual(a - b, Vector(5, 6.7 - 8.7))
    #     self.assertEqual(a * b, Vector(5, 6.7 * 8.7))
    #     self.assertEqual(a / b, Vector(5, 6.7 / 8.7))
    #
    #     c = Vector([1.5, 2.5])
    #     d = Vector([3.0, 7.5])
    #     self.assertEqual((c + d)[0], 4.5)
    #     self.assertEqual((c + d)[1], 10.0)
    #     self.assertEqual((c - d)[0], -1.5)
    #     self.assertEqual((c - d)[1], -5.0)
    #     self.assertEqual((c * d)[0], 4.5)
    #     self.assertEqual((c * d)[1], 18.75)
    #     self.assertEqual((c / d)[0], 0.5)
    #     self.assertEqual((c / d)[1], 1.0 / 3.0)

    def test_negate(self):
        a = Vector(5, 6.7)
        self.assertEqual(-a, Vector(5, -6.7))
        b = Vector([1.5, 3.7])
        self.assertEqual((-b)[0], -1.5)
        self.assertEqual((-b)[1], -3.7)

    def test_bounds_check(self):
        a = Vector(5, 6.7)
        self.assertRaises(RuntimeError, lambda: a[5])

        def assignment_test():
            a[5] = 8

        self.assertRaises(RuntimeError, assignment_test)

    def test_output(self):
        a = Vector(5, 6.7)
        self.assertEqual(str(a), "(6.7,6.7,6.7,6.7,6.7)")
        b = Vector([3.8, 7.2])
        self.assertEqual(str(b), "(3.8,7.2)")

    def test_abs(self):
        a = Vector(5, -6.0)
        self.assertEqual(a.abs(), Vector(5, 6.0))
        b = Vector([1.0, -2.0])
        self.assertEqual(b.abs()[0], 1.0)
        self.assertEqual(b.abs()[1], 2.0)

    def test_acos(self):
        a = Vector(5, 0.67)
        self.assertEqual(a.acos()[0], math.acos(0.67))
        b = Vector([1.0, -0.5])
        self.assertEqual(b.acos()[0], 0)
        self.assertEqual(b.acos()[1], math.acos(-0.5))

    def test_acosh(self):
        a = Vector(5, 1.0)
        self.assertEqual(a.acosh(), Vector(5, math.acosh(1.0)))
        b = Vector([1.0, 2.0])
        self.assertEqual(b.acosh()[0], 0.0)
        self.assertEqual(b.acosh()[1], math.acosh(2.0))

    def test_asin(self):
        a = Vector(5, 0.67)
        self.assertEqual(a.asin(), Vector(5, math.asin(0.67)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.asin()[0], 0.0)
        self.assertEqual(b.asin()[1], math.asin(-0.5))

    def test_asinh(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.sinh(), Vector(5, math.sinh(4.0)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.asinh()[0], 0.0)
        self.assertEqual(b.asinh()[1], math.asinh(-0.5))

    def test_atan(self):
        a = Vector(5, 0.67)
        self.assertEqual(a.atan(), Vector(5, math.atan(0.67)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.atan()[0], 0.0)
        self.assertEqual(b.atan()[1], math.atan(-0.5))

    def test_atan2(self):
        a = Vector(5, 0.67)
        b = Vector(5, -4.0)
        self.assertEqual(a.atan2(b), Vector(5, math.atan2(0.67, -4.0)))
        x = Vector([3, 100])
        y = Vector([1, -3])
        z = Vector([math.atan2(3, 1), math.atan2(100, -3)])
        self.assertEqual(x.atan2(y), z)

    def test_atanh(self):
        a = Vector(5, 0.67)
        self.assertEqual(a.atanh(), Vector(5, math.atanh(0.67)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.atanh()[0], 0.0)
        self.assertEqual(b.atanh()[1], math.atanh(-0.5))

    def test_cbrt(self):
        a = Vector(5, 8.0)
        self.assertEqual(a.cbrt(), Vector(5, 2.0))
        b = Vector([1.0, 64.0])
        self.assertEqual(b.cbrt()[0], 1.0)
        self.assertEqual(b.cbrt()[1], 4.0)

    def test_cos(self):
        a = Vector(5, 0.67)
        self.assertEqual(a.cos(), Vector(5, math.cos(0.67)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.cos()[0], 1.0)
        self.assertEqual(b.cos()[1], math.cos(-0.5))

    def test_cosh(self):
        a = Vector(5, 1.0)
        self.assertEqual(a.cosh(), Vector(5, math.cosh(1.0)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.cosh()[0], 1.0)
        self.assertEqual(b.cosh()[1], math.cosh(-0.5))

    def test_erf(self):
        a = Vector(5, 1.0)
        self.assertAlmostEqual(a.erf()[0], math.erf(1.0))
        b = Vector([2.0, 3.0])
        self.assertAlmostEqual(b.erf()[0], math.erf(2.0))
        self.assertAlmostEqual(b.erf()[1], math.erf(3.0))

    def test_erfc(self):
        a = Vector(5, 1)
        self.assertAlmostEqual(a.erfc()[0], math.erfc(1.0))
        b = Vector([2.0, 3.0])
        self.assertAlmostEqual(b.erfc()[0], math.erfc(2.0))
        self.assertAlmostEqual(b.erfc()[1], math.erfc(3.0))

    def test_exp(self):
        a = Vector(5, 1.0)
        self.assertEqual(a.exp(), Vector(5, math.exp(1.0)))
        b = Vector([2.0, -0.5])
        self.assertEqual(b.exp()[0], math.exp(2.0))
        self.assertEqual(b.exp()[1], math.exp(-0.5))

    def test_exp2(self):
        a = Vector(5, 1.0)
        self.assertEqual(a.exp2(), Vector(5, 2.0))
        b = Vector([2.0, -0.5])
        self.assertAlmostEqual(b.exp2()[0], 4.0)
        self.assertAlmostEqual(b.exp2()[1], 1.0 / math.sqrt(2.0))

    def test_exp10(self):
        a = Vector(5, 1.0)
        self.assertEqual(a.exp10(), Vector(5, 10.0))
        b = Vector([2.0, -0.5])
        self.assertEqual(b.exp10()[0], 100.0)
        self.assertEqual(b.exp10()[1], 1.0 / math.sqrt(10.0))

    # def test_fabs(self):
    #     a = Vector(5, -0.67)
    #     self.assertEqual(a.fabs(), Vector(5, 0.67))
    #     b = Vector([2.0, -0.5])
    #     self.assertEqual(b.fabs()[0], 2.0)
    #     self.assertEqual(b.fabs()[1], 0.5)

    def test_hypot(self):
        a = Vector(5, 3.0)
        b = Vector(5, 4.0)
        self.assertEqual(a.hypot(b)[0], 5.0)
        x = Vector([0.0, 3.0])
        y = Vector([5.0, 4.0])
        self.assertEqual(x.hypot(y), Vector(2, 5.0))

    def test_invcbrt(self):
        a = Vector(5, 8.0)
        self.assertEqual(a.invcbrt(), Vector(5, 0.5))
        b = Vector([1.0, -64.0])
        self.assertEqual(b.invcbrt()[0], 1.0)
        self.assertEqual(b.invcbrt()[1], -0.25)

    def test_invsqrt(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.invsqrt(), Vector(5, 0.5))
        b = Vector([1.0, 16.0])
        self.assertEqual(b.invsqrt()[0], 1.0)
        self.assertEqual(b.invsqrt()[1], 0.25)

    def test_log(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.log(), Vector(5, math.log(4.0)))
        b = Vector([1.0, 0.5])
        self.assertEqual(b.log()[0], 0.0)
        self.assertEqual(b.log()[1], math.log(0.5))

    def test_log2(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.log2(), Vector(5, 2.0))
        b = Vector([1.0, 0.5])
        self.assertEqual(b.log2()[0], 0.0)
        self.assertEqual(b.log2()[1], -1.0)

    def test_log10(self):
        a = Vector(5, 100.0)
        self.assertEqual(a.log10(), Vector(5, 2.0))
        b = Vector([1.0, 0.1])
        self.assertEqual(b.log10()[0], 0.0)
        self.assertEqual(b.log10()[1], -1.0)

    def test_max(self):
        a = Vector([4.0, 5.0, 4.0, 4.0, 4.0])
        self.assertEqual(a.max(), 5.0)
        b = Vector([1.0, 0.5])
        self.assertEqual(b.max(), 1.0)

    def test_min(self):
        a = Vector([4.0, 2.0, 4.0, 4.0, 4.0])
        self.assertEqual(a.min(), 2.0)
        b = Vector([1.0, -0.5])
        self.assertEqual(b.min(), -0.5)

    def test_pow(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.pow(2), Vector(5, 16.0))
        b = Vector([1.0, -0.5])
        self.assertEqual(b.pow(2)[0], 1.0)
        self.assertEqual(b.pow(2)[1], 0.25)

    def test_sin(self):
        a = Vector(5, 6.7)
        self.assertEqual(a.sin(), Vector(5, math.sin(6.7)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.sin()[0], 0.0)
        self.assertEqual(b.sin()[1], math.sin(-0.5))

    def test_sinh(self):
        a = Vector(5, 3.0)
        self.assertEqual(a.sinh(), Vector(5, math.sinh(3.0)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.sinh()[0], 0.0)
        self.assertEqual(b.sinh()[1], math.sinh(-0.5))

    def test_sqrt(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.sqrt(), Vector(5, 2.0))
        b = Vector([1.0, 0.25])
        self.assertEqual(b.sqrt()[0], 1.0)
        self.assertEqual(b.sqrt()[1], 0.5)

    def test_tan(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.tan(), Vector(5, math.tan(4.0)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.tan()[0], 0.0)
        self.assertEqual(b.tan()[1], math.tan(-0.5))

    def test_tanh(self):
        a = Vector(5, 4.0)
        self.assertEqual(a.tanh(), Vector(5, math.tanh(4.0)))
        b = Vector([0.0, -0.5])
        self.assertEqual(b.tanh()[0], 0.0)
        self.assertEqual(b.tanh()[1], math.tanh(-0.5))

    def test_numpy_compatibility(self):
        b = np.array([1.0, 2.0, 3.0])
        c = Vector([1.0, 2.0, 3.0])
        self.assertTrue(((b + c) == np.array([2.0, 4.0, 6.0])).all())
        x = np.linspace(0, 2 * np.pi, 10)
        self.assertTrue((Vector(list(x)).sin() == np.sin(x)).all())
        # Convert a Vector to a Numpy array
        c_array_copy = np.array(c)
        npt.assert_equal(c_array_copy, b)
        # Changing the copy shouldn't change the Vector
        c_array_copy[2] = 4.0
        npt.assert_equal(c, b)
        c_array_reference = np.array(c, copy=False)
        npt.assert_equal(c_array_reference, b)
        # Changing the reference should change the Vector as well
        c_array_reference[2] = 4.0
        self.assertEqual(c[2], 4.0)
        # Convert a Numpy array to a Vector
        b_dv_copy = Vector(b)
        self.assertEqual(b_dv_copy, Vector([1.0, 2.0, 3.0]))
        b_dv_copy[2] = 4.0
        self.assertEqual(b[2], 3.0)

    def test_iterator(self):
        a = Vector([1.0, 2.0, 3.0, 4.0, 5.0])
        for i, val in enumerate(a):
            self.assertEqual(val, a[i])

    def test_size_constructor(self):
        a = Vector(3)
        self.assertEqual(len(a), 3)

    def test_list_constructor(self):
        a = Vector([1.0, 2.0, 3.0, 4.0, 5.0])
        self.assertEqual(len(a), 5)
        self.assertEqual(a[3], 4.0)


if __name__ == "__main__":
    unittest.main(verbosity=2)
