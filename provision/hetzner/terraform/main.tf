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
  ssh_keys = ["eaglegor@ubuntu-win", "eaglegor@DESKTOP"]
}

provider "hcloud" {
  token = var.hcloud_token
}

data "hcloud_floating_ip" "fsn1-floating-ip-1" {
  ip_address = "49.12.114.184"
}

resource "hcloud_server" "fsn1-dev-assfire-master" {
  count       = 1
  name        = "fsn1-dev-assfire-master-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "fsn1"
  ssh_keys    = local.ssh_keys
}

resource "hcloud_server" "fsn1-dev-assfire-worker" {
  count       = 2
  name        = "fsn1-dev-assfire-worker-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "fsn1"
  ssh_keys    = local.ssh_keys
  labels      = {
    "ingress" = "true"
  }
}

resource "hcloud_floating_ip_assignment" "fsn1-assfire-worker-1" {
  floating_ip_id = data.hcloud_floating_ip.fsn1-floating-ip-1.id
  server_id      = hcloud_server.fsn1-dev-assfire-worker[0].id
}

resource "hcloud_server" "hel1-dev-assfire-master" {
  count       = 0
  name        = "hel1-dev-assfire-master-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "hel1"
  ssh_keys    = local.ssh_keys
}

resource "hcloud_server" "hel1-dev-assfire-worker" {
  count       = 0
  name        = "hel1-dev-assfire-worker-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "hel1"
  ssh_keys    = local.ssh_keys
  labels      = {
      "ingress" = "true"
    }
}


resource "hcloud_server" "nbg1-dev-assfire-master" {
  count       = 0
  name        = "nbg1-dev-assfire-master-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "nbg1"
  ssh_keys    = local.ssh_keys
}

resource "hcloud_server" "nbg1-dev-assfire-worker" {
  count       = 0
  name        = "nbg1-dev-assfire-worker-${count.index + 1}"
  image       = "centos-7"
  server_type = "cx21"
  location    = "nbg1"
  ssh_keys    = local.ssh_keys
  labels      = {
      "ingress" = "true"
    }
}
