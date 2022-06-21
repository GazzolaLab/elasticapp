# Distributed under the MIT License.
# See LICENSE.txt for details.

import unittest

import numpy as np
from elasticapp.Arrays import Tensor


class TestTensor(unittest.TestCase):
    def test_shape(self):
        M = Tensor(3, 5, 4)
        self.assertEqual(M.shape, (3, 5, 4))

    def test_getitem(self):
        M = Tensor(3, 4, 5)
        M[1, 2, 0] = 3
        M[0, 0, 1] = 0
        M[2, 1, 2] = 1
        self.assertEqual(M[1, 2, 0], 3)
        self.assertEqual(M[0, 0, 1], 0)
        self.assertEqual(M[2, 1, 2], 1)

    def test_string(self):
        M = Tensor(2, 2, 2)
        M[0, 0, 0] = 1
        M[0, 0, 1] = 2
        M[0, 1, 0] = 3
        M[0, 1, 1] = 4
        M[1, 0, 0] = 5
        M[1, 0, 1] = 6
        M[1, 1, 0] = 7
        M[1, 1, 1] = 8
        self.assertEqual(
            str(M),
            "((           1            2 ) (           3            4 ) )\n((           5            6 ) (           7            8 ) )\n",
        )

    def test_bounds_check(self):
        a = Tensor(2, 3, 2)
        self.assertRaises(RuntimeError, lambda: a[1, 2, 4])

        def assignment_test():
            a[1, 2, 4] = 8

        self.assertRaises(RuntimeError, assignment_test)

    def test_numpy_interoperability(self):
        M = Tensor(1, 2, 2)
        M[0, 0, 0] = 1.0
        M[0, 1, 0] = 2.7
        M[0, 0, 1] = 5.42
        M[0, 1, 1] = -2.3
        # Convert Tensor to Numpy array
        A = np.array(M)
        self.assertTrue(isinstance(A, np.ndarray))
        self.assertEqual(A[0, 0, 0], 1.0)
        self.assertEqual(A[0, 1, 0], 2.7)
        self.assertEqual(A[0, 0, 1], 5.42)
        self.assertEqual(A[0, 1, 1], -2.3)
        # Convert Numpy array to Tensor
        M_from_array = Tensor(A)
        self.assertEqual(M_from_array[0, 0, 0], 1.0)
        self.assertEqual(M_from_array[0, 1, 0], 2.7)
        self.assertEqual(M_from_array[0, 0, 1], 5.42)
        self.assertEqual(M_from_array[0, 1, 1], -2.3)


if __name__ == "__main__":
    unittest.main(verbosity=2)
