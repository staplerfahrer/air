#!/usr/bin/python3
import os
import sys
import json

KEY = '*** INSERT PASSPHRASE HERE ***'

print('content-type: text/html\n\n')

if os.environ['HTTP_USER_AGENT'] != KEY:
	print('bah')
	exit()

# vars={}
# for k in os.environ.keys():
# 	vars[k] = os.environ[k]
# env=json.dumps(vars,indent='\t',sort_keys=True)
# print(env)

try:
	LOG         = os.path.join('/var/www/jacobbruinsma.com/html/io/', os.environ['PATH_INFO'].lstrip('/'))
	LAST_POSTED = '/var/www/jacobbruinsma.com/html/io/air.json'
	if not LOG.startswith('/var/www/jacobbruinsma.com/html/io/'):
		raise Exception('invalid path provided')
	posted = sys.stdin.read()

	if len(posted):
		with open(LOG, 'a') as log:
			log.write(posted + '\n')
		with open(LAST_POSTED, 'w') as last:
			last.write(posted)

	# print(posted)

except Exception as e:
	print(e)