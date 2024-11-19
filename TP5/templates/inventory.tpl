[proxy]
proxy ansible_host=${ proxy.access_ip_v4 }

[master]
master ansible_host=${ master.access_ip_v4 }

[webservers]
%{ for index, webserver in webservers ~}
webserver${ index } ansible_host=${ webserver.access_ip_v4 }
%{ endfor }

[slaves]
%{ for index, slave in slaves ~}
slave${ index } ansible_host=${ slave.access_ip_v4 }
%{ endfor }
[slaves:vars]
master_ip = ${ master.access_ip_v4 }

[all:vars]
ansible_ssh_private_key_file = ssh/id_rsa
docker_repository = francoisgib/dac-tp5
proxy_ip = ${ proxy.access_ip_v4 }
postgres_port = 5432
servers_subnet = 172.28.0.0/16