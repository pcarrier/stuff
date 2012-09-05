#!/usr/bin/env python

# Documentation to follow.

from __future__ import with_statement

from sys import argv, exit
from json import dumps
from syslog import syslog
from time import time

import shelve
from contextlib import closing

from lockfile import FileLock
import requests

PAGERDUTY_URI = 'http://events.pagerduty.com/generic/2010-04-15/create_event.json'
SERVICE_KEY = 'XXX'


def message_to_event(str):
    return dict(line.split(':', 1) for line in str.split('\n'))

def event_to_pagerduty_json(event):
    pagerduty = dict(service_key=SERVICE_KEY, details=event)

    pagerduty['description'] = event['name']

    status = event['status']
    if status == 'PROBLEM':
        pagerduty['event_type'] = 'trigger'
    elif status == 'OK':
        pagerduty['event_type'] = 'resolve'

    pagerduty['incident_key'] = generate_incident_key(event)

    return dumps(pagerduty)


def generate_incident_key(event):
    trigger_id = event['triggerid']

    with FileLock('/tmp/zabbix_pagerduty.lock') as lock:
        with closing(shelve.open('/tmp/zabbix_pagerduty.shelve')) as shelf:
            if event['status'] == 'OK':
                if trigger_id in shelf:
                    return shelf[trigger_id]
                else:
                    syslog('ignored OK: {0}'.format(event))
                    exit(1)
            else:
                key = '{0}:{1}'.format(trigger_id, int(time()))
                shelf[trigger_id] = key
                return key


def post_to_pagerduty(json):
    return requests.post(PAGERDUTY_URI, data=json)


if __name__ == '__main__':
    try:
        message = argv[3]
        syslog('received message: {0}'.format(message))

        event = message_to_event(message)
        syslog('received event: {0}'.format(event))

        if event['severity'] in ['High', 'Disaster']:
            json = event_to_pagerduty_json(event)
            syslog('json: {0}'.format(json))

            response = post_to_pagerduty(json)

            if (response.status_code != 200):
                syslog('wrong response: {0}'.format(response))

    except BaseException, e:
        syslog("exception: {0}".format(e))
