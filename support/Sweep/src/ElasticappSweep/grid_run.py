__doc__ = """
Run parameter sweep for block-size and block-policy.
"""

from typing import Protocol
from typing import Dict, Any, Optional

import os
import sys
import multiprocessing

import ray
from ray import tune
from ray.tune.schedulers import ASHAScheduler
from ray.tune.suggest.hyperopt import HyperOptSearch

import numpy as np
import matplotlib.pyplot as plt

import time
from functools import partial

import yaml

from ElasticappSweep.hook import ElasticappHook, HookProtocol


def runner(config: Dict, Hook: HookProtocol, hook_kwargs: Optional[Dict] = None):
    """
    Function Runner

    Parameters
    ----------
    config : Dict
        Parameter configuration in dictionary
    """

    hook: HookProtocol = Hook(config, **hook_kwargs)
    result: Dict[str, float] = hook()
    # tune.report(objective=objective)
    return result


def execute(
    search_space, input_path: str, execute_name: str, save_path: Optional[str] = None, name=None
):
    # Configuration
    save_path = save_path or "./ray_results"

    # Run Configuration
    # ray.init(num_cpus=64, num_gpus=0) # Limit number of cpu and gpu
    ray.init(local_mode=True)  # Serial Mode
    #ray.init()  # Serial Mode

    # Batch Run
    # metric = "objective"
    # mode = "min" # Minimize objective.
    # ashas_scheduler = ASHAScheduler(metric=metric, mode=mode)
    # hyperopt_search = HyperOptSearch(space, metric=metric, mode=mode)
    tune.run(
        partial(
            runner,
            Hook=ElasticappHook,
            hook_kwargs=dict(execute_name=execute_name, input_path=input_path),
        ),
        config=search_space,
        num_samples=1,
        # scheduler=ashas_scheduler,
        # search_alg=hyperopt_search,
        local_dir=save_path,
        verbose=2,
        name=name,
        max_concurrent_trials=1,
    )
