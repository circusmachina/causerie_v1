{ parsing.pp - A library of basic parser classes useful in larger projects.
  Copyright (C) 2011-2013 Michael Malicoat.

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

{$SMARTLINK ON}

{ This unit defines several classes to handle:
    @unorderedlist(
      @item(lexer definition: @link(AParsedLanguage) and its descendants)
      @item(source scanning: @link(AToken), @link(AScanner) and their
            descendants)
      @item(parsing: @link(AParser), @link(ASourceParser) and their descendants)
      @item(statement handling: @link(AStatement) and its descendants)
      @item(symbols and symbol tables: @link(ASymbol), @link(ASymbolTable) and
            their descendants)
      @item(intermediate code streams: @link(ABookmarkedStream) and its
            descendants)
  )

  All of this functionality routinely serves as the basis for my projects, and
  so I have now condensed what were several units into one for increased
  portability between projects.
}
unit parsing;

{$mode objfpc}{$H+}

interface

uses
  classwork;

{$DEFINE ReadInterface}
{$I opcodes.inc}
{$I syntax.inc}
{$I languages.inc}
{$I symbols.inc}
{$I symbolcollections.inc}
{$I tokens.inc}
{$I tokenlists.inc}
{$I sourcetokens.inc}
{$I scanners.inc}
{$I sourcescanners.inc}
{$I sourcestreams.inc}
{$I parsermessages.inc}
{$I parsers.inc}
{$I parsedlanguageparsers.inc}
{$I symbolparsers.inc}
{$I sourceparsers.inc}
{$I statements.inc}
{$I sourcestatements.inc}

{$UNDEF ReadInterface}

resourcestring
  {$DEFINE ReadResourceStrings}
  {$I tokens.inc}
  {$I parsermessages.inc}
  {$I parsers.inc}
  {$I symbolparsers.inc}
  {$UNDEF ReadResourceStrings}

implementation

uses
  sysUtils, variants;

{$DEFINE ReadImplementation}
{$I opcodes.inc}
{$I syntax.inc}
{$I languages.inc}
{$I symbols.inc}
{$I symbolcollections.inc}
{$I tokens.inc}
{$I tokenlists.inc}
{$I sourcetokens.inc}
{$I scanners.inc}
{$I sourcescanners.inc}
{$I sourcestreams.inc}
{$I parsermessages.inc}
{$I parsers.inc}
{$I parsedlanguageparsers.inc}
{$I symbolparsers.inc}
{$I sourceparsers.inc}
{$I statements.inc}
{$I sourcestatements.inc}

{$UNDEF ReadImplementation}

end.

