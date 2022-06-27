#!/bin/bash -
#SBATCH -o elasticapp.stdout
#SBATCH -e elasticapp.stderr
#SBATCH --ntasks-per-node 128
#SBATCH --mem=256G
#SBATCH --job-name=<some name>
#SBATCH --nodes=1
#SBATCH --account=<some author>
#SBATCH --partition compute
#SBATCH --time 0:03:00
#SBATCH --chdir=<some directory>
#SBATCH --constraint=<constraint>

# To run a job on Expanse:
# - Set the --job-name, --nodes, --account, --chdir, and --time options above, which correspond to job name,
#   number of nodes, and wall time limit in HH:MM:SS, respectively.
# - Set --constraint if needed.
# - Set an `#SBATCH --account` argument for the computing account id.
# - Set the build directory, run directory, executable name,
#   and input file below.
# - For large jobs (multiple nodes), use --partition compute and --ntasks-per-node 128
# - For small jobs (fewer than 128 procs), use -p shared set the number of
#   processors as --ntasks-per-node
#
# Expanse you can edit the launch command at the end of this file directly.
#
# To submit the script to the queue run:
#   sbatch slurm_script.sh

# Replace these paths with the path to your build directory, to the source root
# directory, the elasticapp dependencies module directory, and to the directory
# where you want the output to appear, i.e. the run directory.
# E.g., if you cloned elasticapp in your home directory, set
# ELASTICAPP_BUILD_DIR to ${HOME}/elasticapp/cmake-build. If you want to run in a
# directory called "elastica-run" in the current directory, set
# ELASTICAPP_RUN_DIR to ${PWD}/elastica-run

export ELASTICAPP_BUILD_DIR=${HOME}/elasticapp/cmake-build-release-gcc/
export ELASTICAPP_HOME=${HOME}/elasticapp/
export ELASTICAPP_MODULE_DIR=${HOME}/elasticapp_dependencies
export ELASTICAPP_RUN_DIR=${HOME}/elasticapp-run

# Choose which cluster we are using (supports expanse or bridges2)
source ${ELASTICAPP_HOME}/support/LoadModules/expanse.sh
#source ${ELASTICAPP_HOME}/support/LoadModules/bridges2.sh

# Choose the executable and input file to run
# To use an input file in the current directory, set
# ELASTICAPP_INPUT_FILE to ${PWD}/InputFileName.yaml
export ELASTICAPP_EXECUTABLE=${ELASTICAPP_BUILD_DIR}/bin/example_axial_stretch
#export ELASTICAPP_INPUT_FILE=${PWD}/InputFileName.yaml # TODO
# Set the path to include the build directory's bin directory
export PATH=${ELASTICAPP_BUILD_DIR}/bin:$PATH

# These commands load the relevant modules and cd into the run directory,
# creating it if it doesn't exist
source ${ELASTICAPP_HOME}/support/LoadModules/cluster_setup.sh ${ELASTICAPP_MODULE_DIR}
elasticapp_load_modules
module list

# Configure conda environment
module load anaconda
conda activate gcc

mkdir -p ${ELASTICAPP_RUN_DIR}
cd ${ELASTICAPP_RUN_DIR}
# Copy the input file into the run directory, to preserve it
#cp ${ELASTICAPP_INPUT_FILE} ${ELASTICAPP_RUN_DIR}/ # TODO

# Set desired permissions for files created with this script
umask 0022

# Generate the nodefile
echo "Running on the following nodes:"
echo ${SLURM_NODELIST}
touch nodelist.$SLURM_JOBID
for node in $(echo $SLURM_NODELIST | scontrol show hostnames); do
  echo "host ${node}" >> nodelist.$SLURM_JOBID
done

# elasticapp executable
echo "#!/bin/sh
bash ${ELASTICAPP_MODULE_DIR}
${ELASTICAPP_EXECUTABLE}
" > runscript

chmod u+x ./runscript
./runscript
