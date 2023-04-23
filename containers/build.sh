#!/usr/bin/env sh

arg_dockerfile="containers/Dockerfile.buildenv"
arg_target=dev
arg_imagename=dev-elasticapp

read -rd '' globalhelp <<-EOF
Usage
-----
${0} [options]

options
-------
  --help              : Print this help message.
  --file <dockerfile> : Docker file to use for build.
  --target <tgt>      : Target image, one of [dev | ci]
  --tag <imagename>   : Image name for the container.
EOF

while [ $# -gt 0 ]; do case "$1" in
        --file)
                arg_dockerfile="$2"; shift;;
        --tag)
                arg_imagename="$2"; shift;;
        --target)
                arg_target="$2"; shift;;
        -h | --help)
                echo "${globalhelp}"
                exit 0
                ;;
        *)
                printf "* install: Invalid option encountered, see usage below*\n"
                echo "${globalhelp}"
                exit 1
                ;;
        esac
        shift
done

docker_args="-f ${arg_dockerfile} --target ${arg_target} --build-arg uid=$(id -u) --build-arg gid=$(id -g) --tag=${arg_imagename} ."

echo "Building container:"
cmd="docker build ${docker_args}"
echo "> ${cmd}"
eval "${cmd}"
