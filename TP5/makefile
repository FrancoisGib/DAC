all:
	make terraform -B
	bash scripts/replace_templates.sh
	bash scripts/build_images.sh
	make ansible -B

terraform:
	terraform -chdir=terraform init
	terraform -chdir=terraform apply -auto-approve

ansible:
	bash scripts/ansible_config.sh

update-images:
	bash scripts/replace_templates.sh
	bash scripts/build_images.sh
	ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook ansible/webservers/update_images.yml -i inventory.ini

update-instances-number:
	bash scripts/replace_templates.sh
	ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook ansible/webservers/update_servers_number.yml -i inventory.ini

clean:
	terraform -chdir=terraform destroy -auto-approve
	rm -rf nginx/nginx.conf inventory.ini terraform/.terraform* terraform/terraform.*
