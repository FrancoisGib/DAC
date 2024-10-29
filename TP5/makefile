all:
	make terraform -B
	bash scripts/replace_templates.sh
	bash scripts/build_images.sh
	make ansible -B

all-no-build:
	make terraform -B
	bash scripts/replace_templates.sh
	make ansible -B
	make update-nginx-config -B

terraform:
	terraform -chdir=terraform init
	terraform -chdir=terraform apply -auto-approve

ansible:
	bash scripts/ansible_config.sh

update-webservers-images:
	bash scripts/replace_templates.sh
	bash scripts/build_images.sh
	ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook ansible/webservers/update_images.yml -i inventory.ini

update-instances-number:
	bash scripts/replace_templates.sh
	ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook ansible/webservers/update_servers_number.yml -i inventory.ini
	make update-nginx-config -B

update-nginx-config:
	ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook ansible/nginx/update_config.yml -i inventory.ini

clean:
	rm -rf nginx/nginx.conf inventory.ini terraform/.terraform* terraform/terraform.* minimalserver/http minimalserver/db
	terraform -chdir=terraform destroy -auto-approve
