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
resource openstack_compute_instance_v2 "ubuntu" {
  name        = "ubuntu"
  image_name  = "ubuntu22.04"
  flavor_name = "normale"
  key_pair    = "user_key"
  security_groups = ["default"]
  count = 3

  metadata = {
    app = "ubuntu"
  }

  depends_on = [
    openstack_compute_secgroup_v2.ssh, openstack_compute_keypair_v2.user_key
  ]
}
