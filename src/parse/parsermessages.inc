{ parsermessages.inc: Classes to assist in logging status and error messages
    from a parser.

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
// AParserNote *****************************************************************
{ This class represents a note that is logged by a parser to a given instance
  of @link(ALog).  It is defined for convenience, since @link(ALoggedNote)
  could be used instead; however, this class changes the format of the
  message output to the log so that it includes the file name of the source
  being parsed, if available, and the line number within that source.
}
type AParserNote = class(ALoggedNote)
  protected
    { Refers to the scanner used to parse the source }
    MyScanner: AScanner;

  public
    { Construct a new logged parser message.

      When @link(AParserNote.toString) is called, @code(ThisScanner) will be
      queried for the name of the source being parsed and the current line
      number in that source; both of these items will be output along with the
      message constructed from @code(fmt) and @code(params).
    }
    constructor inSource(const ThisScanner: AScanner;
      const fmt: string; const params: array of const); virtual;

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ALoggedItem.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AParserNote).  If so,
      it copies the value of @link(AParserNote.Scanner) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      output to a text-based device, such as a console.

      This method builds on the behavior inherited from @link(ALoggedNote): it
      calls the inherited routine, then prepends the name of the source file
      and the current line number within that source, according to the format
      specified by @link(pmsgStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the scanner that will be queried for source name
      and line information.

      The caller should NOT free this reference.
    }
    function Scanner: AScanner; virtual;
end;

// AParserHint *****************************************************************
{ This class represents a hint that is logged by a parser to a given instance
  of @link(ALog).  It is defined for convenience, since @link(ALoggedHint)
  could be used instead; however, this class changes the format of the
  message output to the log so that it includes the file name of the source
  being parsed, if available, and the line number within that source.
}
type AParserHint = class(ALoggedHint)
  protected
    { Refers to the scanner used to parse the source }
    MyScanner: AScanner;

  public
    { Construct a new logged parser message.

      When @link(AParserHint.toString) is called, @code(ThisScanner) will be
      queried for the name of the source being parsed and the current line
      number in that source; both of these items will be output along with the
      message constructed from @code(fmt) and @code(params).
    }
    constructor inSource(const ThisScanner: AScanner;
      const fmt: string; const params: array of const); virtual;

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ALoggedItem.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AParserHint).  If so,
      it copies the value of @link(AParserHint.Scanner) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      output to a text-based device, such as a console.

      This method builds on the behavior inherited from @link(ALoggedHint): it
      calls the inherited routine, then prepends the name of the source file
      and the current line number within that source, according to the format
      specified by @link(pmsgStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the scanner that will be queried for source name
      and line information.

      The caller should NOT free this reference.
    }
    function Scanner: AScanner; virtual;
end;

// AParserWarning **************************************************************
{ This class represents a warning that is logged by a parser to a given instance
  of @link(ALog).  It is defined for convenience, since @link(ALoggedWarning)
  could be used instead; however, this class changes the format of the
  message output to the log so that it includes the file name of the source
  being parsed, if available, and the line number within that source.
}
type AParserWarning = class(ALoggedWarning)
  protected
    { Refers to the scanner used to parse the source }
    MyScanner: AScanner;

  public
    { Construct a new logged parser message.

      When @link(AParserWarning.toString) is called, @code(ThisScanner) will be
      queried for the name of the source being parsed and the current line
      number in that source; both of these items will be output along with the
      message constructed from @code(fmt) and @code(params).
    }
    constructor inSource(const ThisScanner: AScanner;
      const fmt: string; const params: array of const); virtual;

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ALoggedItem.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AParserWarning).  If so,
      it copies the value of @link(AParserWarning.Scanner) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      output to a text-based device, such as a console.

      This method builds on the behavior inherited from @link(ALoggedWarning):
      it calls the inherited routine, then prepends the name of the source file
      and the current line number within that source, according to the format
      specified by @link(pmsgStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the scanner that will be queried for source name
      and line information.

      The caller should NOT free this reference.
    }
    function Scanner: AScanner; virtual;
end;

// AParserSyntaxError **********************************************************
{ This class represents a syntax error that is logged by a parser to a given
  instance of @link(ALog).  It is defined for convenience, since
  @link(ALoggedError) could be used instead; however, this class changes the
  format of the message output to the log so that it includes the file name of
  the source being parsed, if available, and the line number within that source.
}
type AParserSyntaxError = class(ALoggedError)
  protected
    { Refers to the scanner used to parse the source }
    MyScanner: AScanner;

  public
    { Construct a new logged parser message.

      When @link(AParserSyntaxError.toString) is called, @code(ThisScanner) will
      be queried for the name of the source being parsed and the current line
      number in that source; both of these items will be output along with the
      message constructed from @code(fmt) and @code(params).
    }
    constructor inSource(const ThisScanner: AScanner;
      const fmt: string; const params: array of const); virtual;

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ALoggedItem.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AParserSyntaxError).  If
      so, it copies the value of @link(AParserSyntaxError.Scanner) from
      @code(Other) to @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      output to a text-based device, such as a console.

      This method builds on the behavior inherited from @link(ALoggedError): it
      calls the inherited routine, then prepends the name of the source file
      and the current line number within that source, according to the format
      specified by @link(pmsgStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the scanner that will be queried for source name
      and line information.

      The caller should NOT free this reference.
    }
    function Scanner: AScanner; virtual;
end;

// AParserFatalError ***********************************************************
{ This class represents a fatal error that is logged by a parser to a given
  instance of @link(ALog).  It is defined for convenience, since
  @link(ALoggedFatalError) could be used instead; however, this class changes
  the format of the message output to the log so that it includes the file name
  of the source being parsed, if available, and the line number within that
  source.
}
type AParserFatalError = class(ALoggedFatalError)
  protected
    { Refers to the scanner used to parse the source }
    MyScanner: AScanner;

  public
    { Construct a new logged parser message.

      When @link(AParserFatalError.toString) is called, @code(ThisScanner) will
      be queried for the name of the source being parsed and the current line
      number in that source; both of these items will be output along with the
      message constructed from @code(fmt) and @code(params).
    }
    constructor inSource(const ThisScanner: AScanner;
      const fmt: string; const params: array of const); virtual;

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ALoggedItem.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AParserFatalError).  If
      so, it copies the value of @link(AParserFatalError.Scanner) from
      @code(Other) to @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      output to a text-based device, such as a console.

      This method builds on the behavior inherited from
      @link(ALoggedFatalError): it calls the inherited routine, then prepends
      the name of the source file and the current line number within that
      source, according to the format specified by
      @link(pmsgStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the scanner that will be queried for source name
      and line information.

      The caller should NOT free this reference.
    }
    function Scanner: AScanner; virtual;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This constant determines the format of the string returned by calls to
    @link(AParserNote.toString), @link(AParserHint.toString),
    @link(AParserWarning.toString), @link(AParserSyntaxError.toString), and
    @link(AParserFatalError.toString).

    The first string placeholder is the name of the source stream being parsed,
    as determined by a call to @link(AStream.name).  If the source stream
    represents a file stream, then this will contain the name of the file being
    parsed.  The integer placeholder will be filled with the current line
    number in the source stream, as determined by a call to
    @link(AScanner.lineNumber).  The second string placeholder is filled with
    the error or status message, which is constructed by calling the inherited
    @code(toString) message for each of the above classes:
    @link(ALoggedNote.toString), @link(ALoggedHint.toString),
    @link(ALoggedWarning.toString), @link(ALoggedError.toString), or
    @link(ALoggedFatalError.toString).
  }
  pmsgStringRepresentation = '%s: %d: %s';
{$ENDIF}
{$IFDEF ReadImplementation}
// AParserNote *****************************************************************
// Constructor
constructor AParserNote.inSource(const ThisScanner: AScanner;
  const fmt: string; const params: array of const);

begin
  inherited because(fmt, params);

  MyScanner := ThisScanner;
end;

// Initializer
function AParserNote.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyScanner := nil;
  end;
end;

// Construct a shallow copy of the other object
function AParserNote.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AParserNote) then
    result := false;

  if result then
  begin
    MyScanner := AParserNote(Other).Scanner;
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the item
function AParserNote.toString: AnsiString;

begin
  // Construct a message that includes source name and line number
  if MyScanner <> nil then
    result := SysUtils.format(pmsgStringRepresentation, [
      MyScanner.Source.name, MyScanner.lineNumber,
      inherited toString
    ])

  else
    result := inherited toString;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the scanner
function AParserNote.Scanner: AScanner;

begin
  Result := MyScanner;
end;

// AParserHint *****************************************************************
// Constructor
constructor AParserHint.inSource(const ThisScanner: AScanner;
  const fmt: string; const params: array of const);

begin
  inherited because(fmt, params);

  MyScanner := ThisScanner;
end;

// Initializer
function AParserHint.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyScanner := nil;
  end;
end;

// Construct a shallow copy of the other object
function AParserHint.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AParserHint) then
    result := false;

  if result then
  begin
    MyScanner := AParserHint(Other).Scanner;
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the item
function AParserHint.toString: AnsiString;

begin
  // Construct a message that includes source name and line number
  if MyScanner <> nil then
    result := SysUtils.format(pmsgStringRepresentation, [
      MyScanner.Source.name, MyScanner.lineNumber,
      inherited toString
    ])

  else
    result := inherited toString;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the scanner
function AParserHint.Scanner: AScanner;

begin
  Result := MyScanner;
end;

// AParserWarning **************************************************************
// Constructor
constructor AParserWarning.inSource(const ThisScanner: AScanner;
  const fmt: string; const params: array of const);

begin
  inherited because(fmt, params);

  MyScanner := ThisScanner;
end;

// Initializer
function AParserWarning.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyScanner := nil;
  end;
end;

// Construct a shallow copy of the other object
function AParserWarning.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AParserWarning) then
    result := false;

  if result then
  begin
    MyScanner := AParserWarning(Other).Scanner;
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the item
function AParserWarning.toString: AnsiString;

begin
  // Construct a message that includes source name and line number
  if MyScanner <> nil then
    result := SysUtils.format(pmsgStringRepresentation, [
      MyScanner.Source.name, MyScanner.lineNumber,
      inherited toString
    ])

  else
    result := inherited toString;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the scanner
function AParserWarning.Scanner: AScanner;

begin
  Result := MyScanner;
end;

// AParserSyntaxError **********************************************************
// Constructor
constructor AParserSyntaxError.inSource(const ThisScanner: AScanner;
  const fmt: string; const params: array of const);

begin
  inherited because(fmt, params);

  MyScanner := ThisScanner;
end;

// Initializer
function AParserSyntaxError.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyScanner := nil;
  end;
end;

// Construct a shallow copy of the other object
function AParserSyntaxError.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AParserSyntaxError) then
    result := false;

  if result then
  begin
    MyScanner := AParserSyntaxError(Other).Scanner;
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the item
function AParserSyntaxError.toString: AnsiString;

begin
  // Construct a message that includes source name and line number
  if MyScanner <> nil then
    result := SysUtils.format(pmsgStringRepresentation, [
      MyScanner.Source.name, MyScanner.lineNumber,
      inherited toString
    ])

  else
    result := inherited toString;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the scanner
function AParserSyntaxError.Scanner: AScanner;

begin
  Result := MyScanner;
end;

// AParserFatalError ***********************************************************
// Constructor
constructor AParserFatalError.inSource(const ThisScanner: AScanner;
  const fmt: string; const params: array of const);

begin
  inherited because(fmt, params);

  MyScanner := ThisScanner;
end;

// Initializer
function AParserFatalError.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyScanner := nil;
  end;
end;

// Construct a shallow copy of the other object
function AParserFatalError.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AParserFatalError) then
    result := false;

  if result then
  begin
    MyScanner := AParserFatalError(Other).Scanner;
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the item
function AParserFatalError.toString: AnsiString;

begin
  // Construct a message that includes source name and line number
  if MyScanner <> nil then
    result := SysUtils.format(pmsgStringRepresentation, [
      MyScanner.Source.name, MyScanner.lineNumber,
      inherited toString
    ])

  else
    result := inherited toString;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the scanner
function AParserFatalError.Scanner: AScanner;

begin
  Result := MyScanner;
end;
{$ENDIF}

