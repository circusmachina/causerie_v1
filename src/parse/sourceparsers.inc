{ sourceparsers.inc: A class to parse a source stream, using a language
    definition and symbol tables, into an intermediate code stream.

  This file is part of parsing.pp, a library of basic and useful parser classes.

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 3 of the License or, at your
  option, any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABLITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License along
  with this program.  If not, see <http://www.gnu.org/licenses/>.
}
{$IFDEF ReadInterface}
{ This class represents a parser that uses a language definition to parse an
  arbitrary source stream into intermediate code.  Symbols (identifiers and
  constants) that are encountered by the parser are destined to be entered into
  its symbol tables.

  You will likely not create a direct instance of this class, as calling
  its @code(parse) method does not do much that is useful.  Instead, you will
  derive a custom class from this one.

  When creating an instance of @link(ASourceParser), you are expected to
  provide:
  @unorderedList(
    @item(an output stream)
  )
  as well as the items required by @link(ASymbolParser) before calling the
  @code(parse) method.
}
type ASourceParser = class(ASymbolParser)
  protected
    { Refers to the stream used by the parser for output. }
    MyOutput: AStream;

  public
    // Initializer
    function init: boolean; override;

    {
