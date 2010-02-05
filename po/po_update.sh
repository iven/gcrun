#!/bin/bash
xgettext -k_ -kN_ --keyword=Q_:1g -o gcrun.pot ../src/*.c
msgmerge -o zh_CN.po zh_CN.po gcrun.pot
