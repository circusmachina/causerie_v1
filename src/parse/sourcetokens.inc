{ sourcetokens.inc: Definitions and classes required to tokenize a source file
    using a parsed language.

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
// ATokenFromSource ************************************************************
{ This class represents a token that is parsed from a source stream.  Its
  methods are defined to retrieve the literal text of the token and match it
  with an opcode defined by a parsed language.  It has methods that enable a
  parser to check syntax and can also write itself to an intermediate code
  stream for further processing by an interpeter.
}
type ATokenFromSource = class(AToken)
  protected
    { Refers to the language definition used to parse the token from the
      source.
    }
    MyLanguage: AParsedLanguage;
    { Stores the literal text of the token, as parsed from the source }
    myText: string;

  public
    { Construct a new instance of @link(ATokenFromSource) that will use the
      specified language to tokenize the source.
    }
    constructor using(const ThisLanguage: AParsedLanguage); virtual;

    { Construct and return an instance of @link(ATokenFromSource) or its
      descendants.

      This method is a factory method designed to be called by a scanner on
      an instance of @link(ATextInputStream).  It reads a character from
      @code(Source) and, depending on how that character is defined within
      @code(ThisLanguage), constructs the appropriate instance of
      @link(ATokenFromSource) or its descendants which will return that token to
      the caller.
    }
    class function readUsing(const ThisLanguage: AParsedLanguage;
      const Source: ATextInputStream): AToken; virtual;

    { Initializer}
    function init: boolean; override;

    { Return the token to its source.  The token will be read again the next
      time the source is queried for a token.

      This method overrides the behavior inherited from @link(AToken).  It
      calls @link(AStream.rewindBy) on @code(Source), passing the size of its
      literal token string as the number of bytes by which the stream position
      should be rewound.

      This method is defined to provide a way for a scanner to "peek" at the
      next token in the source stream without actually having to process it.
    }
    procedure returnTo(const Source: AStream); override;

    { Determine whether or not the token is allowed by one of the specified
      rules.

      This method checks the token opcode against each rule specified in
      @code(rules).  The ID of the first rule that contains the token is
      returned.  If no rule contains the token, this method returns -1.

      This method is defined to allow a parser to determine whether a given
      token may appear at the current source location; in other words, it
      allows a parser to check syntax.
    }
    function isPartOf(const rules: array of TSortKey):TSortKey; virtual;

    { Determine whether or not the token is allowed by one of the named rules.

      This method checks the token opcode against each rule named in
      @code(rules).  The name of the first rule that contains the token opcode
      is returned.  If no rule contains the token, an empty string is returned.

      This method is defined to allow a parser to determine whether a given
      token may appear at the current source location; in other words, it
      allows a parser to check syntax.
    }
    function isPartOf(const rules: array of string): string; virtual;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(AToken.shallowCopyFrom): it calls that method, then checks to see
      whether @code(Other) is an instance of @link(ATokenFromSource).  If so, it
      copies the value of @link(ATokenFromSource.text) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method overrides the behavior inherited from @link(AToken).  It is
      written with the assumption that @code(Source) represents an instance of
      @link(ATextInputStream).  However, in the base implementation of
      @link(ATokenFromSource), it does nothing at all; it is for descendants of
      @link(ATokenFromSource) to retrieve and parse the token text.

      @return(The total number of bytes retrieved from @code(Source).  In the
      base implementation provided by @link(ATokenFromSource), this value will
      always be zero (0).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the token, suitable for output to a
      text-based device, such as a console.

      This method overrides the behavior inherited from @link(AToken).  It
      simply returns a quoted form of @code(Self.text), as obtained by a call to
      @link(Charstring.quote).  It is assumed that this method will be called by
      a parser that needs to output a warning or error message concerning a
      given token.
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the language definition used to parse the token.

      The caller may make use of this reference to directly query the language
      definition being used, but it should NOT attempt to free the reference.
    }
    function Language: AParsedLanguage;

    { Retrieve the literal text of the token, as read from the source stream. }
    function text: string; virtual;
end;

// AnIdentifierToken ***********************************************************
{ This token represents an identifier read from the source.  Identifiers have
  the potential of being reserved words or operators recognized by the
  language; otherwise, they are treated as symbols for entry into a symbol
  table.

  Identifiers are distinct from special tokens in that they generally must
  contain one or more alphanumeric characters in them, whereas a special token
  may be composed entirely of symbol characters.
}
type AnIdentifierToken = class(ATokenFromSource)
  protected
    { Refers to the symbol represented by the token }
    MySymbol: ASymbol;

  public
    { Initializer}
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(ATokenFromSource.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AnIdentifierToken).  If
      so, it copies the value of @link(AnIdentifierToken.Symbol) from
      @code(Other) to @code(Self), overwriting the values in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the token, and just the token, to the specified stream.

      This method builds on the behavior inherited from @link(ATokenFromSource);
      it calls the inherited routine and then, if applicable, writes the scope
      and index of @link(AnIdentifierToken.Symbol) to @code(Dest).

      This method is written with the assumption that @code(Dest) represents an
      intermediate code stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method reads characters from @code(Source) until one is encountered
      that is not allowed to be in an identifier.  It then checks to see
      whether the text accumulated represents a reserved word or operator
      recognized by the language; if so, it updates the value of its opcode.
      Otherwise, it is assumed that the token represents a symbol of some kind;
      either a new one that must be entered into a symbol table, or an existing
      one that has already been defined elsewhere in the source.

      This method is written with the assumption that @code(Source) represents
      an instance of @link(ATextInputStream).  To read symbolic tokens from an
      intermediate code stream, an instance of @link(ASymbolicToken) should be
      used instead.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the symbol represented by the token.

      This reference should NOT be freed by the caller.
    }
    function Symbol: ASymbol; virtual;

    { Set the symbol represented by the token.

      @return(A reference to the previous symbol represented by the token.
      This reference should NOT be freed by the caller.)
    }
    function SetSymbol(const NewSymbol: ASymbol): ASymbol; virtual;
end;

// ANumericConstantToken *******************************************************
{ This class represents a numeric constant parsed from the source.  Numeric
  constants are normally considered to be global symbols, which is why this
  class descends from @link(AnIdentifierToken).

  Note that this class does not override the behavior it inherits from
  @link(AnIdentifierToken.writeSelfTo).  The value of the token is NOT written
  to an intermediate code stream; this is done when the symbol table that
  contains the constant is written and so need not be done again.
}
type ANumericConstantToken = class(AnIdentifierToken)
  protected
    { Stores the computed value of the constant, as parsed from the source }
    myValue: Variant;

  public
    { Initializer }
    function init: boolean; override;

    { Determine whether the constant represents an integer or a floating-point
      value.

      This method simply calls @code(Variants.VarIsFloat) on @code(Self.value).
    }
    function isInteger: boolean; virtual;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(AnIdentifierToken.shallowCopyFrom): it calls that method, then
      checks to see whether @code(Other) is an instance of
      @link(ANumericConstantToken).  If so, it copies the value of
      @link(ANumericConstantToken.value) from @code(Other) to @code(Self),
      overwriting the value in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method reads characters from @code(Source) until one is encountered
      that is not allowed to be in a numeric constant.  It then checks to see
      whether the text accumulated represents a valid number; if not, it
      changes its opcode to @link(TOKCAT_ERROR).

      This method is written with the assumption that @code(Source) represents
      an instance of @link(ATextInputStream).  To read symbolic tokens from an
      intermediate code stream, an instance of @link(ASymbolicToken) should be
      used instead.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve the computed value of the numeric constant, as parsed from the
      source.

      The value returned by this routine represents the computed value of the
      token.  The literal text of the token can be retrieved by calling
      @link(ATokenFromSource.text ANumericConstantToken.text) on the token.
    }
    function value: Variant; virtual;
end;

// AStringLiteralToken *********************************************************
{ This class represents a string literal parsed from the source.  String
  literals are generally considered to be a type of global symbol, which is why
  this class descends from @link(AnIdentifierToken).

  Note that this class does not override the behavior it inherits from
  @link(AnIdentifierToken.writeSelfTo).  The value of the token is NOT written
  to an intermediate code stream; this is done when the symbol table that
  contains the constant is written and so need not be done again.
}
type AStringLiteralToken = class(AnIdentifierToken)
  public
    { Initializer }
    function init: boolean; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method does nothing.  That's right: nothing.  There are a variety of
      ways to process strings, depending on the source language (such as
      Python's strings and docstrings), and so it is left to the parser to
      determine how to handle string literals.  The token type remains valid
      and an instance of this type can be created by the parser and inserted
      into its intermediate code stream to indicate a position in the source
      where a string literal occurred.

      @return(The total number of bytes read from @code(Source).  This will
      always be zero (0) in the default implementation of
      @link(AStringLiteralToken).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;
end;

// ASpecialToken ***************************************************************
{ This class represents a special token, which is usually a delimiter or
  symbolic operator recognized by a parsed language.  Special tokens differ
  from identifier tokens in that they normally need not be comprised of
  alphanumeric characters.
}
type ASpecialToken = class(ATokenFromSource)
  public
    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method reads characters from @code(Source) until one is encountered
      that is not allowed to be part of a special token.  It then checks to see
      whether the text accumulated represents a recognized delimiter or
      operator.  If not, it sets the value of its opcode to
      @link(TOKCAT_ERROR).

      This method is written with the assumption that @code(Source) represents
      an instance of @link(ATextInputStream).  To read symbolic tokens from an
      intermediate code stream, an instance of @link(ASymbolicToken) should be
      used instead.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;
end;

// ASpaceToken *****************************************************************
{ This class represents whitespace encountered in the source.  Whitespace is
  normally discarded, but some parsers (such as for Python) may opt to
  process it instead.
}
type ASpaceToken = class(ATokenFromSource)
  protected
    { Indicates whether successive instances of a given whitespace character
      should be consolidated into one token or not
    }
    myConsolidation: boolean;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(ATokenFromSource.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(ASpaceToken).  If so, it
      copies the value of @link(ASpaceToken.consolidation) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method reads characters from @code(Source) until one is encountered
      that is not considered whitespace.

      If @link(ASpaceToken.consolidation) is @true, then this method reads
      consecutive instances of the first whitespace character encountered until
      a character is encountered that is neither whitespace nor the same as the
      first whitespace character read.  Otherwise, this method reads only a
      single whitespace character from the source.

      This method is written with the assumption that @code(Source) represents
      an instance of @link(ATextInputStream).  To read non-symbolic tokens from
      an intermediate code stream, an instance of @link(AToken) should be used
      instead.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Determine whether or not consecutive instances of a given whitespace
      character will be consolidated into one token or not.

      If this function returns @true, then occurrences of more than one
      consecutive instance of the first whitespace character encountered will
      be consolidated into a single whitespace token.  In other words, if the
      source contains four space characters in a row, as is customary with
      languages like Python, then those four characters will be consolidated
      into one token.  Otherwise, they will be read as four successive tokens.

      For instances of @link(ASpaceToken), this method will return @false by
      default.  For instances of @link(ALineEndingToken), this method will
      return @true by default.  To change this behavior, call
      @link(ASpaceToken.setConsolidation) before calling
      @link(ASpaceToken.readFrom) or @link(ALineEndingToken.readFrom).
    }
    function consolidation: boolean; virtual;

    { Set whether or not consecutive instances of a given whitespace character
      are consolidated into one token.

      If @code(flag) is @true, then occurrences of more than one consecutive
      isntance of the first whitespace character encountered will be
      consolidated into a single token; in other words, if the source contains
      four space characters in a row, as is customary with languages like
      Python, then those four space characters will be collected into one
      token.  Otherwise, they will be read as four distinct tokens.

      @return(The previous value of @link(ASpaceToken.consolidation).)
    }
    function setConsolidation(const flag: boolean): boolean; virtual;
end;

// ALineEndingTokenFromSource **************************************************
{ This class represents the end of a source line.  Certain parsers may
  process such tokens; others may discard them.
}
type ALineEndingTokenFromSource = class(ASpaceToken)
  protected
    { Stores the number of line endings encountered while tokenizing }
    myLineCount: longword;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(ASpaceToken.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of
      @link(ALineEndingTokenFromSource).  If so, it copies the value of
      @link(ALineEndingTokenFromSource.lineCount) from
      @code(Other) to  @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the token, and just the token, to the specified stream.

      This method builds on the behavior inherited from @link(ATokenFromSource).
      It calls the inherited routine, then writes the value of
      @link(ALineEndingTokenFromSource.lineCount) to the stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method reads up to two characters from the source, depending on
      the style of line ending encountered.  The method is written to handle
      Apple-style, Unix-style, and Windows-style line endings.

      This method is written with the assumption that @code(Source) represents
      an instance of @link(ATextInputStream).  To read non-symbolic tokens from
      an intermediate code stream, an instance of @link(AToken) should be used
      instead.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve the number of line endings encountered while tokenizing the
      source.

      This function is most useful when
      @code(ALineEndingTokenFromSource.consolidation) is @true but you still
      want to know how many line endings have been consolidated into one token.
    }
    function lineCount: longword; virtual;
end;

// AnErrorToken ****************************************************************
{ This class represents an erroneous or unrecognized token.  Instances of it
  are normally only constructed when @link(ATokenFromSource.readFrom) encounters
  a character in the source stream that has not been classified by the language
  definition being used to parse the source.
}
type AnErrorToken = class(ATokenFromSource)
  public
    { Initializer }
    function init: boolean; override;

    // CanStream implementation ------------------------------------------------
    { Read the token from the given source stream.

      This method reads characters from @code(Source) until a recognized
      character is encountered.

      This method is written with the assumption that @code(Source) represents
      an instance of @link(ATextInputStream).  It is probably an error to
      attempt to read a token such as this from an intermediate code stream,
      since intermediate code should contain no erroneous or unrecognized
      tokens -- but if it is necessary, an instance of @link(AToken) should be
      used instead.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ATokenFromSource ****************************************************************
// Constructor
constructor ATokenFromSource.using(const ThisLanguage: AParsedLanguage);

begin
  inherited new;

  MyLanguage := ThisLanguage;
end;

// Factory method
class function ATokenFromSource.readUsing(const ThisLanguage: AParsedLanguage;
  const Source: ATextInputStream): AToken;

var
  // Stores a character read from the source
  ch: string;

begin
  Result := nil;

  if (Source = nil) or (ThisLanguage = nil) then
    exit;

  if Source.hasEnded then
  begin
    Result := AStreamEndingToken.new;
    exit;
  end;

  // Read a character from the stream
  ch := '';
  Source.readCharacters(1, ch);

  // Return the character to the stream so it can be read by the token instance
  Source.rewindBy(System.length(ch));

  // Determine the type of character
  case ThisLanguage.categoryOf(ch) of
    CHARCAT_LETTER:
      Result := AnIdentifierToken.using(ThisLanguage);

    CHARCAT_SPECIAL:
      Result := ASpecialToken.using(ThisLanguage);

    CHARCAT_DIGIT:
      Result := ANumericConstantToken.using(ThisLanguage);

    CHARCAT_SPACE:
      Result := ASpaceToken.using(ThisLanguage);

    CHARCAT_EOL:
      Result := ALineEndingTokenFromSource.using(ThisLanguage);

  else
    Result := AnErrorToken.using(ThisLanguage);
  end;
end;

// Initializer
function ATokenFromSource.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myText := '';
  end;
end;

// Return the token to its source
procedure ATokenFromSource.returnTo(const Source: AStream);

begin
  if Source <> nil then
    Source.rewindBy(System.length(myText));
end;

// Determine whether the token is part of any of the specified rules
function ATokenFromSource.isPartOf(const rules: array of TSortKey): TSortKey;

var
  // Loop counter
  i: longword;

begin
  result := 0;

  if System.length(rules) = 0 then
    exit;

  for i := 0 to System.high(rules) do
    if MyLanguage.SyntaxRule(rules[i]).has(myOpcode) then
    begin
      result := rules[i];
      break;
    end;
end;

// Determine whether the token is part of any of the named rules
function ATokenFromSource.isPartOf(const rules: array of string): string;

var
  // Loop counter
  i: longword;

begin
  result := '';

  if System.length(rules) = 0 then
    exit;

  for i := 0 to System.high(rules) do
    if MyLanguage.SyntaxRule(rules[i]).has(myOpcode) then
    begin
      result := rules[i];
      break;
    end;
end;

// Construct a shallow copy of the other object
function ATokenFromSource.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ATokenFromSource) then
    result := false;

  if result then
  begin
    myText := ATokenFromSource(Other).text;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the given stream -- does nothing here
function ATokenFromSource.readFrom(const Source: AStream): TStreamIOSize;

begin
  // Descendant classes should override this
  result := 0;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the token
function ATokenFromSource.toString: AnsiString;

begin
  result := Charstring.quote(myText);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the language used to tokenize the source
function ATokenFromSource.Language: AParsedLanguage;

begin
  Result := MyLanguage;
end;

// Retrieve the literal text of the token
function ATokenFromSource.text: string;

begin
  result := myText;
end;

// AnIdentifierToken ***********************************************************
// Initializer
function AnIdentifierToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MySymbol := nil;
  end;
end;

// Construct a shallow copy of the other object
function AnIdentifierToken.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AnIdentifierToken) then
    result := false;

  if result then
  begin
    MySymbol := AnIdentifierToken(Other).Symbol;
  end;
end;

// Write the token and only the token to the specified stream
function AnIdentifierToken.writeSelfTo(const Dest: AStream): TStreamIOSize;

var
  // Used to store the scope and index of the symbol represented by the token
  symbolReference: TSymbolReference;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Call the inherited routine to write the opcode and anything else
  result := inherited writeSelfTo(Dest);

  // Write the scope and index
  if MySymbol <> nil then
  begin
    symbolReference := MySymbol.reference;
    result := result + Dest.write(sizeof(symbolReference), symbolReference);
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the specified stream
function AnIdentifierToken.readFrom(const Source: AStream): TStreamIOSize;

var
  // Refers to a character read from the stream
  ch: string;

begin
  result := 0;

  if (Source = nil) or (Source is ATextInputStream = false) then
    exit;

  with Source as ATextInputStream do
  begin
    ch := '';
    result := readCharacters(1, ch);

    { Gather characters from the source until we encounter one that is not
      identified as a letter or digit.
    }
    repeat
      myText := myText + ch;
      ch := '';

      // If at the end of the source, then break out of the loop
      if Source.hasEnded then
        break;

      // Read another character
      result := result + readCharacters(1, ch);
    until MyLanguage.characterIn(ch, [
      CHARCAT_LETTER, CHARCAT_WORD, CHARCAT_DIGIT
    ]) = CHARCAT_DUMMY;

    { Check to see if we've gathered a keyword or other special token }
    myOpcode := MyLanguage.opcodeFor(myText);

    { If not a special token, then we must be an identifier }
    if myOpcode = TOKCAT_DUMMY then
      myOpcode := TOKCAT_IDENTIFIER;

    { Return the most recent character to the source }
    if not Source.hasEnded then
    begin
      Source.rewindBy(System.length(ch));
      dec(result);
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the token scope and index
function AnIdentifierToken.Symbol: ASymbol;

begin
  Result := MySymbol;
end;

// Set the token scope and index
function AnIdentifierToken.SetSymbol(const NewSymbol: ASymbol): ASymbol;

begin
  Result := MySymbol;

  MySymbol := NewSymbol;
end;

// ANumericConstantToken *******************************************************
// Initializer
function ANumericConstantToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myValue := Variants.null;
  end;
end;

// Determine whether or not the token represents an integer constant
function ANumericConstantToken.isInteger: boolean;

begin
  result := (Variants.varIsNumeric(myValue)) and
     (Variants.varIsFloat(myValue) = false);
end;

// Construct a shallow copy of the other object
function ANumericConstantToken.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ANumericConstantToken) then
    result := false;

  if result then
  begin
    myValue := ANumericConstantToken(Other).value;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the stream
function ANumericConstantToken.readFrom(const Source: AStream): TStreamIOSize;

var
  // Refers to a character read from the stream
  ch: string;
  // Stores the value of the token as an integer value
  valueAsInteger: int64;
  // Stores the value of the token as a floating-point value
  valueAsFloat: double;

begin
  result := 0;

  if (Source = nil) or (Source is ATextInputStream = false) then
    exit;

  valueAsInteger := 0;
  valueAsFloat := 0;

  with Source as ATextInputStream do
  begin
    ch := '';
    result := readCharacters(1, ch);

    { Gather characters from the source until we encounter one that is not
      identified as a digit or numeric character.
    }
    repeat
      myText := myText + ch;
      ch := '';

      // If at the end of the source, then break out of the loop
      if Source.hasEnded then
        break;

      // Read another character
      result := result + readCharacters(1, ch);
    until MyLanguage.characterIn(ch, [
      CHARCAT_DIGIT, CHARCAT_NUMERIC
    ]) = CHARCAT_DUMMY;

    { Check to see if we have a valid number }
    if SysUtils.tryStrToInt64(myText, valueAsInteger) then
    begin
      myValue := valueAsInteger;
      myOpcode := TOKCAT_NUMBER;
    end

    else if SysUtils.tryStrToFloat(myText, valueAsFloat) then
    begin
      myValue := valueAsFloat;
      myOpcode := TOKCAT_NUMBER;
    end

    { We do not have a valid number }
    else
      myOpcode := TOKCAT_ERROR;

    { Return the most recent character to the source }
    if not Source.hasEnded then
    begin
      Source.rewindBy(System.length(ch));
      dec(result);
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the value of the token
function ANumericConstantToken.value: Variant;

begin
  result := myValue;
end;

// AStringLiteralToken *********************************************************
// Initializer
function AStringLiteralToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOpcode := TOKCAT_STRING;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read from the source stream
function AStringLiteralToken.readFrom(const Source: AStream): TStreamIOSize;

begin
  // Does nothing
  result := 0;
end;

// ASpecialToken ***************************************************************
// CanStream implementation ----------------------------------------------------
// Read the token from the specified stream
function ASpecialToken.readFrom(const Source: AStream): TStreamIOSize;

var
  // Refers to a character read from the stream
  ch: string;

begin
  result := 0;

  if (Source = nil) or (Source is ATextInputStream = false) then
    exit;

  with Source as ATextInputStream do
  begin
    ch := '';
    result := readCharacters(1, ch);

    { Gather characters from the source until we encounter one that is not
      identified as special.
    }
    repeat
      myText := myText + ch;
      ch := '';

      // If at the end of the source, then break out of the loop
      if Source.hasEnded then
        break;

      // Read another character
      result := result + readCharacters(1, ch);
    until MyLanguage.characterIn(ch, [CHARCAT_SPECIAL]) = CHARCAT_DUMMY;

    { Return the most recent character to the source }
    if not Source.hasEnded then
    begin
      Source.rewindBy(System.length(ch));
      dec(result);
    end;

    { Check to see if we've gathered a keyword or other special token }
    while Charstring.census(myText) > 1 do
    begin
      // Do the collected characters represent a keyword or special token?
      myOpcode := MyLanguage.opcodeFor(myText);

      // Yes, so quit
      if myOpcode <> TOKCAT_DUMMY then
        break;

      // Otherwise, strip the rightmost character and try again
      ch := Charstring.character(myText, Charstring.census(myText));
      System.setLength(myText, System.length(myText) - System.length(ch));
      // Return the rightmost character to the source so it will be read again
      Source.rewindBy(System.length(ch));
    end;

    { If not a special token, then we have an unrecognized symbol }
    if myOpcode = TOKCAT_DUMMY then
      myOpcode := TOKCAT_ERROR;
  end;
end;

// ASpaceToken *****************************************************************
// Initializer
function ASpaceToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOpcode := TOKCAT_SPACE;
    { By default, we treat each space character as its own token }
    myConsolidation := false;
  end;
end;

// Construct a shallow copy of the other object
function ASpaceToken.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ASpaceToken) then
    result := false;

  if result then
  begin
    myConsolidation := ASpaceToken(Other).consolidation;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the specified stream
function ASpaceToken.readFrom(const Source: AStream): TStreamIOSize;

var
  // Refers to a character read from the stream
  ch: string;
  // Refers to the initial character read from the stream
  initialCharacter: string;

begin
  result := 0;

  if (Source = nil) or (Source is ATextInputStream = false) then
    exit;

  with Source as ATextInputStream do
  begin
    ch := '';
    result := readCharacters(1, ch);
    initialCharacter := ch;

    { Gather characters from the source until we encounter one that is not
      identified as whitespace.
    }
    repeat
      myText := myText + ch;
      ch := '';

      // If at the end of the source, then break out of the loop
      if Source.hasEnded then
        break;

      // If we are not consolidating characters, then break out of the loop
      if myConsolidation = false then
        break;

      // Read another character
      result := result + readCharacters(1, ch);
    until (MyLanguage.characterIn(ch, [CHARCAT_SPACE]) = CHARCAT_DUMMY) or
      (ch <> initialCharacter);

    { Return the most recent character to the source }
    if (myConsolidation) and (not Source.hasEnded) then
    begin
      Source.rewindBy(System.length(ch));
      dec(result);
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve whether or not space consolidation occurs
function ASpaceToken.consolidation: boolean;

begin
  result := myConsolidation;
end;

// Set whether or not space consolidation occurs
function ASpaceToken.setConsolidation(const flag: boolean): boolean;

begin
  result := myConsolidation;
  myConsolidation := flag;
end;

// ALineEndingTokenFromSource ******************************************************
// Initializer
function ALineEndingTokenFromSource.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOpcode := TOKCAT_EOL;
    { Automatically consolidate line endings }
    myConsolidation := true;
    myLineCount := 0;
  end;
end;

// Construct a shallow copy of the other object
function ALineEndingTokenFromSource.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether we're compatible with the other object
  if not(Other is ALineEndingTokenFromSource) then
    result := false;

  if result then
  begin
    myLineCount := ALineEndingTokenFromSource(Other).lineCount;
  end;
end;

// Write the token to the given stream
function ALineEndingTokenFromSource.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  result := inherited writeSelfTo(Dest);

  result := result + Dest.write(sizeof(myLineCount), myLineCount);
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the specified stream
function ALineEndingTokenFromSource.readFrom(const Source: AStream): TStreamIOSize;

var
  // Refers to a character read from the stream
  ch: string;
  // Refers to the initial character read from the stream
  initialCharacter: string;

begin
  result := 0;

  if (Source = nil) or (Source is ATextInputStream = false) then
    exit;

  with Source as ATextInputStream do
  begin
    ch := '';
    result := readCharacters(2, ch);
    initialCharacter := ch;

    { Gather characters from the source until we encounter one that is not
      identified as a line ending.
    }
    repeat
      myText := myText + ch;
      ch := '';

      // Add to the number of line endings encountered
      inc(myLineCount);

      // If at the end of the source, then break out of the loop
      if Source.hasEnded then
        break;

      // Check for Windows-style line endings
      if ch <> LINE_ENDING_WINDOWS then
      begin
        // Reset the initial character, if needed
        if initialCharacter = ch then
          initialCharacter := Charstring.character(ch, 1);

        // Put back the second character
        ch := Charstring.character(ch, 2);
        System.setLength(myText, System.length(myText) - System.length(ch));
        Source.rewindBy(System.length(ch));
      end;

      // If we are not consolidating characters, then break out of the loop
      if myConsolidation = false then
        break;

      // Read another character
      result := result + readCharacters(2, ch);
    until (MyLanguage.characterIn(ch, [CHARCAT_EOL]) = CHARCAT_DUMMY) or
      (ch <> initialCharacter);

    { Return the most recent character to the source }
    if (myConsolidation) and (not Source.hasEnded) then
    begin
      Source.rewindBy(System.length(ch));
      dec(result);
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the number of line endings encountered
function ALineEndingTokenFromSource.lineCount: longword;

begin
  result := myLineCount;
end;

// AnErrorToken ****************************************************************
// Initializer
function AnErrorToken.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOpcode := TOKCAT_ERROR;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the token from the specified stream
function AnErrorToken.readFrom(const Source: AStream): TStreamIOSize;

var
  // Refers to a character read from the stream
  ch: string;

begin
  result := 0;

  if (Source = nil) or (Source is ATextInputStream = false) then
    exit;

  with Source as ATextInputStream do
  begin
    ch := '';
    result := readCharacters(1, ch);

    { Gather characters from the source until we encounter one that is not
      identified as erroneous.
    }
    repeat
      myText := myText + ch;
      ch := '';

      // If at the end of the source, then break out of the loop
      if Source.hasEnded then
        break;

      // Read another character
      result := result + readCharacters(1, ch);
    until MyLanguage.characterIn(ch, [CHARCAT_ERROR]) = CHARCAT_DUMMY;

    { Return the most recent character to the source }
    if not Source.hasEnded then
    begin
      Source.rewindBy(System.length(ch));
      dec(result);
    end;
  end;
end;
{$ENDIF}
