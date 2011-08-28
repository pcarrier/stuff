from sys import path
from os.path import dirname, join
from hashlib import sha1
from errno import ENOENT, errorcode

path.append(dirname(__file__))
from bottle import run, get, post, abort, request, default_app

#TMP
from bottle import debug
debug(True)

PASTIES_DIR = join(dirname(__file__), 'pasties')
URI = 'http://localhost:8080/'

@get('/')
def help():
    return "Send your pastie to\n"\
        "$ curl -F 'pastie=<-' "+URI+"\n"

@post('/')
def post():
    contents = request.forms.get('pastie')
    hash = sha1(contents).hexdigest()
    try:
        f = open(join(PASTIES_DIR, hash), 'w')
    except IOError, e:
        abort(500, errorcode[e.errno])
    else:
        f.write(contents)
    return URI+hash+"\n"

@get('/:hash')
def get(hash):
    try:
        f = open(join(PASTIES_DIR, hash), 'r')
    except IOError, e:
        if e.errno == ENOENT:
            abort(404, "No such pastie")
        else:
            abort(500, errorcode[e.errno])
    else:
        return f.read()

# application = default_app()
run(host='localhost', port=8080)
