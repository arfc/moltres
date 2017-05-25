#!/bin/bash

# If a user supplies an argument, that directory will be used as the start point for finding files that contain whitespace, otherwise the root directory
# will be used (one up from the scripts directory where this script is located)
REPO_DIR=${1:-"$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/../"}

if [ ! -d "$REPO_DIR" ]; then
  echo "$REPO_DIR directory does not exist";
else
    while read -rd '' fname; do
	expand $fname > /tmp/expandtmp
	mv /tmp/expandtmp $fname
  done < <( find "$REPO_DIR" -name "*.i" -type f -print0)
fi
