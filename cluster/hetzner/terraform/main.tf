terraform {
  backend "remote" {
    hostname     = "app.terraform.io"
    organization = "assfire"

    workspaces {
      name = "dev"
    }
  }
}

variable hcloud_token {}

locals {
  ssh_keys = ["eaglegor@DESKTOP-N469TTV"]
}

provider "hcloud" {
  token = var.hcloud_token
}

resource "hcloud_network" "assfire-private-network" {
  name = "assfire-private-network"
  ip_range = "10.0.1.0/24"
}

resource "hcloud_network_subnet" "assfire-private-network-eu" {
  network_id = hcloud_network.assfire-private-network.id
  type = "cloud"
  network_zone = "eu-central"
  ip_range = "10.0.1.0/24"
}

locals {
    cnt_fsn1_master = 1
    cnt_fsn1_worker = 2
    cnt_hel1_master = 0
    cnt_hel1_worker = 0
    cnt_nbg1_master = 0
    cnt_nbg1_worker = 0
}

resource "hcloud_server" "fsn1-dev-assfire-master" {
  count       = local.cnt_fsn1_master
  name        = "fsn1-dev-assfire-master-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "fsn1"
  ssh_keys    = local.ssh_keys
}

resource "hcloud_server_network" "fsn1-dev-assfire-master-network" {
  count = local.cnt_fsn1_master
  server_id = hcloud_server.fsn1-dev-assfire-master[count.index].id
  subnet_id = hcloud_network_subnet.assfire-private-network-eu.id
}

resource "hcloud_server" "fsn1-dev-assfire-worker" {
  count       = local.cnt_fsn1_worker
  name        = "fsn1-dev-assfire-worker-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "fsn1"
  ssh_keys    = local.ssh_keys
  labels      = {
    "ingress" = "true"
  }
}

resource "hcloud_server_network" "fsn1-dev-assfire-worker-network" {
  count = local.cnt_fsn1_worker
  server_id = hcloud_server.fsn1-dev-assfire-worker[count.index].id
  subnet_id = hcloud_network_subnet.assfire-private-network-eu.id
}

resource "hcloud_server" "hel1-dev-assfire-master" {
  count       = local.cnt_hel1_master
  name        = "hel1-dev-assfire-master-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "hel1"
  ssh_keys    = local.ssh_keys
}

resource "hcloud_server_network" "hel1-dev-assfire-master-network" {
  count = local.cnt_hel1_master
  server_id = hcloud_server.hel1-dev-assfire-master[count.index].id
  subnet_id = hcloud_network_subnet.assfire-private-network-eu.id
}

resource "hcloud_server" "hel1-dev-assfire-worker" {
  count       = local.cnt_hel1_worker
  name        = "hel1-dev-assfire-worker-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "hel1"
  ssh_keys    = local.ssh_keys
  labels      = {
      "ingress" = "true"
    }
}

resource "hcloud_server_network" "hel1-dev-assfire-worker-network" {
  count = local.cnt_hel1_worker
  server_id = hcloud_server.hel1-dev-assfire-worker[count.index].id
  subnet_id = hcloud_network_subnet.assfire-private-network-eu.id
}

resource "hcloud_server" "nbg1-dev-assfire-master" {
  count       = local.cnt_nbg1_master
  name        = "nbg1-dev-assfire-master-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "nbg1"
  ssh_keys    = local.ssh_keys
}

resource "hcloud_server_network" "nbg1-dev-assfire-master-network" {
  count = local.cnt_nbg1_master
  server_id = hcloud_server.nbg1-dev-assfire-master[count.index].id
  subnet_id = hcloud_network_subnet.assfire-private-network-eu.id
}

resource "hcloud_server" "nbg1-dev-assfire-worker" {
  count       = local.cnt_nbg1_worker
  name        = "nbg1-dev-assfire-worker-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "nbg1"
  ssh_keys    = local.ssh_keys
  labels      = {
      "ingress" = "true"
    }
}

resource "hcloud_server_network" "nbg1-dev-assfire-worker-network" {
  count = local.cnt_nbg1_worker
  server_id = hcloud_server.nbg1-dev-assfire-worker[count.index].id
  subnet_id = hcloud_network_subnet.assfire-private-network-eu.id
}
