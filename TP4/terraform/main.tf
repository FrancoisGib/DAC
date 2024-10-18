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

# Create a web server
resource openstack_compute_instance_v2 "master" {
  name        = "master"
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

# Create a web server
resource openstack_compute_instance_v2 "slave" {
  name        = "slave"
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

output "proxy" {
  value = openstack_compute_instance_v2.proxy.*.access_ip_v4
  description = "The IP address of the created OpenStack instance(s)"
}

output "master" {
  value = openstack_compute_instance_v2.master.*.access_ip_v4
  description = "The IP address of the created OpenStack instance(s)"
}

output "slaves" {
  value = openstack_compute_instance_v2.slave.*.access_ip_v4
  description = "The IP address of the created OpenStack instance(s)"
}

