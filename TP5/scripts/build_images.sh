#!/bin/bash

if [ ! -z $1 ] && [ $1 -eq 1 ]
then
   docker build -t francoisgib/dac-tp5:webserver webserver
else
   gcc --static -o minimalserver/http minimalserver/http.c
   docker build -t francoisgib/dac-tp5:webserver minimalserver
fi

docker build -t francoisgib/dac-tp5:nginx-load-balancer nginx

docker push francoisgib/dac-tp5:webserver
docker push francoisgib/dac-tp5:nginx-load-balancer