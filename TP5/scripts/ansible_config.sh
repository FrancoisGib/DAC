#!/bin/bash

# Allow read access to ssh files (just in case)
chmod 400 ssh/*

export ANSIBLE_HOST_KEY_CHECKING=False

ansible-playbook ansible/master_playbook.yml -i inventory.ini