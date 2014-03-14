#!/bin/bash
CLEAR="delete from team; delete from hex; delete from world; delete from faction; vacuum;"
sqlite3 before.db "${CLEAR}"
sqlite3 after.db "${CLEAR}"
./sqlite_test
sleep 10
python3 image.py before.db before.png
python3 image.py after.db after.png