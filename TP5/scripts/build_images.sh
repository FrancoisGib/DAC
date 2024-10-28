#!/bin/bash
if [ -z $DOCKER_REPOSITORY_PATH ]
then
  export DOCKER_REPOSITORY_PATH="francoisgib\/dac-tp5"
fi

docker_repository_path=$(echo "$DOCKER_REPOSITORY_PATH" | tr -d '\\')

if [ ! -z $NODE_SERVER ]
then
   docker build -t $docker_repository_path:webserver webserver
else
   gcc --static -o minimalserver/http minimalserver/http.c
   docker build -t $docker_repository_path:webserver minimalserver
fi

docker build -t $docker_repository_path:nginx-load-balancer nginx

docker push $docker_repository_path:webserver
docker push $docker_repository_path:nginx-load-balancer