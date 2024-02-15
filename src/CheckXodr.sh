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

echo
echo =============================
echo QChecker for OpenX - QC4OpenX
echo =============================

if [ ! $1 ]; then
	echo Syntax:
	echo
	echo CheckXODR.sh myXodrFileToCheck.xodr
	echo
	echo
	pause
	exit
fi

XODR_FILE=$1
if [ ! -f "$XODR_FILE" ]; then
	echo File $XODR_FILE does not exist.
	echo
	echo
	pause
	exit
fi

if [[ ! ${XODR_FILE,,} =~ \.xodr$ ]]; then
	echo File $XODR_FILE is no XODR file.
	echo
	echo
	pause
	exit
fi

echo
echo =========================================
echo == Configuration
echo =========================================
echo Check: $XODR_FILE
echo
echo Delete all .xqar Files:
find . -name "*.xqar" -type f -delete -print
echo
echo

echo =========================================
echo == XODR schema check
echo =========================================
$INSTALL_DIR/XodrSchemaChecker $XODR_FILE
if [ $? != 0 ]; then
	echo Schema is not valid. Will skip any other checks.
	pause
	exit 1
fi
echo
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
echo Finished all.
echo
