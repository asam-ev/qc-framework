#!/bin/bash
# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


function pause(){
	read -p "Press [Enter] key to continue ..."
}

# Get current directory where script lies
INSTALL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo =============================
echo QChecker for OpenX - QC4OpenX
echo =============================
echo

if [ ! $1 ]; then
	echo Syntax:
	echo
	echo CheckXOSC.sh myXoscFileToCheck.xosc
	echo
	echo
	pause
	exit
fi

XOSC_FILE=$1
if [ ! -f "$XOSC_FILE" ]; then
	echo File $XOSC_FILE does not exist.
	echo
	echo
	pause
	exit
fi

if [[ ! ${XOSC_FILE,,} =~ \.xosc$ ]]; then
	echo File $XOSC_FILE is no XOSC file.
	echo
	echo
	pause
	exit
fi

echo
echo =========================================
echo == Configuration
echo =========================================
echo Check: $XOSC_FILE
echo
echo Delete all .xqar Files:
find . -name "*.xqar" -type f -delete -print
echo
echo

echo =========================================
echo == XOSC schema check
echo =========================================
$INSTALL_DIR/XoscSchemaChecker $XOSC_FILE
if [ $? != 0 ]; then
	echo Schema is not valid. Will skip any other checks.
	pause
	exit 1
fi
echo
echo =========================================
echo == Pool results
echo =========================================
$INSTALL_DIR/ResultPooling
echo
echo

echo =========================================
echo == Generate text report
echo =========================================
$INSTALL_DIR/TextReport Result.xqar
echo
echo

echo
echo
echo Finished all.
echo
