#
# deadeye
#
Vagrant.configure('2') do |config|
  config.vm.box = 'ubuntu/trusty64'

  config.vm.network 'public_network'

  config.vm.provider 'virtualbox' do |vb|
    vb.memory = 4096
    vb.cpus = 2
    vb.customize [
      'storageattach', :id,
      '--storagectl', 'SATAController',
      '--port', 1,
      '--device', 0,
      '--type', 'hdd',
      '--medium', '/Users/jeff/VirtualBox VMs/20161014-tk1.vdi'
    ]
  end

  # We provision using ansible_local, bootstrap our ansible roles by
  # downloading them from our repo.
  config.vm.provision 'shell', inline: <<-SCRIPT
  mount /dev/hdb1 /mnt
  GIT=/usr/bin/git
  ANSIBLE_REPO=https://github.com/strykeforce/ansible.git
  ANSIBLE_DIR=/opt/ansible

  [[ ! -x $GIT ]] && apt-get install -y git

  # if [[ -d $ANSIBLE_DIR ]]; then
  #   cd $ANSIBLE_DIR
  #   $GIT pull -q
  # else
  #   $GIT clone -q $ANSIBLE_REPO $ANSIBLE_DIR
  # fi
  /bin/true
  SCRIPT

  config.vm.provision 'ansible_local' do |ansible|
    ansible.provisioning_path = '/opt/ansible'
    ansible.playbook = 'vagrant.yml'
    ansible.groups = {
      'bazel' => ['default']
    }
    ansible.sudo = true
    ansible.verbose = false
  end
end
