{ sourceparsers.inc: A class to parse a source stream using a language
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
}
type ASourceParser = class(ASymbolParser)
  protected
    { Refers to the stream used by the parser for output. }
    MyOutput: AStream;

  public
    { Initializer }
    function init: boolean; override;

    { Execute the parser.

      This method builds on the behavior inherited from
      @link(ASymbolParser.parse): it first calls the inherited routine, then
      checks to ensure that @link(ASourceParser.Output) is valid (not @nil).
      If @link(ASourceParser.Output) is @nil, then this method returns -1;
      otherwise, it returns zero (0).

      You will want to override this method when deriving from
      @link(ASourceParser) so that it does actual work.
    }
    function parse: int64; override;

    { Set the parser's output stream and execute the parser.

      This method sets the value of @link(ASourceParser.Output) to
      @code(Dest) and then calls @link(ASourceParser.parse).  The result of
      that call is returned to the caller.

      If @code(Dest) is invalid (@nil), then this routine does nothing and
      returns -1.

      @return(Zero (0) on success.  A negative value means an error occurred
      that prevented parsing from completing successfully; a positive value
      indicates the number of syntax errors that were encountered while
      parsing.)
    }
    function parseInto(const Dest: AStream): int64; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the output stream used by the parser.

      This is the stream to which syntax-checked code is written and will most
      likely represent an intermediate code stream of some kind.

      The caller should NOT free the reference returned by this routine.
    }
    function Output: AStream; virtual;

    { Set the output stream used by the parser.

      In most cases, @code(Dest) will refer to a binary stream which is used
      to capture raw tokens as they are sucessfully parsed from a source
      stream.

      @return(The previous value of @link(ASourceParser.Output).)
    }
    function SetOutput(const Dest: AStream): AStream; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Initializer
function ASourceParser.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyOutput := nil;
  end;
end;

// Parse the source
function ASourceParser.parse: int64;

begin
  // Call the inherited routine first
  result := inherited parse;

  if result <> 0 then
    exit;

  // Check to see whether we have a valid output stream
  if MyOutput = nil then
  begin
    // Check to see if we can inherit from the parent
    if (MyParent <> nil) and (MyParent is ASourceParser) then
      MyOutput := ASourceParser(MyParent).Output

    else
      result := -1;
  end;
end;

// Parse the source into the specified output stream
function ASourceParser.parseInto(const Dest: AStream): int64;

begin
  result := -1;

  // Only parse if the destination stream is valid
  if Dest <> nil then
  begin
    MyOutput := Dest;

    result := Self.parse;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the output stream
function ASourceParser.Output: AStream;

begin
  Result := MyOutput;
end;

// Set the output stream to use
function ASourceParser.SetOutput(const Dest: AStream): AStream;

begin
  // Return the old value
  Result := MyOutput;

  MyOutput := Dest;
end;
{$ENDIF}

