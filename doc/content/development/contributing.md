# Contributing

The contributing guidelines here are mostly adapted from
[MOOSE contributing guidelines](https://mooseframework.inl.gov/framework/contributing.html) and
[Commit Often, Perfect Later, Publish Once: Git Best Practices](https://sethrobertson.github.io/GitBestPractices/)
by Seth Robertson (CC BY-SA 3.0).

## Code Standards

Like MOOSE, Moltres uses the ClangFormat for formatting all C++ code in the repository. If you
ever want to contribute changes to the Moltres repository, make you run the following command
to automatically format any new code in your current branch relative to the supplied `<branch>`
(defaults to HEAD if omitted).

```bash
git clang-format [<branch>]
```

The configuration file for the formatting is provided in the `.clang-format` file in the base
directory. Refer to MOOSE [Code Standards](https://mooseframework.inl.gov/sqa/framework_scs.html)
for more details on the code standard.

## Referencing Issues

In every pull request, we strongly recommend referencing an issue. This practice encourages robust
discussions on whether the change is necessary and appropriate to Moltres. It also provides
relevant context to the new changes proposed for quicker code review.

## Work in a Fork

Create your own [fork](https://docs.github.com/en/get-started/quickstart/fork-a-repo) where you can
make and commit your set of changes.

### 1. Fork Moltres

- Navigate to [https://github.com/arfc/moltres](https://github.com/arfc/moltres)
- Click the "Fork" button near the upper right corner of the page.
- Clone your new fork to your local machine using the following command (replace "username" with
  your GitHub username):


```bash
git clone git@github.com:username/moltres.git
```

### 2. Add the `upstream` Remote

Add the original Moltres repository as a remote named `upstream`:

```bash
cd moltres
git remote add upstream git@github.com:arfc/moltres.git
```

### 3. Make Changes to Moltres

Create a branch for your work:

```bash
git checkout -b branch_name
```

Make your changes and commit them to the branch you just created:

```bash
git add your_new_file.C your_new_file.h
git commit -m "Your commit message here"
```

Ensure that your commit messages are insightful and descriptive such that they let people quickly
understand your changes without having to read code. *Commit early and often* to create checkpoints
that you can revert to if you break something while implementing your changes. Frequent commits
also helps to keep your commits short and sweet.

If you have been working on your changes for some time, there may have been new updates to Moltres
implemented by others since the last time you pulled from the `upstream` remote. Merge those
updates into your current branch by running the following commands while your current branch is
checked out.

```bash
git fetch upstream
git merge upstream/devel
```

!alert tip
Refer to
[Commit Often, Perfect Later, Publish Once: Git Best Practices](https://sethrobertson.github.io/GitBestPractices/)
for an in-depth guide on Git workflows.

### 4. Update Documentation

If you make any changes to the source code which alters functionality, please make
the corresponding changes to the documentation in `moltres/doc/content/source` for the specific
class you modified. You may build this website locally to review your documentation
changes by running the following commands in the `moltres/doc` directory:

```bash
./moosedocs.py build --serve
```

!alert tip
Feel free to reach out to anyone in the Moltres development team for help with documentation.

!alert note
Detailed documentation of Moltres syntax is currently a work in progress. However, feel free to
contribute if you are familiar with the source code.

### 5. Push Your Changes to Your GitHub Fork

Push your branch to your fork on GitHub.

```bash
git push origin branch_name
```

## Create a Pull Request

Go back to the [main Moltres repository](https://github.com/arfc/moltres) and create a pull request
to merge your branch to the Moltres `devel` branch. If you had just pushed your branch to GitHub,
GitHub will display a prompt asking whether you wish to create a new pull request using that
branch. Otherwise, simply select your fork and branch from the dropdown list in the new pull
request page.

## Review & Merge

Someone from the Moltres development team will review your pull request and either request
additional changes or approve your pull request.
