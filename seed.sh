#!/bin/bash
if [ -e $1 ]
then
	mv $1 $1-
fi
sqlite3 $1 < seed.sql
python3 seed.py $1
python3 image.py $1 $2
