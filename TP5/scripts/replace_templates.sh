#!/bin/bash

ips=$(terraform output -state=terraform/terraform.tfstate | grep -E "[1-9]+" | sed 's/"//g' | sed 's/ //g')
ips_array=($(echo $ips | tr ',' "\n"))

nginx_proxy=${ips_array[0]}
webservers=("${ips_array[@]:1}")

echo "Nginx proxy : $nginx_proxy"
echo "Web servers : ${webservers[@]}"

# Replace templates for configuration

cat templates/template_inventory.ini > inventory.ini
sed -i "s/{ nginx_proxy_ip }/$(echo $nginx_proxy | sed "s/ //g")/g" inventory.ini
sed -i "s/{ webservers_ip }/$(echo ${webservers[@]} | sed 's/ /\\n/g')/g" inventory.ini

webservers_ip_string=""

for webserver_ip in ${webservers[@]} 
do
  webservers_ip_string+="server ${webserver_ip}:8080 max_fails=3 fail_timeout=10s;\n\t\t"
done

cat templates/template-nginx.conf > nginx/nginx.conf
sed -i "s/{ instances_ip }/$(echo $webservers_ip_string)/g" nginx/nginx.conf