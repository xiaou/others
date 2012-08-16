#! /bin/sh

function countarg
{
	echo "$#"
}

#IFS=,
countarg "$@"
countarg "$*"

