#!/usr/bin/env bash
# @file      upload_artifacts.sh
# @author    Ignacio Vizzo     [ivizzo@uni-bonn.de]
#
# Copyright (c) 2020 Ignacio Vizzo, all rights reserved
set -e

cat <<EOF
Modern C++ Final Project Release Script Helper.

Welcome to the magic script to make release, you only need to provide the
comlpete name of the authors of the project. If you are only 1 author just hit
enter when asked for the 2nd author.

As pre-requiste you only need to have:
   - A running CI/CD pipeline with a job called 'deploy'. This job must be the
     last job of your pipeline and must provide the 'artifacts.zip' build
     artifacts, from the deploy stage. If you are using the template this
     should be already done.
   - An \$IGG_GITLAB_TOKEN env. variable on your system. You need to create
     yourself this token from your profile page in gitlab, and copy paste it
     to your ~/.bashrc(export IGG_GITLAB_TOKEN=jahsdkahsd44)
EOF

# Install requirments (if not there)
python3 -c 'import gitlab' 2>/dev/null && python3 -c 'import git' 2>/dev/null || {
  echo "Installing Python3 dependencies"
  python3 -m pip install --user --upgrade python-gitlab gitpython
}

python3 make_release.py
