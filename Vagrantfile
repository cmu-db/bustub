# -*- mode: ruby -*-
# vi: set ft=ruby et:

# Customize VM resources
VM_CPUS = "4"
VM_MEMORY = "2048" # MB
VM_NAME = "BustubVM"

Vagrant.configure(2) do |config|

  # Pick a box to use
  config.vm.box = "ubuntu/focal64"

  # Config VM
  config.vm.provider :virtualbox do |vb|
    vb.customize [ "modifyvm", :id,
      "--memory", VM_MEMORY,
      "--name", VM_NAME,
      "--nicspeed1", 1000000,
      "--nicspeed2", 1000000,
      "--ioapic", "on",
      "--cpus", VM_CPUS ]

  end

  # SSH
  config.ssh.forward_agent = true
  config.ssh.forward_x11 = true

  # Link current repo into VM
  config.vm.synced_folder "..", "/bustub"

  # Setup environment
  config.vm.provision :shell, :inline => "apt-get update"
  config.vm.provision "shell" do |s| 
    s.path = "https://raw.githubusercontent.com/cmu-db/bustub/master/build_support/packages.sh"
    s.args = "-y"
  end

  end
