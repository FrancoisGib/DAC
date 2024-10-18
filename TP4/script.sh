# Get the ip addresses of the instances and split them into master proxy slaves

ips=$(terraform output -state=terraform/terraform.tfstate | grep -E "[1-9]+" | sed 's/"//g' | sed 's/ //g')
ips_array=($(echo $ips | tr ',' "\n"))

proxy=${ips_array[0]}
master=${ips_array[1]}
slaves=("${ips_array[@]:2}")

echo "Proxy : $proxy"
echo "Master : $master"
echo "Slaves : ${slaves[@]}"

# Replace templates for configuration

## Replace the template inventory.ini
cat templates/template_inventory.ini > inventory.ini
sed -i "s/{ proxy_ip }/$(echo $proxy | sed "s/ //g")/g" inventory.ini
sed -i "s/{ master_ip }/$(echo $master | sed "s/ //g")/g" inventory.ini
sed -i "s/{ slaves_ip }/$(echo ${slaves[@]} | sed 's/ /\\n/g')/g" inventory.ini

## Replace the template haproxy.cfg
cat templates/template_haproxy.cfg > haproxy.cfg
sed -i "s/{ master_ip }/$(echo $master | sed "s/ //g")/g" haproxy.cfg
sed -i "s/{ slave1_ip }/$(echo ${slaves[0]} | sed 's/ /\\n/g')/g" haproxy.cfg
sed -i "s/{ slave2_ip }/$(echo ${slaves[1]} | sed 's/ /\\n/g')/g" haproxy.cfg

# Launch the different configurations playbooks

export ANSIBLE_HOST_KEY_CHECKING=False

ansible-playbook ansible/common_configuration.yml -i inventory.ini
ansible-playbook ansible/master_configuration.yml -i inventory.ini
ansible-playbook ansible/slaves_configuration.yml -i inventory.ini
ansible-playbook ansible/proxy_configuration.yml -i inventory.ini