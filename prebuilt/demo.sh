#!/bin/sh
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi
./ryzenadj --stapm-time=3600 --stapm-limit=27000 --fast-limit=30000 --slow-limit=27000 --tctl-temp=90 --vrmmax-current=45000 --reapply-every=500
