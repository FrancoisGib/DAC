#ansible-playbook $1 -i inventory.ini

ips=$(terraform output | grep -E "[1-9]+" | sed 's/"//g' | sed 's/ //g')
ips_array=($(echo $ips | tr ',' "\n"))
#echo $ips
#echo ${ips_array[@]}

master=${ips_array[0]}
slaves=("${ips_array[@]:1}")

echo "Master : $master"
echo "Slaves : ${slaves[@]}"

cat template_inventory.ini > inventory.ini
sed -i "s/master_ip/$(echo $master | sed "s/ //g")/g" inventory.ini

sed -i "s/slaves_ip/$(echo ${slaves[@]} | sed 's/ /\\n/g')/g" inventory.ini

export ANSIBLE_HOST_KEY_CHECKING=False

ansible-playbook playbook.yml -i inventory.ini