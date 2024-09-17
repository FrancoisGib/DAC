resource openstack_compute_secgroup_v2 "ssh" {
  name        = "ssh"
  description = "Open input ssh port"
  rule {
    from_port   = 22
    to_port     = 22
    ip_protocol = "tcp"
    cidr        = "172.18.12.0/23"
  }
}