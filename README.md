LeekScript language
===================

[![Build Status](https://travis-ci.org/leek-wars/leekscript.svg?branch=master)](https://travis-ci.org/leek-wars/leekscript)
[![Coverage Status](https://coveralls.io/repos/github/leek-wars/leekscript/badge.svg?branch=master)](https://coveralls.io/github/leek-wars/leekscript?branch=master)
[![GPL Licence](https://badges.frapsoft.com/os/gpl/gpl.svg?v=103)](https://opensource.org/licenses/GPL-3.0/)
[![HitCount](https://hitt.herokuapp.com/leek-wars/leekscript.svg)](https://github.com/leek-wars/leekscript)

LeekScript is a language initially designed for Leek Wars, and for games in general. Design to be simple to use for beginners, but with rich syntax and functionnalities, and a good speed thanks to an optimized JIT compiler.

The project is still in development, and shouldn't be used for the moment.


Demo
----

https://leekscript.com

Building
--------

```
make
```
The executable `leekscript` is in the `build/` folder.

Export as shared library
```
make lib
```
`libleekscript.so` will be in `build/` folder. You can `make install` to copy it in `/usr/lib/`.

Usage
-----

Run a LeekScript top-level
```
./leekscript
```

Run the tests
```
./leekscript -test
```

Execute a file
```
./leekscript -f my_file.ls
```

Run a code, and get the result as JSON
```
./leekscript -e "my code" "{}"
```

Libraries used
--------------
* [GNU LibJIT](https://www.gnu.org/software/libjit/), by Rhys Weatherley, under GPL license
* [GNU MP](https://gmplib.org/), by Torbjörn Granlund, under GPL license
* [JSON for modern C++](https://github.com/nlohmann/json), by Niels Lohmann, under MIT license

License
-------

Distributed under the GPL3 license. Copyright (c) 2016, Pierre Lauprêtre
