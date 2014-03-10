#!/bin/bash
CLEAR="delete from team; delete from hex; delete from world; vacuum;"
sqlite3 before.db "${CLEAR}"
sqlite3 after.db "${CLEAR}"
./sqlite_test