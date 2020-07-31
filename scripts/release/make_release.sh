#!/usr/bin/env bash
# @file      upload_artifacts.sh
# @author    Ignacio Vizzo     [ivizzo@uni-bonn.de]
#
# Copyright (c) 2020 Ignacio Vizzo, all rights reserved

cat <<EOF
Welcome to the magic script to make release, you only need to provide the
comlpete name of the authors of the project. If you are only 1 author just hit
enter when asked for the 2nd author
EOF

read -p "Enter first  author name and surname: " AUTHOR_1
read -p "Enter second author name and surname: " AUTHOR_2

SURNAME_1="$(echo $AUTHOR_1 | awk '{print $NF}')"
SURNAME_2="$(echo $AUTHOR_2 | awk '{print $NF}')"

# Get PROJECT_ID from the current git repository
GITLAB_URL="https://gitlab.igg.uni-bonn.de/api/v4/projects/"
PROJECT_URL=$(git remote -vv | tail -n1 | awk '{print $2}' | tr -d '\n')
STUDENT_NAME=$(echo "$PROJECT_URL" | awk -F\[/:.\] '{print $5}')
PROJECT_NAME=$(echo "$PROJECT_URL" | awk -F\[/:.\] '{print $6}')
PROJECT_ID=$(curl --silent \
  --header "PRIVATE-TOKEN: $IGG_GITLAB_TOKEN" \
  --request GET \
  $GITLAB_URL/${STUDENT_NAME}%2F${PROJECT_NAME} |
  jq '.id')

# Download latest artifacts to your local machine:
echo "Downloading Artifacts from Project ID = $PROJECT_ID..."
curl --silent \
  --header "PRIVATE-TOKEN: $IGG_GITLAB_TOKEN" \
  --output artifacts.zip \
  $GITLAB_URL/$PROJECT_ID/jobs/artifacts/master/download?job=deploy

# Upload this artifacts, check for the output!!!
echo "Pushing the just downloaded artifacts.zip to project long-term storage..."
ARTIFACTS=$(
  curl --silent \
    --header "PRIVATE-TOKEN: $IGG_GITLAB_TOKEN" \
    --form "file=@artifacts.zip" \
    $GITLAB_URL/$PROJECT_ID/uploads |
    jq '.'markdown |
    tr -d '"'
)

# Make the data_json file
NAME="C++ Final Project"
TAG_NAME="2020_${SURNAME_1}_${SURNAME_2}"
REF="master"
read -r -d '' DESC <<EOF
### Authors
 - ${AUTHOR_1}
 - ${AUTHOR_2}

### Latest Artifacts
 - ${ARTIFACTS}
EOF

DATA=$(jq -n \
  --arg name "$NAME" \
  --arg tag "$TAG_NAME" \
  --arg ref "$REF" \
  --arg desc "$DESC" \
  '{name: $name, tag_name: $tag, ref: $ref, description: $desc}')

# Make Release
echo "Making new release with the following data:"
echo $DATA | jq '.'
echo "Server response:"
curl --silent \
  --header 'Content-Type: application/json' \
  --header "PRIVATE-TOKEN: $IGG_GITLAB_TOKEN" \
  --data "${DATA}" \
  --request POST \
  $GITLAB_URL/$PROJECT_ID/releases/ | jq '.'

echo "Check https://gitlab.igg.uni-bonn.de/$STUDENT_NAME/$PROJECT_NAME/-/releases"
