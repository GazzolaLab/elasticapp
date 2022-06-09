# Distributed under the MIT License.
# See LICENSE.txt for details.

import unittest

import numpy as np
import numpy.testing as npt
from elasticapp.Arrays import IndexVector, IndexVectorView  # noqa


class TestIndexVectorView(unittest.TestCase):
    def test_len(self):
        a = IndexVector(5, 67)
        av = a[1:3]
        self.assertEqual(len(av), 2)

    def test_shape(self):
        a = IndexVector(5, 67)
        av = a[1:3]
        self.assertEqual(av.shape, (2,))

    def test_output(self):
        a = IndexVector(5, 67)
        av = a[1:3]
        self.assertEqual(str(av), "(67,67)")
        b = IndexVector([38, 72, 114])
        bv = b[1:]
        self.assertEqual(str(bv), "(72,114)")

    def test_getitem(self):
        a = IndexVector(5, 42)
        av = a[:-1]  # fro
        av[0] = 3
        av[1] = 0
        av[2] = 1
        # it should reflect in the original array
        self.assertEqual(a[0], 3)
        self.assertEqual(a[1], 0)
        self.assertEqual(a[2], 1)
        self.assertEqual(a[3], 42)
        self.assertEqual(a[4], 42)

    def test_bounds_check(self):
        a = IndexVector(5, 67)
        av = a[1:4]
        self.assertRaises(RuntimeError, lambda: av[3])

        def assignment_test():
            av[3] = 8

        self.assertRaises(RuntimeError, assignment_test)

    def test_numpy_compatibility(self):
        b = np.array([1, 2, 3])
        c = IndexVector([0, 1, 2, 3, 4])
        cv = c[1:4]
        self.assertTrue(((b + cv) == np.array([2, 4, 6])).all())

        # Convert a IndexVectorView to a Numpy array
        c_array_copy = np.array(cv)
        npt.assert_equal(c_array_copy, b)

        # Changing the copy shouldn't change the IndexVectorView
        c_array_copy[2] = 4
        npt.assert_equal(cv, b)

        c_array_reference = np.array(cv, copy=False)
        npt.assert_equal(c_array_reference, b)

        # Changing the reference should change the IndexVectorView as well
        c_array_reference[2] = 4
        self.assertEqual(cv[2], 4)
        # Changing the reference should change the original IndexVector as well
        self.assertEqual(c[3], 4)

    def test_view(self):
        a = IndexVector(5, 67)
        av = a[1:4]
        self.assertEqual(len(av), 3)

        avv = av[1:]
        self.assertEqual(len(avv), 2)

        avv[0] = 3
        avv[1] = 0
        # it should reflect in the original array
        self.assertEqual(avv[0], 3)
        self.assertEqual(avv[1], 0)
        self.assertEqual(av[1], 3)
        self.assertEqual(av[2], 0)
        self.assertEqual(a[2], 3)
        self.assertEqual(a[3], 0)


if __name__ == "__main__":
    unittest.main(verbosity=2)
