#!/bin/bash

sudo apt-add-repository -y ppa:ansible/ansible 
sudo apt update 
sudo apt install -y ansible
ansible-playbook --vault-password-file ~/ansible_pass bootstrap.yaml

echo "##### Bootstrapping complete #####"
echo "You should be able to run 'ansible-playbook -i <path_to_playbooks>/terraform/inventory <playbook file>'"