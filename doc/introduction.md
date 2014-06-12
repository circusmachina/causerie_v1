**causerie v1.0**: A simple, elegant and powerful programming language.  
Copyright (C) 2014 by [Michael Malicoat][5]  
[Home page](http://causerie.circusmachina.com) |
[Source][] |
[Source documentation](http://www.circusmachina.com/projects/causerie/doc/html/index.html) (you're looking at it) |
[Bug tracker](http://mantis.circusmachina.com)

Introduction
------------
Causerie is a programming language, derived from [Smalltalk][], that is 
designed to be *simple*, *elegant*, and *powerful*.  You may ask why the world
needs another programming language, and I would answer that, based on my own
[explorations][1] of the available languages, I have yet to find one that 
delivers on all three of the above ideas.  Smalltalk comes close but, 
unfortunately, it has so far been rendered as an interpreted language only, and
I want to write binary executables.  Besides these reasons, it's also a 
challenge: can I write a computer language?  And can I write one that delivers
on the ideas of simplicity, elegance, and power?  We'll find out!

The initial implementation of causerie is written using [Free Pascal][] and
[Lazarus][].  Free Pascal was chosen for a couple of reasons: first, I tried 
writing the classes required in pure C with the help of [GLib][]/[GObject][]
but, even though I find GLib and GObject to be admirable for their depth of
documentation and fairly clean code, actually *using* it still proves to be
somewhat cumbersome, even with the help of the [C preprocessor][2].  Second,
I *didn't* want to write it in C++.  Yes, I'm biased against C++; there is no
denying that it's a widely-used (if not popular) language, but its adherents
seem loath to document anything and its behavior of templates, multiple 
inheritance, and allowing operators to be overloaded at will, together with its
adherents' abhorrence of documentation, makes sifting through someone else's C++ 
code a lot like trying to decipher some deranged wizard's arcane scrawl.  That 
way lies screaming madness.  I know -- I've had to do it.  I imagine many of you 
have, too.

Free Pascal allows the code to be well-structured, if still a bit cumbersome.
I set out to write the code with profuse documentation and, although this tends
to slow progress a little, it ensures that I can go back and figure out what
the *expletive* I was thinking when I wrote a particular routine.  It should
prove helpful to you, too, if you're interesting in things like parser 
internals -- at least, I hope so.  If you find an error or omission, or if
something is not quite explained to your satisfaction, even after consulting
the documentation here, the [causerie wiki][3], or the [various articles][4]
on my site, please [drop me a line][5].  All serious inquiries will be answered
in due time.

Now, a few words on what you can expect to find in the [Source][].

## A word on the Source Code ##
### Class inheritance ###
Nearly everything derives from [`AnObject`][6].  I could just as easily have 
derived from `TObject`, but I didn't.  The main reason is that I dislike the 
Borland style of beginning all types, including classes, with a `T`.  In fact, I 
refuse to follow any of the Borland naming schemes where classes are concerned;
constructors and methods are named something that makes sense in context -- for 
example, [`AStringLeaf.named('blah')`][7] as opposed to 
`TStringLeaf.create('blah')`.  `T` is reserved when declaring a type that is not 
a class, such as [`TIOStreamOutputSize`][8] or [`TSymbolReference`][9].

I suppose that brings us to naming conventions.

### Naming conventions ###
Classes are named in a way that makes sense in context.  Do I want to create
an object?  I use `AnObject`.  Do I need a parser for source code?  I use
[`ASourceParser`][10].  Methods are likewise named.  Do I need a file stream 
that writes to a file?  I call [`AFileOutputStream.toFileNamed()`][11].

Classes and variables that represent class instances are always capitalized.
Custom types are also capitalized.  Functions that return classes or class
instances are capitalized.  Everything else begins with a lower-case letter and
uses camelCase, if necessary.

### Every method is virtual ###
Every method is virtual.  This makes the code flexible.  The trade-off in 
calling time is fairly negligible and certainly worth the added flexibility.
Causerie's methods are likewise all virtual.

### Properties, and the lack thereof ###
No, I don't use properties at all.  You may notice that properties are also 
absent from causerie's language definition, even though it compiles for use with
`GObject`, and `GObject` supports them.  The reason is simple enough: they're 
wasteful and largely unnecessary.

Once upon a time, I used properties liberally to prevent other programmers from 
modifying values and so forth -- but I found, through practice, that defining a 
property and then a couple of protected methods to access and alter such a value 
not only cluttered up the class declaration, but was less efficient, from a 
coding time standpoint, than simply defining the accessor and mutator methods 
up-front -- especially where indexed properties (the ones that pretend to be 
arrays) are concerned.

The standard I've followed for dealing with properties is as follows: given a 
protected value named `myValue`, the accessor is named `value` and the mutator, 
if any, is named `setValue`.  Both the accessor and mutator methods are 
functions that return the same type of value.  In the case of the mutator, the 
value returned is the *previous* value of `myValue`, in case the caller wants to
save it and restore it at a later point in time.  If there is no method named
`setValue`, then `myValue` is a read-only property that cannot be modified.

Much simpler than having a `property` declaration that clutters up the
landscape.  Do you pay a small penalty for having to call a function?  No more
than if your property has to do the same thing (as it must for indexed
properties).

### Odds and ends ###
And now, some odds and ends:

* Yes, the initial version of causerie uses a recursive-descent parser.  I
  don't want to hear about it.  If it's good enough for [SpiderMonkey][12], then
  it's good enough for causerie.  Besides, those of you who are really paying
  attention will notice that the code has been written in such a way that making
  the parser table-driven in the future can be done without too much effort...
  
That's all there is to say for now.  Have a look through the documentation, 
check out the wiki and the source, and let me know what you think!

[Free Pascal]: http://www.freepascal.org
[GLib]: http://library.gnome.org/devel/glib/stable/
[GObject]: http://library.gnome.org/devel/gobject/stable/
[Lazarus]: http://www.lazarus.freepascal.org
[Smalltalk]: http://en.wikipedia.org/wiki/Smalltalk
[Source]: https://github.com/circusmachina/causerie_v1
[1]: http://www.circusmachina.com/gladius/Code/Adventures-in-Computer-Linguistics
[2]: http://www.circusmachina.com/gladius/Code/Writing-Object-Oriented-Code-with-GLibGObject
[3]: http://causerie.circusmachina.com
[4]: http://www.circusmachina.com
[5]: http://www.circusmachina.com/gladius/Profiles/Michael
[6]: http://www.circusmachina.com/projects/causerie/doc/html/classwork.AnObject.html
[7]: http://www.circusmachina.com/projects/causerie/doc/html/classwork.AStringLeaf.html#named
[8]: http://www.circusmachina.com/projects/causerie/doc/html/classwork.html#TStreamIOSize
[9]: http://www.circusmachina.com/projects/causerie/doc/html/parsing.TSymbolReference.html
[10]: http://www.circusmachina.com/projects/causerie/doc/html/parsing.ASourceParser.html
[11]: http://www.circusmachina.com/projects/causerie/doc/html/classwork.AFileOutputStream.html#toFileNamed
[12]: https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey/Internals
