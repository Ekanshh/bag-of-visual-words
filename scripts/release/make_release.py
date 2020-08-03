#!/usr/bin/env python3
# @file      make_release.py
# @author    Ignacio Vizzo     [ivizzo@uni-bonn.de]
#
# Copyright (c) 2020 Ignacio Vizzo, all rights reserved
import os
import sys

import gitlab
from git import Repo

igg_gitlab = 'https://gitlab.igg.uni-bonn.de/'
igg_token = os.environ['IGG_GITLAB_TOKEN']

# Markdown descriptions templates
description_template = """
# Authors

- {author_1}
- {author_2}

# Latest Artifacts

- {artifacts}
"""

description_template_one = """
# Author

- {author_1}

# Latest Artifacts

- {artifacts}
"""


def get_project_name():
    """
    Obtains the namespace of the project and the project name from the remote
    git url of the project.
    """
    git = Repo(os.getcwd(), search_parent_directories=True).git
    remote = git.remote('-vv').split()[1]
    return remote.split(':')[-1].split('.')[0]


def get_deploy_job(project):
    """Obtain a job object from the lastest successful pipeline."""
    # The first pipeline is always the latest one
    last_pipeline = project.pipelines.list()[0]
    # The last job should always be the deploy
    for job in last_pipeline.jobs.list():
        if 'deploy' in job.attributes['name']:
            deploy_job_id = job.get_id()
            return project.jobs.get(deploy_job_id)

    # If we reach this point, this means that the deploy job doesn't exist
    print("[ERROR] 'deploy' job doesn't exist in the latest pipeline")
    sys.exit(1)


def download_latest_artifacts(project):
    """Dowload the latest deploy artifacts and keep it on a memory object."""
    deploy_job = get_deploy_job(project)
    try:
        return deploy_job.artifacts()
    except gitlab.exceptions.GitlabGetError as err:
        print("[ERROR] latest deploy job most likely didn't succeed,", err)
        print("Please check:", deploy_job.attributes['web_url'])
        sys.exit(1)


def get_student_names():
    """
    Prompts the user for the First and Last name, and returns 2 string with the
    capitalized version of the authors's names.

    The expected input is in the form of:
       - Ignacio Vizzo
       - Cyrill Stachniss
    """
    print("Enter first author name and surname:")
    author_1 = input().title()

    print("Enter second author name and surname(or press [ENTER] to skip):")
    author_2 = input().title()

    return author_1, author_2


def get_gitlab_project(name):
    """
    Obtains a Gitlab Project object using the python gitlab api. This will allow
    to manage the project from the main script.
    """
    gl = gitlab.Gitlab(igg_gitlab, igg_token)
    try:
        return gl.projects.get(name)
    except gitlab.exceptions.GitlabGetError as error:
        print("[ERROR], most likely you don't have the right token api", error)
        sys.exit(1)


def get_release_data(artifacts_md):
    """With the given markdown_md (containts the link to the latest push
    artifacts) builds a dictionary with all the release information."""
    author_1, author_2 = get_student_names()
    if author_2:
        surname_1 = author_1.split()[-1]
        surname_2 = author_2.split()[-1]
        tag_name = "2020_{surname_1}_{surname_2}".format(surname_1=surname_1,
                                                         surname_2=surname_2)
        description = description_template.format(author_1=author_1,
                                                  author_2=author_2,
                                                  artifacts=artifacts_md)
    else:
        surname_1 = author_1.split()[-1]
        tag_name = "2020_{surname_1}".format(surname_1=surname_1)
        description = description_template_one.format(author_1=author_1,
                                                      artifacts=artifacts_md)

    # Outputs the release data in a dictionary object
    return {
        'name': 'C++ Final Project',
        'tag_name': tag_name,
        'ref': 'master',
        'description': description
    }


if __name__ == "__main__":
    project_name = get_project_name()
    print("Creating new release for", project_name)
    project = get_gitlab_project(project_name)

    # 1. Download Artifacts
    artifacts = download_latest_artifacts(project)
    assert artifacts, "artifacts from latest deploy job empty"

    # 2. Push artifacts to long-term archive repository
    artifacts_md = project.upload('artifacts.zip',
                                  filedata=artifacts)['markdown']

    # 3. Make a new release
    release_data = get_release_data(artifacts_md)
    try:
        project.releases.create(release_data, timeout=10)
    except gitlab.exceptions.GitlabCreateError as reason:
        print("[ERROR] Couldn't create Release, reason:", reason)

    release_url = project.attributes['web_url'] + '/-/releases'
    print("Successfully created new release, please check:", release_url)
