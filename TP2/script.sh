if [ -z $1 ]
then
    echo "Please provide a file name"
    exit 0
fi

cat template_inventory.ini > temp.ini
ips=$(terraform output | grep -E "[1-9]+" | sed 's/"//g' | sed 's/,/\\n/g' | sed 's/ //g')
sed -i "s/instances_ip/$(echo $ips | sed "s/ //g")/g" temp.ini
cat temp.ini > inventory.ini
rm temp.ini
export ANSIBLE_HOST_KEY_CHECKING=False
ansible-playbook $1 -i inventory.ini