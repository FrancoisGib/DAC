variable "ssh_key" {
	description = "The ssh key"
	type = string
} 

resource openstack_compute_keypair_v2 "user_key" {
  name       = "user_key"
  public_key = var.ssh_key
}
