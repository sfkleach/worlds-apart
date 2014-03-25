#!/bin/bash
DB=$1
CLEAR="delete from team; delete from hex; delete from world; delete from faction; vacuum;"
if [ -e before.db ]
then
	echo 'Clearing before.db'
	sqlite3 before.db "${CLEAR}"
else
	echo 'Creating before.db'
	sqlite3 before.db < seed.sql
fi
if [ -e after.db ]
then
	echo 'Clearing after.db'
	sqlite3 after.db "${CLEAR}"
else
	echo 'Creating after.db'
	sqlite3 after.db < seed.sql
fi
./simulation --db=${DB}
python3 image.py before.db before.png
python3 image.py after.db after.png