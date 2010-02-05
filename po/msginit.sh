#!/bin/bash
if [ -n "$*" ]
then
	msginit -l $*
else
	msginit -l zh_CN.utf8
fi
