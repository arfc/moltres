-   [General Notes](#general-notes)
-   [Issuing a Pull Request](#issuing-a-pull-request)
-   [Reviewing a Pull Request](#reviewing-a-pull-request)
-   [Running Tests](#running-tests)
-   [Cautions](#cautions)
-   [An Example](#an-example)
    -   [Introduction](#introduction)
    -   [Acquiring Moltres and Workflow](#acquiring-moltres-and-workflow)
    -   [See also](#see-also)
-   [Releases](#releases)

------------------------------------------------------------------------

# The developer Workflow 

## General Notes

-   The terminology we use is based on the
    [Integrator Workflow ](http://en.wikipedia.org/wiki/Integrator_workflow).

-   Use a branching workflow similar to the one described
    in the [progit gitbook](http://progit.org/book/ch3-4.html).

-   Keep your own "master" and "develop" branches in sync with the
    main repository's "master" and "develop" branches. Specifically,
    do not push your own commits directly to your "master" and
    "develop" branches.

-   Any commit should *pass all tests* (see [Running Tests](#running-tests)).

-   See the [An Example](#an-example) section below for a full walk through.

-   In addition to a review of the algorithmic and logical changes in
    your contribution, it will be reviewed on a variety of levels
    including such things as style, documentation, tests, etc. While
    such review can appear tedious, these aspects are important for the
    long term success of the project.

## Issuing a Pull Request

-   When you are ready to move changes from one of your topic branches
    into the "develop" branch, it must be reviewed and accepted by
    another developer.

-   You may want to review this
    [tutorial](https://help.github.com/articles/using-pull-requests/)
    before you make a pull request to the develop branch.

## Reviewing a Pull Request

-   Look over the code.

-   Check that it meets the [MOOSE style guidelines](http://mooseframework.org/wiki/CodeStandards/).

-   Make inline review comments concerning improvements.

-   Wait for the Continuous Integration service to show full test
    passage.

-   Click the green "Merge Pull Request" button.

-   Note: if the button is not available, the requester needs to merge
    or rebase from the current HEAD of the blessed's "develop"
    (or "master") branch.

## Running Tests

You can run the tests yourself using: - for Moltres:
To ensure that Moltres is functioning properly, run  the following from the
root of the Moltres directory.

```bash
./run_tests -j8 
```

## Cautions

-   **NEVER** merge the "master" branch into the "develop" branch.
    Changes should only flow *to* the "master" branch *from* the
    "develop" branch.

## An Example

### Introduction

As this type of workflow can be complicated to converts from SVN and
very complicated for brand new programmers, an example is provided.

For the sake of simplicity, let us assume that we want a single
"sandbox" branch in which we would like to work, i.e. where we can store
all of our work that may not yet pass tests or even compile, but where
we also want to save our progress. Let us call this branch "Work". So,
when all is said and done, in our fork there will be three branches:
"master", "develop", and "Work".

### Acquiring Moltres and Workflow

We begin with a fork of the main Moltres repository. After initially forking the
repo, we will have two branches in our fork: "master" and "develop".

#### Acquiring a Fork of the Moltres Repository

A fork is *your* copy of Moltres. Github offers an excellent
[tutorial](http://help.github.com/fork-a-repo/) on how to set one up.
The rest of this example assumes you have set up the "upstream"
repository as `moltres/core`. Note that git refers to your fork as
"origin".

First, let's make our "work" branch: :: .../moltres\_dir/\$ git branch
work .../moltres\_dir/\$ git push origin work

We now have the following situation: 

- there exists the main copy of the master and develop branches, 
- there exists your fork's copy of the master, develop, and Work branches, 
 -*AND* there exists your *local* copy of the master, develop, and Work branches. 

It is important now to note that you may wish to work from home or the office.
If you keep your fork's branches up to date (i.e., "push" your changes before
you leave), only your *local* copies of your branches may be different when you
next sit down at the other location.

### Workflow: The Beginning

Now, for the workflow! This is by no means the only way to perform this
type of workflow, but I assume that you wish to handle conflicts as
often as possible (so as to keep their total number small). Let us
imagine that you have been at work, finished, and successfully pushed
your changes to your *origin* repository. You are now at home and want
to continue working a bit. To begin, let's update our *home's local
branches*. ::

```
    .../moltres_dir/$ git checkout develop
    .../moltres_dir/$ git pull upstream develop
    .../moltres_dir/$ git push origin develop

    .../moltres_dir/$ git checkout work
    .../moltres_dir/$ git pull origin work
    .../moltres_dir/$ git rebase develop
    .../moltres_dir/$ git push origin work
```

Perhaps a little explanation is required. We first want to make sure
that this new local copy of the develop branch is up-to-date with
respect to the remote origin's branch and remote upstream's branch. If
there was a change from the remote upstream's branch, we want to push
that to origin. We then follow the same process to update the work
branch, except:

1.  we don't need to worry about the *upstream* repo because it doesn't
    have a work branch, and
2.  we want to incorporate any changes which may have been introduced in
    the develop branch update.

#### Workflow: The End

As time passes, you make some changes to files, and you commit those
changes (to your *local work branch*). Eventually (hopefully) you come
to a stopping point where you have finished your project on your work
branch *AND* it compiles *AND* it runs input files correctly *AND* it
passes all tests! Perhaps you have found Nirvana. In any case, you've
performed the final commit to your work branch, so it's time to make a
pull request online and wait for our developer friends to review and
accept it.

Sometimes, your pull request will be held by the reviewer until further
changes are made to appease the reviewer's concerns. This may be
frustrating, but please act rationally, discuss the issues on the GitHub
space made for your pull request, consult the
[style guide](http://moltres.github.com/devdoc/style_guide.html),
email the developer listhost for further advice, and make changes to
your topic branch accordingly. The pull request will be updated with
those changes when you push them to your fork. When you think your
request is ready for another review, you can reopen the review yourself
with the button made available to you.

#### See also

A good description of a git workflow with good graphics is available at
[nvie](http://nvie.com/posts/a-successful-git-branching-model/).

This contributor document was isnpired by the one written by the [Cyclus team
](github.com/cyclus/cyclus).
