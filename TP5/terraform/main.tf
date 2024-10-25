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

resource openstack_compute_instance_v2 "nginx-proxy" {
  name        = "nginx-proxy"
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

resource openstack_compute_instance_v2 "webservers" {
  name        = "webservers"
  image_name  = "ubuntu22.04"
  flavor_name = "normale"
  key_pair    = "user_key"
  security_groups = ["default"]
  count = 2

  metadata = {
    app = "ubuntu"
  }

  depends_on = [
    openstack_compute_keypair_v2.user_key
  ]
}

output "nginx-proxy" {
  value = openstack_compute_instance_v2.nginx-proxy.*.access_ip_v4
  description = "The IP address of the created OpenStack instance(s)"
}

output "webservers" {
  value = openstack_compute_instance_v2.webservers.*.access_ip_v4
  description = "The IP address of the created OpenStack instance(s)"
}

