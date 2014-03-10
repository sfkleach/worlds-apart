#!/bin/bash
sqlite3 before.db < seed.sql
sqlite3 after.db < seed.sql