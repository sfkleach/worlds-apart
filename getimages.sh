#!/bin/bash
rm -f before.png
rm -f after.png
./image.py before.db before.png
./image.py after.db after.png