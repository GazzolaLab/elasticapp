# Distributed under the MIT License.
# See LICENSE.txt for details.

import math
import unittest

import numpy as np
import numpy.testing as npt
from elasticapp.Arrays import IndexVector


class TestIndexVector(unittest.TestCase):
    def test_len(self):
        a = IndexVector(5, 6)
        self.assertEqual(len(a), 5)

    def test_bounds_check(self):
        a = IndexVector(5, 6)
        self.assertRaises(RuntimeError, lambda: a[5])

        def assignment_test():
            a[5] = 8

        self.assertRaises(RuntimeError, assignment_test)

    def test_output(self):
        a = IndexVector(5, 7)
        self.assertEqual(str(a), "(7,7,7,7,7)")
        b = IndexVector([3, 7])
        self.assertEqual(str(b), "(3,7)")

    def test_numpy_compatibility(self):
        # !
        b = np.array([1, 2, 3], dtype=np.ulonglong)
        c = IndexVector([1, 2, 3])
        self.assertTrue(((b + c) == np.array([2, 4, 6])).all())
        # Convert a IndexVector to a Numpy array
        c_array_copy = np.array(c)
        npt.assert_equal(c_array_copy, b)
        # Changing the copy shouldn't change the IndexVector
        c_array_copy[2] = 4
        npt.assert_equal(c, b)
        c_array_reference = np.array(c, copy=False)
        npt.assert_equal(c_array_reference, b)
        # Changing the reference should change the IndexVector as well
        c_array_reference[2] = 4
        self.assertEqual(c[2], 4)
        # Convert a Numpy array to a IndexVector
        b_dv_copy = IndexVector(b)
        self.assertEqual(b_dv_copy, IndexVector([1, 2, 3]))
        b_dv_copy[2] = 4
        self.assertEqual(b[2], 3)

    def test_iterator(self):
        a = IndexVector([1, 2, 3, 4, 5])
        for i, val in enumerate(a):
            self.assertEqual(val, a[i])

    def test_size_constructor(self):
        a = IndexVector(3)
        self.assertEqual(len(a), 3)

    def test_list_constructor(self):
        a = IndexVector([1, 2, 3, 4, 5])
        self.assertEqual(len(a), 5)
        self.assertEqual(a[3], 4)


if __name__ == "__main__":
    unittest.main(verbosity=2)
