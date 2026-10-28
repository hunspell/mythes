# Security Policy

## Supported versions

Only the latest release is maintained. Fixes go to `master`. If you are
running something older, the answer to a security report is to
upgrade.

## What is in scope

Memory safety problems reachable from ordinary use: a crash, or a read
or write outside a buffer, when looking up a word taken from a document
in a thesaurus installed the normal way.

## What is out of scope

1. A crafted thesaurus. The .idx and .dat files are installed data,
   from a distribution package or from an extension the user chose,
   and an extension can already run code as the user. A bug that needs
   a crafted thesaurus is fixed as an ordinary bug, with no advisory
   and no CVE. If a CVE is assigned for one anyway, we will ask for it
   to be marked disputed.
2. Missing or wrong synonyms. Those belong to the thesaurus for the
   language concerned.
3. High memory use or slow lookups on input built to be pathological.
4. Bugs in the applications that embed MyThes, including callers that
   free a lookup result with a different count than Lookup returned.
   Report those to the application.

One report per root cause. A variant of an issue already reported, or
of a fix under review, earns no separate credit, and a finding against
code that is in no release is fixed on master without an advisory.

## How to report

Use GitHub's private reporting: the Security tab of this repository,
then "Report a vulnerability". That keeps the report private until
there is a fix.

If that does not work for you, mail caolanm@gmail.com.

A report is most useful with the input that triggers it, the version
or commit you tested, and the sanitizer output if you have it.

## What happens next

MyThes has one active maintainer, and there is no fixed timetable.

Most findings are fixed in a public commit once they are understood. A
finding serious enough to need handling differently is handled case by
case.

Please allow 90 days before disclosing publicly.

Reporters are credited unless they ask not to be.
