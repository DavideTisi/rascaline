# -*- coding: utf-8 -*-
import unittest
import numpy as np

import rascaline
from rascaline import Descriptor
from rascaline.calculator import DummyCalculator

from test_systems import TestSystem


class TestEmptyDescriptor(unittest.TestCase):
    def test_values(self):
        descriptor = Descriptor()
        self.assertEqual(descriptor.values.shape, (0, 0))

    def test_gradients(self):
        descriptor = Descriptor()
        self.assertEqual(descriptor.gradients.shape, (0, 0))

    def test_environments(self):
        descriptor = Descriptor()
        self.assertEqual(len(descriptor.environments), 0)

    def test_features(self):
        descriptor = Descriptor()
        self.assertEqual(len(descriptor.features), 0)

    def test_gradients_environments(self):
        descriptor = Descriptor()
        self.assertEqual(len(descriptor.gradients_environments), 0)


class TestDummyDescriptor(unittest.TestCase):
    def test_values(self):
        system = TestSystem()
        calculator = DummyCalculator(cutoff=3.2, delta=12, name="", gradients=False)
        descriptor = calculator.compute(system)

        values = descriptor.values
        self.assertEqual(values.shape, (4, 2))
        self.assertTrue(np.all(values[0] == (12, 0)))
        self.assertTrue(np.all(values[1] == (13, 1)))
        self.assertTrue(np.all(values[2] == (14, 2)))
        self.assertTrue(np.all(values[3] == (15, 3)))

    def test_gradients(self):
        system = TestSystem()
        calculator = DummyCalculator(cutoff=3.2, delta=12, name="", gradients=False)
        descriptor = calculator.compute(system)
        self.assertEqual(descriptor.gradients.shape, (0, 0))

        calculator = DummyCalculator(cutoff=3.2, delta=12, name="", gradients=True)
        descriptor = calculator.compute(system)
        gradients = descriptor.gradients
        self.assertEqual(gradients.shape, (18, 2))
        for i in range(18):
            self.assertTrue(np.all(gradients[i] == (0, 1)))

    def test_environments(self):
        system = TestSystem()
        calculator = DummyCalculator(cutoff=3.2, delta=12, name="", gradients=False)
        descriptor = calculator.compute(system)

        environments = descriptor.environments
        self.assertEqual(len(environments), 4)
        self.assertEqual(environments[0], (0, 0))
        self.assertEqual(environments[1], (0, 1))
        self.assertEqual(environments[2], (0, 2))
        self.assertEqual(environments[3], (0, 3))

    def test_gradient_indexes(self):
        system = TestSystem()
        calculator = DummyCalculator(cutoff=3.2, delta=12, name="", gradients=False)
        descriptor = calculator.compute(system)
        self.assertEqual(len(descriptor.gradients_environments), 0)

        calculator = DummyCalculator(cutoff=3.2, delta=12, name="", gradients=True)
        descriptor = calculator.compute(system)
        gradients_environments = descriptor.gradients_environments
        self.assertEqual(len(gradients_environments), 18)
        self.assertEqual(gradients_environments[0], (0, 0, 1, 0))
        self.assertEqual(gradients_environments[1], (0, 0, 1, 1))
        self.assertEqual(gradients_environments[2], (0, 0, 1, 2))

        self.assertEqual(gradients_environments[3], (0, 1, 0, 0))
        self.assertEqual(gradients_environments[4], (0, 1, 0, 1))
        self.assertEqual(gradients_environments[5], (0, 1, 0, 2))

        self.assertEqual(gradients_environments[6], (0, 1, 2, 0))
        self.assertEqual(gradients_environments[7], (0, 1, 2, 1))
        self.assertEqual(gradients_environments[8], (0, 1, 2, 2))

        self.assertEqual(gradients_environments[9], (0, 2, 1, 0))
        self.assertEqual(gradients_environments[10], (0, 2, 1, 1))
        self.assertEqual(gradients_environments[11], (0, 2, 1, 2))

        self.assertEqual(gradients_environments[12], (0, 2, 3, 0))
        self.assertEqual(gradients_environments[13], (0, 2, 3, 1))
        self.assertEqual(gradients_environments[14], (0, 2, 3, 2))

        self.assertEqual(gradients_environments[15], (0, 3, 2, 0))
        self.assertEqual(gradients_environments[16], (0, 3, 2, 1))
        self.assertEqual(gradients_environments[17], (0, 3, 2, 2))

    def test_features(self):
        system = TestSystem()
        calculator = DummyCalculator(cutoff=3.2, delta=12, name="", gradients=False)
        descriptor = calculator.compute(system)

        features = descriptor.features
        self.assertEqual(len(features), 2)
        self.assertEqual(features[0], (1, 0))
        self.assertEqual(features[1], (0, 1))