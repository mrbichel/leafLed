from fabric.api import run, local, env, cd, lcd
from fabric.operations import put

env.hosts = ['pi@leaf.local']
env.skip_bad_hosts=True


code_dir = '/home/pi/openFrameworks/apps/piLedControl/LedClient'

def upgrade():
    run('sudo apt-get update')
    run('sudo apt-get upgrade')

def start():
    with cd(code_dir):
        run("sudo make run")

def deploy():
    with cd(code_dir):
        with lcd('LedClient'):
            put('src/*', 'src/')
            run("make")
            #run("nohup make >& /dev/null < /dev/null &") # Run in the background
            
        
