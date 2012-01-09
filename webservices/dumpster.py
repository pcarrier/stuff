from sys import path
from os import listdir, makedirs
from os.path import dirname, join, normpath
from errno import ENOENT, errorcode

path.append(dirname(__file__))
from bottle import get, post, run, request, abort, default_app, response

URI = 'https://localhost:8080/'
DATA_DIR = '/var/lib/dumpster'

def is_subpath(path, ancestor):
    path = normpath(path)
    return path.startswith(ancestor)

@post('/:machine/:file')
def post_file(machine, file):
    client_cn = request.environ.get('SSL_CLIENT_S_DN_CN')
    if client_cn != machine:
        abort(403, "You can only post into /%s/" % client_cn)

    contents = request.forms.get('data')

    try:
        dest_dir = join(DATA_DIR, machine)
        if not is_subpath(dest_dir, DATA_DIR):
            abort(403, "No way you're going up")
	try:
            makedirs(dest_dir)
	except:
	    pass
        dest_file = join(dest_dir, file)
        if not is_subpath(dest_file, DATA_DIR):
            abort(403, "No way you're going up")
        f = open(join(dest_dir, file), 'w')
    except IOError, e:
        abort(500, errorcode[e.errno])
    else:
        f.write(contents)
        f.close()

    return "%s%s/%s\n" % (URI, machine, file)

@get('/:machine/:file')
def get_file(machine, file):
    response.content_type = 'text/plain'

    path = join(DATA_DIR, machine, file)
    if not is_subpath(path, DATA_DIR):
        abort(403, "No way you're going up")

    try:
        f = open(join(DATA_DIR, machine, file), 'r')
    except IOError, e:
        if e.errno == ENOENT:
            abort(404, "Nothing here.")
        else:
            abort(500, errorcode[e.errno])
    else:
        contents = f.read()
        f.close()
        return contents

application = default_app()
