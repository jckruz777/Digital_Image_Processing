#!/bin/bash

free & > /dev/null	
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
free & > /dev/null
