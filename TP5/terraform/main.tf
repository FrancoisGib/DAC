# Define required providers
terraform {
required_version = ">= 0.13.4"
  required_providers {
    openstack = {
      source  = "terraform-provider-openstack/openstack"
      version = "~> 1.53.0"
    }
  }
}

resource openstack_compute_instance_v2 "proxy" {
  name        = "proxy"
  image_name  = "ubuntu22.04"
  flavor_name = "normale"
  key_pair    = "user_key"
  security_groups = ["default"]

  metadata = {
    app = "ubuntu"
  }

  depends_on = [
    openstack_compute_keypair_v2.user_key
  ]
}

resource openstack_compute_instance_v2 "servers" {
  count = 3
  name        = "server-${count.index}"
  image_name  = "ubuntu22.04"
  flavor_name = "normale"
  key_pair    = "user_key"
  security_groups = ["default"]

  metadata = {
    app = "ubuntu"
  }

  depends_on = [
    openstack_compute_keypair_v2.user_key
  ]
}

variable "template_file" {
  description = "Template file path"
  type        = string
  default     = "../templates/inventory.tpl"
}

output "inventory_ini" {
  sensitive = true
  value = templatefile(var.template_file, {
    proxy      = openstack_compute_instance_v2.proxy,
    webservers = openstack_compute_instance_v2.servers,
    master     = openstack_compute_instance_v2.servers[0],
    slaves     = slice(openstack_compute_instance_v2.servers, 1, length(openstack_compute_instance_v2.servers))
  })
  description = "Ansible inventory.ini formatted output"
}