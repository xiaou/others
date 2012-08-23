#! /bin/sh


ps -A | grep spawn-fcgi | awk '{print $1}' | head -2
#ps -A | grep spawn-fcgi | awk '{print $1}'






