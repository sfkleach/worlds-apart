#!/bin/bash
CLEAR="delete from team; delete from hex; delete from world; delete from faction; vacuum;"
if [ -e dump0.db ]
then
	echo 'Clearing before.db'
	sqlite3 dump0.db "${CLEAR}"
else
	echo 'Creating before.db'
	sqlite3 dump0.db < seed.sql
fi
if [ -e dump1.db ]
then
	echo 'Clearing after.db'
	sqlite3 dump1.db "${CLEAR}"
else
	echo 'Creating after.db'
	sqlite3 dump1.db < seed.sql
fi
./simulation $*
python3 image.py dump0.db dump0.png
python3 image.py dump1.db dump1.png