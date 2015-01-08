{ tokens.inc: Definitions and classes required to tokenize an intermediate
    code stream using a parsed language.

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
// AToken **********************************************************************
{ This class represents a basic token which is parsed from a source stream or
  retrieved from an intermediate code stream.  A token is used to match text
  from the source to an internal representation (@link(TOpcode)) so that it
  can be processed and acted upon by a parser.

  Tokens support streaming to and from arbitrary streams and can be chained
  together in lists -- this, in fact, is how the default implementations of
  @link(AParser) and its descendants manage tokens.
}
type AToken = class(ANode)
  protected
    { Stores the internal representation of the token -- its opcode }
    myOpcode: TOpcode;
    { Indicates whether or not the token should be appended to parser output }
    mySilence: boolean;

  public
    { Construct and return the appropriate instance of @link(AToken) or
      @link(ASymbolicToken).

      This method is designed to be used on streams that contain intermediate
      code.  It is a factory method that will read an opcode from @code(Source)
      and construct the appropriate token instance depending on that opcode.
      If the opcode is @link(TOKCAT_IDENTIFIER), @link(TOKCAT_NUMBER), or
      @link(TOKCAT_STRING), an instance of @link(ASymbolicToken) is read and
      returned; otherwise an instance of @link(AToken) is returned.
    }
    class function from(const Source: AStream): AToken; virtual;

    { Initializer }
    function init: boolean; override;

    { Retrieve the category to which the token belongs.

      Token category constants are defined such that the category can be
      specified in the high word of a @link(TOpcode) while a unique code for the
      token is specified in the low word.  This is particularly useful for
      keywords, operators, and special characters that are recognized by a
      language.

      This method simply masks out all but the high word and returns that
      value, which should be one of the @code(TOKCAT_*) constants, such as
      @link(TOKCAT_KEYWORD).  For tokens that do not represent a keyword,
      operator, or special character, this value will likely be the same as
      that returned by @link(AToken.opcode).
    }
    function category: TOpcode;

    { Return the token to its source.  The token will be read again the next
      time the source is queried for a token.

      As defined in the base implementation of @link(AToken), this method
      merely calls @link(AStream.rewindBy) on @code(Source), passing the
      size of its opcode as the number of bytes by which the stream position
      should be rewound.

      This method is defined to provide a way for a scanner to "peek" at the
      next token in the source stream without actually having to process it.
    }
    procedure returnTo(const Source: AStream); virtual;

    { Determine whether or not the token is specified in the given rule.

      This method checks the token opcode against those specified by
      @code(Rule).  If the token appears anywhere at all in the rule, this
      method returns @true; otherwise it returns @false.

      This method is defined to allow a parser to determine whether a given
      token may appear at the current source location; in other words, it
      allows a parser to check syntax.
    }
    function isPartOf(const Rule: ASyntaxRule): boolean; overload; virtual;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to see
      whether @code(Other) is an instance of @link(AToken).  If so, it copies
      the values of
      @unorderedList(
        @item(@link(AToken.opcode) and)
        @item(@link(AToken.silenced))
      )
      from @code(Other) to @code(Self), overwriting the values in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Calculate the number of bytes required to stream the token.

      This method overrides the behavior inherited from
      @link(ANode.selfStreamingLength).  It returns the number of bytes required
      to stream the value of @link(AToken.opcode).
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the token, and just the token, to the specified intermediate code
      stream.

      This method overrides the behavior inherited from @link(ANode).  In the
      base implementation provided in @link(AToken), it simply writes the value
      of @link(AToken.opcode) to @code(Dest).  Descendants of @link(AToken) may
      override this method to write additional data to the stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Write the token to the specified intermediate code stream.

      This method overrides the behavior inherited from @link(ANode).  It does
      not seek to preserve the links between tokens even if they are part of
      @link(ATokenList); this is because tokens are parsed serially and do not
      need to be loaded all at once into memory.  The method as defined here
      simply calls @link(AToken.writeSelfTo) and returns.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeTo(const Dest: AStream): TStreamIOSize; override;

    { Read the token from the specified intermediate code stream.

      This method overrides the behavior inherited from @link(ANode).  In the
      base implementation provided in @link(AToken), it simply reads the value
      of @link(AToken.opcode) from @code(Source).  Descendants of @link(AToken)
      may override this method to read additional data from the stream.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Produce a string representation of the token, suitable for output to a
      text-based device such as a console.

      This method overrides the behavior inherited from @link(ANode).  It
      returns the display name of the token and a hex-formatted representation
      of @link(AToken.opcode).  The format of the returned string is governed
      by @link(toknStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the opcode associated with the token.

      The value returned by this function will vary depending on the type of
      token parsed.  At the very least, it will contain one of the
      @code(TOKCAT_*) constants, such as @link(TOKCAT_IDENTIFIER).  This value
      is set by the token when it is parsed from the source and cannot be
      changed.
    }
    function opcode: TOpcode;

    { Determine whether or not the token is silenced.

      A token that is silenced should not be output by a parser to its
      intermediate code stream.  This flag will most often be used to mark
      tokens that represent whitespace, as most parsers choose to ignore
      whitespace.
    }
    function silenced: boolean; virtual;

    { Set whether or not the token is silenced.

      A token that is silenced should not be output by a parser to its
      intermediate code stream.  This flag will most often be used to mark
      tokens that represent whitespace, as most parsers are written to ignore
      whitespace.

      @return(Whether or not the token was previously silenced.)
    }
    function setSilenced(const flag: boolean): boolean; virtual;
end;

// ASymbolicToken **************************************************************
{ This class represents a symbol that is entered into a symbol table.  These
  will usually be identifiers, numeric constants, or string literals.  The
  actual value of the symbol is stored in a separate symbol table, to which
  the token makes reference by means of @link(ASymbolicToken.symbol).
}
type ASymbolicToken = class(AToken)
  protected
    { Stores the scope and index of the symbol represented by the token. }
    mySymbol: TSymbolReference;

  public
    { Initializer}
    function init: boolean; override;

    { Return the token to its source.  The token will be read again the next
      time the source is queried for a token.

      This method overrides the behavior inherited from @link(AToken).  It
      rewinds @code(Source) by the size of its index and opcode, then calls
      the inherited routine.

      This method is defined to provide a way for a scanner to "peek" at the
      next token in the source stream without actually having to process it.
    }
    procedure returnTo(const Source: AStream); override;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(AToken.shallowCopyFrom): it calls that method, then checks to see
      whether @code(Other) is an instance of @link(ASymbolicToken).  If so, it
      copies the value of @link(ASymbolicToken.symbol) from @code(Other) to
      @code(Self), overwriting the values in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Calculate the number of bytes required to stream the token.

      This method builds upon the behavior inherited from
      @link(AToken.selfStreamingLength): it calls that routine, then adds the
      number of bytes required to stream the value of
      @link(ASymbolicToken.symbol).
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the token, and just the token, to the specified stream.

      This method builds upon the behavior inherited from @link(AToken).  It
      calls the inherited routine, then writes the value of
      @link(ASymbolicToken.symbol) to @code(Dest).

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the specified intermediate code stream.

      This method builds upon the behavior inherited from @link(AToken).  It
      calls the inherited routine, then reads the value of
      @link(ASymbolicToken.symbol).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the token, suitable for printing to
      a text-based device, such as a console.

      This method overrides the behavior inherited from @link(AToken).  It
      constructs a string that contains the display name of the class and the
      value of @link(ASymbolicToken.symbol).  The format of this string is
      controlled by the value of @link(smtkStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the token scope, which refers to the symbol list to which the
      symbol belongs.  A given source program may have one or more symbol
      lists, which are indexed within an instance of
      @link(ASymbolVectorVector).

      General practice is to define a few global symbol lists: one for all
      string literals, one for all numeric constants, and one for all types; as
      well as one or more local scopes for each routine within the source
      program.  The value returned by this method is the index of the
      appropriate instance of @link(ASymbolVector) within that list.
    }
    function symbol: TSymbolReference;
end;

// ALineEndingToken ************************************************************
{ This class represents the end of a source line.  Some parsers may process
  these tokens out of the source and place them in an intermediate code stream
  as a way to mark the end of a statement.
}
type ALineEndingToken = class(AToken)
  protected
    { Stores the number of line endings represented by the token }
    myLineCount: longword;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to see
      whether @code(Other) is an instance of @link(AToken).  If so, it copies
      the value of @link(ALineEndingToken.lineCount) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Calculate the number of bytes required to stream the token.

      This method builds upon the behavior inherited from
      @link(AToken.selfStreamingLength): it calls that routine, then adds
      the number of bytes required to stream the value of
      @link(ALineEndingToken.lineCount).
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the token, and just the token, to the given stream.
      This method builds upon the behavior inherited from @link(AToken).  It
      calls the inherited routine, then writes the number of line endings that
      are represented by the token.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given stream.

      This method builds upon the behavior inherited from @link(AToken).  It
      calls the inherited routine, then reads the number of line endings that
      are represented by the token.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the token, suitable for
      output to a text-based device, such as a console.

      This method overrides the behavior inherited from @link(AToken); it
      simply returns the value defined by @link(letkStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the number of line endings represented by the token.

      This value is usually set when an instance of
      @link(ALineEndingTokenFromSource) has written itself to an intermediate
      code stream.  The value will be at least one (1), but may be more
      depending upon whether or not multiple line endings in the original source
      were consolidated into one token.
    }
    function lineCount: longword; virtual;
end;

// AStreamEndingToken **********************************************************
{ This class represents the end of a source stream.  While it is possible that
  some language definitions may specify one or more characters to mark the end
  of a source stream, it is more likely that this token will be returned when
  the source stream indicates that its end has been reached.
}
type AStreamEndingToken = class(AToken)
  public
    { Initializer }
    function init: boolean; override;

    { Return the token to its source.

      This method overrides the behavior inherited from @link(AToken).  It
      does nothing.
    }
    procedure returnTo(const Source: AStream); override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      To avoid attempts to read beyond the end of the stream, this method does
      nothing when called.

      @return(The total number of bytes read from @code(Source), which will
      always be zero (0).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the token, suitable for
      output to a text-based device, such as a console.

      This method overrides the behavior inherited from @link(AToken); it
      simply returns the value defined by @link(setkStringRepresentation).
    }
    function toString: AnsiString; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This constant defines how a string representation of @link(AToken) is
    constructed when @link(AToken.toString) is called.

    The string placeholder will be filled with the display name of the class,
    as returned by a call to @link(APrintingObject.displayName).  The
    integer placeholder will be filled with the value of the token opcode,
    as returned by a call to @link(AToken.opcode).
  }
  toknStringRepresentation = '%s: %X';

  { This constant defines how a string representation of @link(ASymbolicToken)
    is constructed when @link(ASymbolicToken.toString) is called.

    The string placeholder will be filled with the display name of the class,
    as returned by a call to @link(APrintingObject.displayName).  The first
    and second integer placeholders are filled with the scope and index of the
    symbol represented by the token, as returned by a call to
    @link(ASymbolicToken.symbol).
  }
  smtkStringRepresentation = '%s in scope %X: %d';

  { This constant defines how a string representation of @link(ALineEndingToken)
    is constructed when @link(ALineEndingToken.toString) is called.

    The integer placeholder is filled with the number of line endings
    represented by the token, as retrieved by a call to
    @link(ALineEndingToken.lineCount).
  }
  letkStringRepresentation = 'end of line (%d)';

  { This constant defines how a string representation of
    @link(AStreamEndingToken) is constructed when
    @link(AStreamEndingToken.toString) is called.

    As defined here, the format requires no additional parameters.
  }
  setkStringRepresentation = 'end of stream';
{$ENDIF}
{$IFDEF ReadImplementation}
// AToken **********************************************************************
// Factory method
class function AToken.from(const Source: AStream): AToken;

var
  // Stores an opcode read from the source stream
  tokenOpcode: TOpcode;

begin
  Result := nil;

  if Source = nil then
    exit;

  if Source.hasEnded then
  begin
    Result := AStreamEndingToken.new;
    exit;
  end;

  tokenOpcode := TOKCAT_DUMMY;

  // Read an opcode
  Source.read(sizeof(tokenOpcode), tokenOpcode);

  // Construct the appropriate token
  case tokenOpcode of
    TOKCAT_IDENTIFIER, TOKCAT_NUMBER, TOKCAT_STRING:
      Result := ASymbolicToken.new;

    TOKCAT_EOL:
      Result := ALineEndingToken.new;

  else
    Result := Self.new;
  end;

  // Return the opcode so it can be read by the token instance
  Source.rewindBy(sizeof(tokenOpcode));
end;

// Initializer
function AToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOpcode := TOKCAT_DUMMY;
    mySilence := false;
  end;
end;

// Retrieve the token category
function AToken.category: TOpcode;

begin
  result := myOpcode and TOKCAT_CATMASK;
end;

// Return the token to the source
procedure AToken.returnTo(const Source: AStream);

begin
  if Source <> nil then
    Source.rewindBy(sizeof(myOpcode));
end;

// Determine whether the token is part of the specified rule
function AToken.isPartOf(const Rule: ASyntaxRule): boolean;

begin
  result := false;

  if Rule <> nil then
    result := Rule.has(myOpcode);
end;

// Construct a shallow copy of the other object
function AToken.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AToken) then
    result := false;

  if result then
  begin
    myOpcode := AToken(Other).opcode;
    mySilence := AToken(Other).silenced;
  end;
end;

// Calculate the number of bytes required to stream the token
function AToken.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine
  result := inherited selfStreamingLength;

  // Add ourselves
  inc(result, sizeof(myOpcode));
end;

// Write the token to the given intermediate code stream
function AToken.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
    result := Dest.write(sizeof(myOpcode), myOpcode);
end;

// CanStream implementation ----------------------------------------------------
// Write the token to the given intermediate code stream
function AToken.writeTo(const Dest: AStream): TStreamIOSize;

begin
  result := Self.writeSelfTo(Dest);
end;

// Read the token from the given intermediate code stream
function AToken.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
    result := Source.read(sizeof(myOpcode), myOpcode);
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the token
function AToken.toString: AnsiString;

begin
  result := SysUtils.format(toknStringRepresentation, [
    Self.displayName, myOpcode
  ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve the opcode associated with the token
function AToken.opcode: TOpcode;

begin
  result := myOpcode;
end;

// Determine whether or not the token is silenced
function AToken.silenced: boolean;

begin
  result := mySilence;
end;

// Set whether or not the token is silenced
function AToken.setSilenced(const flag: boolean): boolean;

begin
  result := mySilence;
  mySilence := flag;
end;

// ASymbolicToken **************************************************************
// Initializer
function ASymbolicToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    mySymbol.scope := 0;
    mySymbol.index := 0;
  end;
end;

// Return the token to the given source
procedure ASymbolicToken.returnTo(const Source: AStream);

begin
  if Source <> nil then
  begin
    Source.rewindBy(sizeof(mySymbol));
    // Rewind past the opcode
    inherited returnTo(Source);
  end;
end;

// Construct a shallow copy of the other object
function ASymbolicToken.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ASymbolicToken) then
    result := false;

  if result then
  begin
    mySymbol := ASymbolicToken(Other).symbol;
  end;
end;

// Calculate the number of bytes required to stream the token
function ASymbolicToken.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine
  result := inherited selfStreamingLength;

  // Add ourselves
  inc(result, sizeof(mySymbol));
end;

// Write the token to the given stream
function ASymbolicToken.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    // Write the opcode
    result := inherited writeSelfTo(Dest);

    result := result + Dest.write(sizeof(mySymbol), mySymbol);
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the given stream
function ASymbolicToken.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
  begin
    // Read the opcode
    result := inherited readFrom(Source);

    // Read the scope and index
    result := result + Source.read(sizeof(mySymbol), mySymbol);
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the token
function ASymbolicToken.toString: AnsiString;

begin
  result := SysUtils.format(smtkStringRepresentation, [
    Self.displayName, mySymbol.scope, mySymbol.index
  ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve the token scope
function ASymbolicToken.symbol: TSymbolReference;

begin
  result := mySymbol;
end;

// ALineEndingToken ************************************************************
// Initializer
function ALineEndingToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOpcode := TOKCAT_EOL;
    myLineCount := 0;
  end;
end;

// Construct a shallow copy of the other object
function ALineEndingToken.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ALineEndingToken) then
    result := false;

  if result then
  begin
    myLineCount := ALineEndingToken(Other).lineCount;
  end;
end;

// Calculate the number of bytes required to stream the token
function ALineEndingToken.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine
  result := inherited selfStreamingLength;

  // Add ourselves
  inc(result, sizeof(myLineCount));
end;

// Write just the token to a given stream
function ALineEndingToken.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  result := inherited writeSelfTo(Dest);

  result := result + Dest.write(sizeof(myLineCount), myLineCount);
end;

// CanStream implementation ----------------------------------------------------
// Read the token from a stream
function ALineEndingToken.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source = nil then
    exit;

  result := inherited readFrom(Source);

  result := result + Source.read(sizeof(myLineCount), myLineCount);
end;

// CanPrint implementation -----------------------------------------------------
// Return a string representation of the token
function ALineEndingToken.toString: AnsiString;

begin
  result := SysUtils.format(letkStringRepresentation, [myLineCount]);
end;

// Properties ------------------------------------------------------------------
// Retrieve the number of line endings represented by the token
function ALineEndingToken.lineCount: longword;

begin
  result := myLineCount;
end;

// AStreamEndingToken **********************************************************
// Initializer
function AStreamEndingToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOpcode := TOKCAT_EOS;
  end;
end;

// Return the token to the stream -- does nothing
procedure AStreamEndingToken.returnTo(const Source: AStream);

begin
  exit;
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the stream -- does nothing
function AStreamEndingToken.readFrom(const Source: AStream): TStreamIOSize;

begin
  // Do nothing
  result := 0;
end;

// CanPrint implementation -----------------------------------------------------
// Return a string representation of the token
function AStreamEndingToken.toString: string;

begin
  result := setkStringRepresentation;
end;

{$ENDIF}
