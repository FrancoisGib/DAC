all:
	terraform init terraform
	terraform apply -auto-approve terraform
	bash script.sh

clean:
	terraform destroy -auto-approve terraform
	rm -rf nginx/nginx.conf inventory.ini .terraform* terraform.*