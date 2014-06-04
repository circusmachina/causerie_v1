causerie v1.0: A simple, elegant and powerful programming language.  
Copyright (C) 2014 by [Michael Malicoat](mailto:michael@circusmachina.com)  
[Home page](http://causerie.circusmachina.com) |
[Source](https://github.com/circusmachina/causerie_v1) |
[Bug tracker](http://mantis.circusmachina.com)

This file was last modified by Michael Malicoat on 3 June 2014.

## License ##
This program is free software: you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
this program.  If not, see [http://www.gnu.org/licenses/]().

## Overview ##
In its initial inception (this one, the one you are looking at right now),
causerie will be more of a glorified preprocessor than an actual compiler.
Nevertheless, it will successfully compile code from its own Smalltalk-derived
syntax, to C, and then to a finished program.

For more information, please see the [Wiki](http://causerie.circusmachina.com).

## Compiling ##
### With Lazarus ###
The initial version of causerie is written in [Free Pascal][] using [Lazarus][].
The Lazarus project files are found in the top-level directory (the same 
directory as this file) and, when finished, will allow you to build causerie 
from source.  Simply open `cause.lpi` in lazarus, then choose `Build` from the 
`Run` menu (or press `Shift+F9` as of Lazarus 1.2.2).

On Windows, Lazarus will be the preferred method of building causerie from 
source.

### With GNU `MAKE` ###
If you do not have Lazarus, or would prefer not to use it, you will be able to
build causerie using a provided `Makefile`; note, however, that you will need
Free Pascal and GNU `MAKE` installed for this to work.  This option makes use
of `fpcmake`, which I haven't used before, but based on the available options
as described in the online manual, it does not appear that this option will 
produce directory names that are as organized; it does not seem that the 
automatically-generated `Makefile` will be able to place the compiled units and 
the final binary in a subfolder of `lib/` or `build/` that indicates the target
platform.  The end result should be the same, however.

The initial version of causerie was built with Free Pascal 2.6.4, so you will
need at least this version to compile.  It may be that older versions will also
successfully compile causerie, but I have not tested this myself.  In the
future, causerie will compile itself, so hopefully this will not be a matter of
concern for too long.

## Installing ##
This cannot yet be done, as only the base library is finished and the parsing
library is nearing completion.  This file will be updated as we get closer to
a functional program.

[Free Pascal]: http://www.freepascal.org
[Lazarus]: http://www.lazarus.freepascal.org/
