# Contributing to NetPanzer

## Bug Reports and Feature Requests

Anyone may [open an
issue](https://github.com/netpanzer/netpanzer/issues).

## Discussion

If you're not sure if something is a bug, you can search, or post on, the
[Discussions](https://github.com/netpanzer/netpanzer/issues).

### Discord

You may participate on our [Discord server](https://discord.gg/kCAB2CMswd).
It's important to remember that any discussions about bugs or desired features
in a chat room usually scroll away and become forgotten, so it's best to
create a ticket or discussion on GitHub (mentioned above) if you really want a
bug fix or new feature.

## Coding Standards

Our goal is to use the [Google C++ Style
Guide](https://google.github.io/styleguide/cppguide.html).

A `.clang-format` file is located in `src`.

You can format code automatically by using
[clang-format](https://clang.llvm.org/docs/ClangFormat.html) with the
following arguments:

    clang-format TBD

## Website

TBD

## Translating

TBD

## Patches and Pull Requests

To prevent work-overlap, please post on a ticket if you'll be working
on a specific issue (or create a ticket if there's not one open yet.
**Note**: If more than one person submits a patch for the same thing,
your patch may get rejected.

**Note**: If you agreed to work to work on a ticket but later find that
you're unable to work on it, or if you changed your mind, please post
again on the ticket to let everyone know it's up for grabs.

You can use [The GitHub
flow](https://guides.github.com/introduction/flow/), which mostly just
involves creating a separate branch for each patch you're working on.
Using that method helps prevent merge conflicts later. *Note* that you
should never need to work on the master branch or merge your patches
into the master branch (don't forget to periodically [sync your
repo](https://docs.github.com/en/github/collaborating-with-pull-requests/working-with-forks/syncing-a-fork).)

Source code patches should only contain changes related to a single
issue. This helps speed up the review and discussion process.

### Commit Messages

Different projects have different guidelines. If you're not sure what to write
for a commit message on this project, [view the commit
log](https://github.com/netpanzer/netpanzer/commits/master/) to see previous
ones.

## Documentation

Documentation should wrap at column 79 when possible (this makes it easier to
edit and review).

If you're fixing typos and grammar errors in documentation, multiple changes
in one PR is fine.
