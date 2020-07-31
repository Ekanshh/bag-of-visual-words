# @file      upload_artifacts.sh
# @author    Ignacio Vizzo     [ivizzo@uni-bonn.de]
#
# Copyright (c) 2019 Ignacio Vizzo, all rights reserved
#
# https://docs.gitlab.com/ce/api/projects.html#upload-a-file

# Use your own ID here
PROJECT_ID=853

# Download latest artifacts to your local machine:
curl --silent \
  --header "PRIVATE-TOKEN: $IGG_GITLAB_TOKEN" \
  --output artifacts.zip \
  "https://gitlab.igg.uni-bonn.de/api/v4/projects/$PROJECT_ID/jobs/artifacts/master/download?job=deploy"

# Upload this artifacts, check for the output!!!
echo "PLEASE MAKE SURE YOU COPY THIS LINE INTO THE release_data.json FILE:"
echo "========================================================================"
curl --silent \
  --header "PRIVATE-TOKEN: $IGG_GITLAB_TOKEN" \
  --form "file=@artifacts.zip" \
  "https://gitlab.igg.uni-bonn.de/api/v4/projects/$PROJECT_ID/uploads" |
  jq '.'markdown
echo "========================================================================"
