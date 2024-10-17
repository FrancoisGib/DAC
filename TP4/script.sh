ips=$(terraform output -state=terraform/terraform.tfstate | grep -E "[1-9]+" | sed 's/"//g' | sed 's/ //g')
ips_array=($(echo $ips | tr ',' "\n"))

proxy=${ips_array[0]}
master=${ips_array[1]}
slaves=("${ips_array[@]:2}")

echo "Proxy : $proxy"
echo "Master : $master"
echo "Slaves : ${slaves[@]}"

export ANSIBLE_HOST_KEY_CHECKING=False

cat templates/template_inventory.ini > inventory.ini
sed -i "s/{ proxy_ip }/$(echo $proxy | sed "s/ //g")/g" inventory.ini
sed -i "s/{ master_ip }/$(echo $master | sed "s/ //g")/g" inventory.ini
sed -i "s/{ slaves_ip }/$(echo ${slaves[@]} | sed 's/ /\\n/g')/g" inventory.ini

ansible-playbook ansible/playbook.yml -i inventory.ini
ansible-playbook ansible/playbook.yml -i inventory.ini

ansible-playbook ansible/master_configuration.yml -i inventory.ini
ansible-playbook ansible/slaves_configuration.yml -i inventory.ini

cat templates/pgpool.conf > pgpool.conf
sed -i "s/{ master_ip }/$(echo $master | sed "s/ //g")/g" pgpool.conf
sed -i "s/{ slave1_ip }/$(echo ${slaves[0]} | sed 's/ /\\n/g')/g" pgpool.conf
sed -i "s/{ slave2_ip }/$(echo ${slaves[1]} | sed 's/ /\\n/g')/g" pgpool.conf

ansible-playbook ansible/proxy_configuration.yml -i inventory.ini