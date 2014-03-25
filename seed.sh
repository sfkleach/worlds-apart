#!/bin/bash
# Usage: ./seed.sh JSON.json DATABASE.db IMAGE.png
JSON=$1
DB=$2
IMG=$3
if [ -e ${DB} ]
then
	mv ${DB} ${DB}-
fi
sqlite3 ${DB} < seed.sql
python3 seed.py ${DB} ${JSON}
python3 image.py ${DB} ${IMG}
