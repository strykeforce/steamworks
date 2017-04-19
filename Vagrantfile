#
# roborio-skel
#
Vagrant.configure('2') do |config|
  config.vm.box = 'bento/ubuntu-16.04'

  config.vm.provider 'vmware_fusion' do |v|
    v.name = 'deadeye'
    v.vmx['memsize'] = '1024'
    v.vmx['numvcpus'] = '2'
  end

  # manual provisioning
  # - FlyCapture for Linux 64
  # - OpenCV 2.4 sudo apt install libopencv-dev; http://packages.ubuntu.com/xenial/libopencv-dev
  # - Msgpack: cd /opt; sudo git clone https://github.com/msgpack/msgpack-c.git
  # We provision using ansible_local, bootstrap our ansible roles by
  # downloading them from our repo.
  # config.vm.provision 'shell', inline: <<-SCRIPT
  # GIT=/usr/bin/git
  # ANSIBLE_REPO=https://github.com/strykeforce/ansible.git
  # ANSIBLE_VERSION=master
  # ANSIBLE_DIR=/opt/ansible
  #
  # [[ ! -x $GIT ]] && apt-get install -y git
  #
  # [[ ! -d $ANSIBLE_DIR ]] && $GIT clone -q $ANSIBLE_REPO $ANSIBLE_DIR
  #
  # cd $ANSIBLE_DIR
  # $GIT checkout -q $ANSIBLE_VERSION
  # $GIT pull -q
  # SCRIPT
  #
  # config.vm.provision 'ansible_local' do |ansible|
  #   ansible.provisioning_path = '/opt/ansible'
  #   ansible.playbook = 'vagrant.yml'
  #   ansible.groups = {
  #     'roborio' => ['default']
  #   }
  #   ansible.sudo = true
  #   ansible.verbose = false
  #   ansible.raw_arguments = ['--vault-password-file=/vagrant/.vault_pw']
  # end
end
