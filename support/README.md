# Set of scripts to aid cluster usage

1. [Expanse(UCSD)](#cluster-setup) and [Bridges2(PSC)](#cluster-setup)
2. [Job Submission](#job-submission)
2. [Other Dependencies](#other-dependencies)

## Cluster Setup

>> Setting up dependencies might take some time (10-15 min). I would recommend building the dependencies before submitting the job. Within the script, the download/installation process is skipped if dependencies already exist.

### Module Install

```bash
source LoadModules/expanse.sh  # or bridges2.sh
source LoadModules/cluster_setup.sh ~/elasticapp_module  # Install dependencies and packages
module use ${HOME}/elasticapp_module/modules  # Register dependencies as module. (must provide absolute path)
module avail
```

```bash
----------------------------------- /home/<user-name>/elasticapp_module/modules ----------------------------------
blaze    blaze_tensor    brigand    cxxopts    yaml-cpp
 
------------------------ /cm/shared/apps/spack/cpu/lmod/linux-centos8-x86_64/clang/10.0.0 ------------------------
amdblis/2.2-openmp        hwloc/1.11.11    ncurses/6.2           openmpi/4.0.4    xz/5.2.5
amdblis/2.2        (D)    iq-tree/2.1.1    numactl/2.0.12 (L)    ucx/1.8.1

---------------------------- /cm/shared/apps/spack/cpu/lmod/linux-centos8-x86_64/Core ----------------------------
abaqus/2018.bk                       gaussian09/09.D.01        matlab/2022a
abaqus/2018                          gaussian09/09.E.01 (D)    mpfr/4.0.2
abaqus/2021hf7                (D)    gcc/7.5.0                 nbo/7.0-openblas

...
```

### Module Load

```bash
module load blaze
module load blaze_tensor
module load brigand
module load yaml-cpp
module load cxxopts
```

#### Alias

Alias in `LoadModules/<cluster>.sh` provides batch module loads:

```bash
elasticapp_load_modules  # Load necessary modules
...
```

## Job Submission

We provde basic template for batch script in `SubmitCluster/cluster_script.sh`.

## Other Dependencies

Following dependencies are require to complete the build, but not included in the above setup scripts. Make sure those are available on your cluster setup before building `elasticapp`.

|     **Dependency**    | **Minimum Version** | **Recommendation** |
|:---------------------:|:-------------------:|:------------------:|
|         cmake         |         3.13        |  anaconda, modules |
|        doxygen        |                     |      anaconda      |
|        PyBind11       |                     |      anaconda      |
| compiler (gcc, clang) | gcc>10, clang>10    | anaconda, modules  |

