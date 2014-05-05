from fabric.api import run, local, env, cd, lcd, settings
from fabric.operations import put, get
from fabric.decorators import roles
#            #run("nohup make >& /dev/null < /dev/null &") # Run in the background

# has new passwords:
# 3, 10, 11, 12, 20

env.password = "etunaz20"
env.roledefs = {
    'leader': ['pi@leaf18.local', ],

    'all': [
        #'pi@leaf18.local',
        'pi@leaf4.local',
    ],
    
    'old': [
        'pi@leaf23.local',
        'pi@leaf30.local',
        'pi@leaf17.local',
    ]

}

env.skip_bad_hosts=True
addons_dir = '/home/pi/openFrameworks/addons'
code_dir = '/home/pi/openFrameworks/apps/leafLed/LedClient'

def compile_deploy():
    compile()
    deploy_all()

@roles('all')
def upgrade_system():
    run('sudo apt-get update')
    run('sudo apt-get upgrade')

@roles('leader')
def test_run():
    start()

#@roles('all')
#def update_src_all():    
#   with cd(code_dir):  
#        with lcd('LedClient'):
#            put('src/*', 'src/')   
#
#with lcd('../../addons/'):
#    with cd(addons_dir):
#        put('ofxLPD8806/src/*', 'ofxLPD8806/src/')
            
@roles('leader')
def update_addons():
    with lcd('../../addons/'):
        with cd(addons_dir):
            put('ofxLPD8806/src/*', 'ofxLPD8806/src/')


# TODO: Compile 1 place then distribute
@roles('leader')
def compile():
    with cd(code_dir):  
        with lcd('LedClient'):
            put('src/*', 'src/')
            run("make")
            get("bin/LedClient", "./bin/")

@roles('all')
def kill_all():
    with settings(warn_only=True):
        run('sudo killall -r LedClient') # stop it first


def deploy():
    with cd(code_dir):  
        with lcd('LedClient'):
            with settings(warn_only=True):
                run('rm bin/LedClient')
            put('bin/LedClient', 'bin')
            run('chmod a+x ./bin/LedClient')
        start_background()

@roles('all')
def deploy_all():
    deploy()

def start_background(): # not working
    #run("dtach sudo make run") 
    # _runbg('sudo make run');
    with settings(warn_only=True):
        run('sudo killall -r LedClient') # stop it first  
        #run('screen -S leaf -X quit')
        
    run("screen -d -m -S leaf sudo make run; sleep 1") 
    #run("nohup sudo make run & sleep 5; exit 0") 
    #run("nohup sudo make run & sleep 5; exit 0 >& /dev/null < /dev/null &") 

@roles('leader')
def test_start():
    start()
    
def start():
    with settings(warn_only=True):
        run('sudo killall -r LedClient') # stop it first
    with cd(code_dir):
        run("sudo make run")

@roles('all')
def start_all():
    start()

### Problem with theese, does not broadcast bonjour on startup - and do not turn of properly
#@roles('all')  
#def reboot_all():
#    run("sudo reboot")
#
#@roles('all')
#def shutdown_all():
#    run("sudo shutdown -r now")

@roles('all')
def list():
    run("hostname")

@roles('old')
def set_passwords():
    run('passwd pi')

@roles('old')
def install_screen():
    run("sudo apt-get install screen")