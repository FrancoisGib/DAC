#!/bin/bash

docker build -t francoisgib/dac-tp5:webserver webserver
docker build -t francoisgib/dac-tp5:nginx-load-balancer nginx

docker push francoisgib/dac-tp5:webserver
docker push francoisgib/dac-tp5:nginx-load-balancer