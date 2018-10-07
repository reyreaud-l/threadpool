Changelog
=========

All notable changes to this project will be documented in this file.

The format is based on `Keep a Changelog <https://keepachangelog.com/en/1.0.0/>`_,
and this project adheres to `Semantic Versioning <https://semver.org/spec/v2.0.0.html>`_.

Unreleased
----------
Changed
~~~~~~~
- Remove hook copy overload (it was wrong)

[2.0.0] - 2018-09-21
--------------------
Added
~~~~~~~
- Code examples from the doc are now buildable standalone programs.
- Tasks hooks.
- Workers hooks.
- Documentation generation with breath and sphinx.
- Deploy documentation on read the doc.
- Uninstall target in CMake.
- Changelog section in documentation.

Changed
~~~~~~~
- CI now check linux/osx with multiples compiler.
- Documentation is now hosted on read the doc. The documentation now also
  includes doxygen using breathe sphinx plugin.

[1.0.0] - 2018-06-26
--------------------
Added
~~~~~
- First release (!yay).
- Fixed/Variable pool size.
- Fetch result of task with futures.
- Fetch number of waiting/working workers.

List of releases
----------------
- `Unreleased <https://github.com/reyreaud-l/threadpool/compare/v1.0.0...HEAD>`_
- `2.0.0 <https://github.com/reyreaud-l/threadpool/compare/v1.0.0...v2.0.0>`_
- `1.0.0 <https://github.com/reyreaud-l/threadpool/compare/3e51279...v1.0.0>`_
