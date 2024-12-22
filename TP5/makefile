all:
	terraform -chdir=terraform init
	terraform -chdir=terraform apply -var "template_file=../templates/inventory.tpl" -auto-approve
	terraform -chdir=terraform output -raw inventory_ini > inventory.ini
	ansible-playbook ansible/master_playbook.yml

all-old-terraform-version:
	terraform init terraform
	terraform apply -auto-approve -var "template_file=./templates/inventory.tpl" terraform
	terraform output inventory_ini > inventory.ini
	ansible-playbook ansible/master_playbook.yml
	
update-webservers-images: build_webserver_image
	ansible-playbook ansible/update_webservers.yml

update-nginx-config:
	ansible-playbook ansible/update_nginx_config.yml

build_webserver_image:
	docker build -t francoisgib/dac-tp5:webserver ./webserver
	docker push francoisgib/dac-tp5:webserver

clean:
	rm inventory.ini
	terraform -chdir=terraform destroy -auto-approve
	
