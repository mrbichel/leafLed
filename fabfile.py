from fabric.api import run, local, put, env, cd
env.hosts = ['pi@leaf.local']

code_dir = '/home/pi/openFrameworks/apps/piLedControl'

def upgrade():
    run('sudo apt-get update')
    run('sudo apt-get upgrade')

def start():
    with cd(code_dir):
        with cd('LedClient'):
            run("sudo make run")

def deploy():
    with cd(code_dir):
        put('LedClient', code_dir)
        with cd('LedClient'):
            run("nohup make >& /dev/null < /dev/null &") # Run in the background
            
        
