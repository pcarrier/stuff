# List every filename that doesn't contain a dot in files/
# Put bottle.py in the same directory
from sys import path
from os import listdir
from os.path import dirname, join

path.append(dirname(__file__))
from bottle import run, route, default_app

@route('/envs')
def envs():
    return "\n".join(
        filter(lambda name: '.' not in name,
            listdir(join(dirname(__file__), 'dataroot'))
        )
    )+"\n"

# application = default_app()
run(host='localhost', port=8080)
