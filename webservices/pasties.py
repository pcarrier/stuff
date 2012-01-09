from sys import path
from os.path import dirname, join
from hashlib import sha1
from errno import ENOENT, errorcode

path.append(dirname(__file__))
from bottle import run, get, post, redirect, abort, request, template, TEMPLATE_PATH, default_app

PASTIES_DIR = join(dirname(__file__), 'pasties')
TEMPLATE_PATH.append(join(dirname(__file__), 'templates'))
URI = 'http://localhost:8080/'

@get('/')
def help():
    return template('pasties_index', uri=URI)

@post('/')
def post():
    contents = request.forms.get('pastie')
    submit = request.forms.get('submit')
    hash = sha1(contents).hexdigest()
    try:
        f = open(join(PASTIES_DIR, hash), 'w')
    except IOError, e:
        abort(500, errorcode[e.errno])
    else:
        f.write(contents)
        f.close()
    if submit:
        # In a web client, redirect to the page
        redirect(URI+hash)
    else:
        # From the command-line, show the URI
        return URI+hash+"\n"

@get('/:hash#[0-9a-f]+#')
def get(hash):
    response.content_type = 'text/plain'
    try:
        f = open(join(PASTIES_DIR, hash), 'r')
    except IOError, e:
        if e.errno == ENOENT:
            abort(404, "No such pastie")
        else:
            abort(500, errorcode[e.errno])
    else:
        contents = f.read()
        f.close()
        return contents

# application = default_app()
run(host='localhost', port=8080)
