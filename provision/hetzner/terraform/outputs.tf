resource "local_file" "AnsibleInventory" {
  content = templatefile("inventory.tmpl",
    {
      fsn1-dev-assfire-masters = hcloud_server.fsn1-dev-assfire-master.*,
      fsn1-dev-assfire-workers = hcloud_server.fsn1-dev-assfire-worker.*,
      hel1-dev-assfire-masters = hcloud_server.hel1-dev-assfire-master.*,
      hel1-dev-assfire-workers = hcloud_server.hel1-dev-assfire-worker.*,
      nbg1-dev-assfire-masters = hcloud_server.nbg1-dev-assfire-master.*,
      nbg1-dev-assfire-workers = hcloud_server.nbg1-dev-assfire-worker.*,
      floating_ip = "49.12.114.184"
  })
  filename = "inventory"
}
