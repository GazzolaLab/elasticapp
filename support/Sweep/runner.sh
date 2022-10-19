export RUN_EXE=""
export INPUT_PATH=""

python cases/strong_scaling.py \
    -e $RUN_EXE \
    -i $INPUT_PATH

python cases/weak_scaling.py \
    -e $RUN_EXE \
    -i $INPUT_PATH
