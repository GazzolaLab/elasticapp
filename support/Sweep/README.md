# ElasticappSweep: Tune Elastica++ for Scaling

The purpose of `ElasticappSweep` is to find optimal setting to run `Elastica++` in different environment. It utilize `ray[tune]` library for parameter sweep and data collection.

> This tool will be refactored into separate repository in the future.

## Pipeline

```mermaid
graph LR
S{"ElasticappSweep<br/>ray[tune]"}
e("tune.ExperimentAnalysis")
t("TensorboardX")
p("pd.Dataframe")
E[Elastica++] -- Generate Case Files --> A((Executable<br/>case file))
B((default.YAML)) --> S
A --> S
S --> D[(Database<br/>./ray_results)]
D --> c(Analysis)
e --> c
t --> c
p --> c
```

1. `ElasticappSweep` will first collect default configuration/parameters for `Elastica++` execution file, and generate set of configuration-YAML files with the parameter that needs to be sweeped.
    - __Grid-search__ for testing different `block size` and `block policy`.
    - __Scaling__ for testing different `number of elements` and `number of rods`.
    > This configuration-YAML file will be stored along with the output directory, which is entirely managed by `ray[tune]`.
2. The `Elastica++` program will run within each directory with each configuration-YAML.
    - Output files will also be saved in each directory.
    - For each run, an objective (i.e. run time) is saved as a result.
    - Scripts we provide will run each "Trials" serially, because we are measuring the benchmark performance.
        - If want to convert to parallel optimization, you can call `ray.init(num_cpus:int, num_gpus:int)`.
    - Scripts we provided does `grid-search`: _run every combination of sweeping variable_. If wants to convert the algorithm to do `search`: _optimization_, use schedular and search algorithm in `ray[tune]`. [ref](https://docs.ray.io/en/latest/tune/key-concepts.html#search-spaces)
    - To setup non-MPI distributed strategy, checkout [here](https://docs.ray.io/en/latest/tune/tutorials/tune-distributed.html).
    - To setup MPI distributed strategy, checkout [here](https://docs.ray.io/en/latest/cluster/slurm.html).
3. The result will be stored in the directory `./ray_results`.
    - To view the result, user can load the entire directory into `Tensorboard`.
    - To read the result as dataframe, use [`tune.ExperimentAnalysis`](https://docs.ray.io/en/latest/tune/api_docs/analysis.html#experimentanalysis-tune-experimentanalysis) module.
4. Output file structure (example)
    > Directory and file name can be customized by setting `trial_name_creator` and `trial_dirname_creator`. [ref](https://docs.ray.io/en/latest/tune/api_docs/execution.html#tune-run)
```sh
ray_results
└── runner_2022-08-16_17-05-32
    ├── basic-variant-state-2022-08-16_17-05-33.json
    ├── experiment_state-2022-08-16_17-05-33.json         # Experiment state file in dataframe
    ├── runner_8bd84_00000_0_block_policy=policy1,block_size=1_2022-08-16_17-05-33
    │   ├── events.out.tfevents.1660687533  # Tensorboard data file
    │   ├── output.yml                      # Output from Elastica++
    │   ├── params.json                     # Sweep Parameter
    │   ├── params.pkl
    │   ├── progress.csv                    # Current progress (e.g. last objective values, loss, etc.)
    │   └── result.json                     # Result (row of the dataframe)
    ├─── runner_8bd84_00001_1_block_policy=policy2,block_size=1_2022-08-16_17-05-35
    │    ├── events.out.tfevents.1660687535
    │    ├── output.yml
    │    ├── params.json
    │    ├── params.pkl
    │    ├── progress.csv
    │    └── result.json
   ...

3 directories, 14 files
```
     
    
## Analysis (Examples)

### Tensorboard

> Installing `ray[tune]` should also install `TensorboardX`. If not, you can install it using `pip`.

```
tensorboard --logdir=./ray_results
```

<img src="https://dl.boxcloud.com/d/1/b1!1Bx0FpbCY_P3lord1aKta2NuTE5431K24UrUqvmDugqPaC8sL4jp-AcMwSi2CrnmlByJRyN--Iix_FwRsXwCRwqamiiKS2PbQF6LfvxWZwzcyWmicuMJ0yUPbSsQ9567vKvrkTMWXwiC83dx5F7jM7ShGWG4XdF-gzpugd7vfyDah2kpf2HUejiTydVMQjW0gZ0gvxus9NMz09LIY5Wq5-joLUQRqlZhtyv6DUWXC9jvlrQR140Q-pR1a6lJVvEoaob7AX1W11qWy6pRhCZdfAo9VYd-MipJUa-VitnL6-2M-eTw9nzJjeOkeaeGD-2cMWsRDAmiEgChiS5JF_-f7o6FgY8yJcZx-4OmOGQRH3nK3EuBL5z9HfGllmQpuftTzTkgMe_nRvlaR8VD0kyF9kHQt-gDzCO_puy5Exg3dCKjV9joyGwzJKAnm9DEcT33AesU2NjxtzAVqHPRm7VHAW6yOkKcutK2lyH9UrylfyM73Vc2KF5SUiHWd_JFkuggfYstVeCqjsLkKLS-ZShZAbIctYMKiUA534ICmZLoySKJ0_EFDYcvByA4dCalsQ5HiBEQqx-jwYQHYD7WFqJj12Hu83gpy0j0Cyjgz4fJnkvzAJEiRVy9sWQjRB9j_ATEZEFhiLq3nMawuHa3ezgtQjPGtfejln_To_BHvFJ_EzN_xcnaF9vRnn-r_kyDW5qq2KFgu26xRvNFbn0_CYen8UUUU5ilxJpVcJ7QQhYK-gB5qeRIqap-jg1Ljc3do4DVMCNPm2_qAEUmJp-SYw4E_L-XgEbKWRJBd6phSdyP4jT7RqLjLgkQJBNoYVqbyTJ2pzgvat0ptVqexzsmT6W1ok5y5z_jTFLQbluSIXbgSZf6z3RI6HwMzfsvCHW0hZ7j2gJkM4TUwPYtM3DV8sfZJ_t2vk8lHktlG1Hens878y17rx-F8ge-l1pw5DsmIowC3WFEbYUV16DYfl5wWYsp7j8T-JGAjW8g4lYkq20z-7fF0mjK4G28sZrvmRhnLXZHUsDfnla9yu9F4hqvYnYS8IfX4mJX94EJIhnsTyPSSa7ltB3jK_1PPB1M9XGiB-jq99LugE_vK8-TKOH2DwJxY2aYmBEd0hyyAi1YHDPISrYwY_nXsxLLZt-sMsIF5hC5R_E4XZ9pQ3pr8l5dhlRy6Ywc5M40Nt4npsXmCi9oE9ytQpWjRKCD0MEcErO65xjzzmuIVmCUSwSWQtu7WAg9olyICbBG6CxdClpzWmcENtm9hnSVUeWvFYBN3F5M2kEKEgKY9qsZpYxntQUD4nyR-KIWrMzlrvJja8Tdr4ysnEXY-AOrCcq0H3OxTADVGy9_mjRQ8wMDGG6vWiJ6nvLbzSnlsIj40jzsOkelvJbI4Y9cFiwdstVog6GUTm3u/download" alt="tensorboard1" width="1200"/>

<img src="https://dl.boxcloud.com/d/1/b1!TQBCV-zAcowSpFyaRrN2EpMhIarOZ31pIhHxZSh02OK6-FlnJ4o8n_c_9jCaoX_ZrItB--y3mdxhDl2-l4qDyWBUVmHRbRhdkZv9eAgF1zCdfeHiKThXPYOUCwyQ9TZMer5f738lKT2K4a4HkKgcMrRDCyolt88KaiaNMsVUlvkM8UXQDw3-27HuMNmicPunXC962PNOAPAX0K4vJhgYRl0WL3IJkn1-w5b_Oh-DrAkr5J77v_kHzqPS42tJi0xzOKkhOJdG6uiDu-FYAm7Cl2CJa3nJ9yO_H2sBplKQst19XeNJUX37ykGbbB34ivejSbLuCBlWJILUkLgIVGiKfPo5u8Szznl-xISQCFXNYG2dXt5I6vuSivEVAkjdoV5ftNO6orzzMocZcjCfs7AX4z2RycxkFSnL7gif8GZmglH-7TYOGOhKM0iDve612ELWa1Xcdek7QBHLX8LHy7plNjLmQYqAJe7NOilo6DxxE60TNV2OcaNaTLMFO3epAKTcyUkCSH5IuQ4q-iP0PpZExewl2HnJ19XiTIe8EY46hGNtF5tmy5FQO81P1Gnh42j27_dbkkbUNoFgaj-4zBTN7v4iCTb66U6KWTCmSejVoQwBTc6XEhjW2SHKXaL-dZU5CuEF-zYNMqc9kRhnL9UyEKyh04WNU0ccm0QhM7md94Hhbdd7WCmqj86gm8OFXWy6HLkt2AYiX7fvAfQ86pyBb8vONAOhixt2N5skff4aN5BS2TVHVHV3OiEvqyWi7oXt01Mn_8egTr-1gRrrxIlKFQHrJN2xwuAEHCDJdeZA06h95MMQEgczg47qN5G_wx8JfL3ypMO2hzbtBqpIByN72P4chM2MLF7n9-hX5ghzQFbxVpswolVBg-tYoGy2g-WhQMe1KPrdq83nJtwTbcIisVX6aIxINrldPQL6LFA1c6MOD8hGzjTXVuMfBKA_K8U3W8v-31InO01l-l6weVqv1RuzxXYwt625oqJpA_VPgs_fFFuUZUoTfupdXFXl95EN7Yvp_6oheK0TfqyLr3_vm1PwPFv4DKM6i4eLPmTYP7NTO_lNaUHc3Kfpj4U3CYI4q3VOO3isDmhPbFkWGZDLDxbK1Eum8ovLiH4zy7Jr8kc0I_A6KYO7COR9Mvq72k10OQ21U8dQ9pnik1Qa-BPoBbauVP7Mdjqh-16g-WC_lsl4uRXraj32erWfgoUEY_w_8eDCU62wK4kd-wafwTW6Mpbu8kJHmknRXcuCNeAhUCn35wlf5doGfxBLnH00ELYTsbjnNezomc7rU5MN6DR9K-EvohyCdvQtyb6PiRMlTJ7-u8aL2mkzYXiLhVdC_sbYWoLXIsy-zYYYUfNAb8IQd3dal5GmDWHQLxtOakJlKXsFJuJwrl6s51LaTcE./download" alt="tensorboard2" width="1200"/>

### tune.ExperimentAnalysis

> TODO: Might refactor the steps into `ElasticappSweep.dataview()`.

```py
path = "ray_results/runner_2022-08-16_17-05-32/experiment_state-2022-08-16_17-05-33.json"
analysis = tune.ExperimentAnalysis(path)
df = analysis.dataframe()
```

## How-to-run

### Install

```sh
poetry install
```

### Grid-search: varying block-size and block-policy

```sh
python -m ElasticappSweep.cases.grid_search_blocksize_and_policy \
  --execute=<path to executable elastica++ case study file> \
  --default_config=<path to default configuration yaml file> \
```

### Scaling: varying number of element and rods

```sh
python -m ElasticappSweep.cases.scaling_wrt_num_element_and_threading \
  --execute=<path to executable elastica++ case study file> \
  --default_config=<path to default configuration yaml file> \
```
