from typing import Dict
import pytest


class MockHook:
    def __init__(self, config: Dict):
        self.config = config

    # Empty hook, just for testing
    def execute(self) -> float:
        return self.config["block_size"] + self.config["num_threads"]
