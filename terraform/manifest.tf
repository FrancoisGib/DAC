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
resource openstack_compute_instance_v2 "ubuntu-1" {
  name        = "ubuntu-1"
  image_name  = "ubuntu22.04"
  flavor_name = "normale"
  key_pair    = "user_key"
  security_groups = ["ssh"]

  metadata = {
    app = "ubuntu"
  }

  depends_on = [
    openstack_compute_secgroup_v2.ssh, openstack_compute_keypair_v2.user_key
  ]
}

resource openstack_compute_instance_v2 "ubuntu-2" {
  name        = "ubuntu-2"
  image_name  = "ubuntu22.04"
  flavor_name = "normale"
  key_pair    = "user_key"
  security_groups = ["ssh"]

  metadata = {
    app = "ubuntu"
  }

  depends_on = [
    openstack_compute_secgroup_v2.ssh, openstack_compute_keypair_v2.user_key
  ]
}

resource openstack_compute_instance_v2 "ubuntu-3" {
  name        = "ubuntu-3"
  image_name  = "ubuntu22.04"
  flavor_name = "normale"
  key_pair    = "user_key"
  security_groups = ["ssh"]

  metadata = {
    app = "ubuntu"
  }

  depends_on = [
    openstack_compute_secgroup_v2.ssh, openstack_compute_keypair_v2.user_key
  ]
}