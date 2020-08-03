# Make a Release

<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [Make a Release](#make-a-release)
  - [Final Project: Make a Release](#final-project-make-a-release)
  - [Deploy Artifacts](#deploy-artifacts)
  - [Instructions](#instructions)

<!-- /code_chunk_output -->

## Final Project: Make a Release

In order to submit your final presentation for the C++ Final project you
**must** make a release in your repository. This way all your code and the build
artifacts(binaries + libraries) will be safely saved in a snapshot of your
repository. You can cleanup your code after the submission, but the release you
make will the one we will consider for the evaluation.

Please be aware that all the changes you make in your releases are being tracked
by GitLab, so if you change the Release after the submissions, we will know.

## Deploy Artifacts

You might need to update your `.gitlab-ci.yml` configuration file to tell the CI
that you want to store the artifacts of the build. In this step you must also
specify for how long you want the CI to keep these artifacts. Usually 1 week is
ok.

You **need** to have a "_deploy_" stage in your pipeline:

```yml
deploy:
  stage: deploy
  dependencies:
    - build
  script:
    - echo "Build succeeded, archiving artifacts..."
    - cd build/
    - make -j$(nproc --all) install
  artifacts:
    name: bow_artifacts
    expire_in: 1 week
    paths:
      - results/
```

The build artifacts from the gitlab CI will be usually removed after 1
week(depending on the `.gitlab-ci.yml` configuration file) In order to make sure
that your submission will be safely stored we need to put this artifacts into a
long-term storage place in your project. For this we you need to manually
download the latest artifacts and run some script we've created to make this
task less painful.

There are many ways of doing this. The simplest one in our case would be to
download the latest artifacts from your repository and push this artifacts to a
long-term archive repository. Be aware that this artifacts **can't be** removed
at any point in time by anyone. So please consider if everything is working
before doing this.

## Instructions

1. First we need a token to access our gitlab projects from the shell, for this
   follow
   [these](https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html#creating-a-personal-access-token)
   instructions and then you have 2 options:
   1. copy the token to your `~/.bashrc` (**recomendded**)

```sh
echo "export IGG_GITLAB_TOKEN=RQyayyx92LnqtVk_zm1B" >> ~/.bashrc
```

2.  Do it once in the running shell:

```sh
export IGG_GITLAB_TOKEN=RQyayyx92LnqtVk_zm1B # this is your token
```

2. First make sure your latest pipeline succeed
3. Run the script on your terminal and check that there is no error messages

```sh
./make_release.sh
```

After you made the release you can go to your repository and check that
everything is ok:
![release_example](release_example.png)

Now we can access your source code and binaries at any point in time. Just make
sure this binaries are updated (with the help of the CI) and that everything
runs properly. If your binaries doesn't work the submissions will be discarded.

Make sure there are instructions on your **README.md** on your local repository
