#!/bin/bash

cd /usr/local/svn/bin

./svnserve -d -r /repo1 --listen-host 192.168.1.6

exit 0
