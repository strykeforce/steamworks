Vagrant.configure('2') do |config|
  config.vm.box = 'ubuntu/trusty64'

  config.vm.network 'public_network'

  # We provision using ansible_local, bootstrap our ansible roles by
  # downloading them from our repo.
  config.vm.provision 'shell', inline: <<-SCRIPT
  GIT=/usr/bin/git
  ANSIBLE_REPO=https://github.com/strykeforce/ansible.git
  ANSIBLE_DIR=/opt/ansible

  [[ ! -x $GIT ]] && apt-get install -y git

  if [[ -d $ANSIBLE_DIR ]]; then
    cd $ANSIBLE_DIR
    $GIT pull -q
  else
    $GIT clone -q $ANSIBLE_REPO $ANSIBLE_DIR
  fi
  SCRIPT

  config.vm.provision 'ansible_local' do |ansible|
    ansible.provisioning_path = '/opt/ansible'
    ansible.playbook = 'opencv.yml'
    ansible.groups = {
      'opencv' => ['default']
    }
    ansible.sudo = true
    ansible.verbose = false
  end
end
