{ parsers.inc: The base class for all parsers.

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
// AParser *********************************************************************
{ This class represents a generic parser.  You will likely not construct an
  instance of this class directly since it only provides the support mechanisms
  needed for a parser and lacks the code to do the actual parsing.

  When constructing a parser, you are expected to provide:
  @unorderedList(
    @item(a log for message output)
    @item(an input stream, unless the parser creates its own)
  )

  The parser will create and dispose of its own scanner while parsing.
}
type AParser = class(AnObject)
  protected
    { Refers to the scanner used to tokenize the source }
    MyScanner: AScanner;
    { Refers to the log used for parser output }
    MyLog: ALog;
    { Refers to the source stream being parsed }
    MySource: AStream;
    { Refers to the list of tokens parsed from the source }
    MyTokens: ATokenList;

  public
    { Construct a new parser that will parse the specified source and output
      status and error messages to the specified log.

      Both @code(ThisSource) and @code(ThisLog) must be valid instances of their
      respective classes in order for @link(AParser.parse) to succeed.
    }
    constructor forSource(const ThisSource: AStream;
      const ThisLog: ALog); virtual;

    { Initializer }
    function init: boolean; override;

    { Destroy the parser instance.

      This method frees the token list and the scanner associated with the
      parser, if any, then calls the inherited routine.
    }
    destructor destroy; override;

    { Parse the source.

      The base definition of this method does nothing more than check to
      ensure that @link(AParser.Source) and @link(AParser.Log) are both
      valid pointers.  Descendant classes may call this routine before
      performing their own checks and parsing the specified source.

      @return(A negative value if an error has occurred before any parsing
      was done.  This will most likely occur if either @link(AParser.Source) or
      @link(AParser.Log) are invalid (@nil) pointers.  A positive value
      indicates the number of non-fatal errors that occurred while parsing.  A
      value of zero (0) indicates all is well.)
    }
    function parse: int64; virtual;

    { Retrieve a reference to the most recent token parsed from the source.

      This method returns a reference to the last token in the token list
      maintained by the parser.  This reference should NOT be freed by the
      caller; that will be done by the token list to which the token belongs.

      If the caller wishes to pop a token from the token list, it should do so
      by calling @link(ATokenList.Pop) on @link(AParser.TokenList).  It is then
      responsible for freeing the token itself.
    }
    function CurrentToken: AToken; virtual;

    { Retrieve a reference to the second-most-recent token parsed from the
      source.

      This method returns a reference to the second-to-last token in the token
      list maintained by the parser.  This reference should NOT be freed by
      the caller; that will be done by the token list to which the token
      belongs.
    }
    function PreviousToken: AToken; virtual;

    { Peek at the next token without advancing the current position in the
      source.

      This method calls @link(AScanner.Peek) on the instance of @link(AScanner)
      maintained by the parser and returns the token reference to the caller.
      The caller is responsible for freeing that reference when it is no longer
      required.
    }
    function PeekToken: AToken; virtual;

    { Retrieve the next token from the source.

      This method calls @link(AScanner.next) on the instance of @link(AScanner)
      maintained by the paser and pushes the resulting token reference onto its
      list of tokens.  That reference is then returned to the caller.  The
      caller should NOT free it directly.  Instead, they should pop the token
      from @link(AParser.Tokens) and then free the reference.

      If @code(silenceCurrentToken) is @true, then this method calls
      @link(AToken.setSilenced) on the current token before retrieving the
      next token from the source.  Although @link(AParser) does not make use of
      this behavior, descendant classes may prevent silenced tokens from being
      output to their intermediate code streams.
    }
    function NextToken(const silenceCurrentToken: boolean = false): AToken;
      virtual;

    { Retrieve the next token from the source, but only if the specified
      @code(condition) is met.

      This method is defined for convenience; it allows the programmer to
      specify a boolean expression, the outcome of which will determine whether
      another token is read from the source or not.  If @code(condition)
      evaluates to @true, then this method calls @link(AParser.NextToken) on
      @code(Self); otherwise it does nothing and returns @nil.

      If @code(silenceCurrentToken) is @true, then this method calls
      @link(AToken.setSilenced) on the current token before retrieving the
      next token from the source.  Although @link(AParser) does not make use of
      this behavior, descendant classes may prevent silenced tokens from being
      output to their intermediate code streams.
    }
    function NextTokenIf(const condition: boolean;
      const silenceCurrentToken: boolean = false): AToken; virtual;

    { Return the current token to the source.

      This method pops the last token from its token list and returns it to the
      source stream, so that it will be read again.  If
      @link(AParser.CurrentToken) is @nil, then this routine does nothing.
    }
    procedure rewind; virtual;

    { Resynchronize the parser to the specified @code(opcode).

      This method is designed to allow the parser to recover after a syntax
      error.  If @code(CurrentToken.opcode) does not match @code(opcode), it
      logs a syntax error to indicate that the current token is not
      expected, then calls @link(AParser.skipToToken) to skip tokens in the
      source.
    }
    procedure resyncToToken(const opcode: TOpcode); virtual;

    { Resynchronize the parser to one of the tokens from the specified
      @code(Rule).

      This method is designed to allow the parser to recover after a syntax
      error.  If @code(CurrentToken.opcode) is not found in @code(ThisRule),
      it logs a syntax error to indicate that the current token is not expected,
      then calls @link(AParser.skipTo) to skip tokens in the source.

      If @code(ThisRule) is @nil, then this routine does nothing.
    }
    procedure resyncTo(const ThisRule: ASyntaxRule); virtual;

    { Skip tokens in the source until one with the specified @code(opcode) is
      encountered.

      If @code(silenceInterveningTokens) is @true, then all tokens encountered
      before @code(opcode) will have their @code(silenced) property set by a
      call to @link(AToken.setSilenced).  Although @link(AParser) does not make
      use of this behavior, descendant classes may prevent silenced tokens from
      being written to their intermediate code streams.

      @raises(AParserFatalError if @code(opcode) is not @link(TOKCAT_EOS) and
        the end of the source was reached before @code(opcode) was found.)
    }
    procedure skipToToken(const opcode: TOpcode;
      const silenceInterveningTokens: boolean = true); virtual;

    { Skip tokens until one of the tokens in @code(ThisRule) is encountered.

      If @code(ThisRule) is @nil, then this method does nothing.

      If @code(silenceInterveningTokens) is @true, then all tokens encountered
      before @code(opcode) will have their @code(silenced) property set by a
      call to @link(AToken.setSilenced).  Although @link(AParser) does not make
      use of this behavior, descendant classes may prevent silenced tokens from
      being written to their intermediate code streams.

      @raises(AParserFatalError if @code(ThisRule) does not contain
      @link(TOKCAT_EOS) and the end of the source is encountered before an
      opcode that is part of @code(ThisRule).)
    }
    procedure skipTo(const ThisRule: ASyntaxRule); virtual;

    { Output a note to the log.

      This method is defined for convenience; it automates the construction of
      an instance of @link(AParserNote), then writes that instance to
      @link(AParser.Log) before freeing it.

      @code(fmt) specifies the format of the message that will be written to
      the log.  @code(params) specifies any values that should be incorporated
      into that message when it is logged.
    }
    procedure note(const fmt: string; const params: array of const); virtual;

    { Output a hint to the log.

      This method is defined for convenience; it automates the construction of
      an instance of @link(AParserHint), then writes that instance to
      @link(AParser.Log) before freeing it.

      @code(fmt) specifies the format of the message that will be written to
      the log.  @code(params) specifies any values that should be incorporated
      into that message when it is logged.
    }
    procedure hint(const fmt: string; const params: array of const); virtual;

    { Output a warning to the log.

      This method is defined for convenience; it automates the construction of
      an instance of @link(AParserWarning), then writes that instance to
      @link(AParser.Log) before freeing it.

      @code(fmt) specifies the format of the message that will be written to
      the log.  @code(params) specifies any values that should be incorporated
      into that message when it is logged.
    }
    procedure warn(const fmt: string; const params: array of const); virtual;

    { Output a syntax error to the log.

      This method is defined for convenience; it automates the construction of
      an instance of @link(AParserSyntaxError), then writes that instance to
      @link(AParser.Log) before freeing it.

      @code(fmt) specifies the format of the message that will be written to
      the log.  @code(params) specifies any values that should be incorporated
      into that message when it is logged.
    }
    procedure syntaxError(const fmt: string; const params: array of const);
      virtual;

    { Output a fatal error to the log.

      This method is defined for convenience; it automates the construction of
      an instance of @link(AParserFatalError), then writes that instance to
      @link(AParser.Log).  The instance is returned to the caller, which allows
      the caller to both raise the fatal error and log it in one statement:
      @longcode(#
        // We should not be at the end of the source here
        if MySource.hasEnded then
          // Log and raise an exception that will halt parsing
          raise Self.fatalError('unexpected end of source', []);
      #)

      If the caller does not intend to raise an exception with the returned
      instance, then it is responsible for freeing the instance when it is no
      longer required.

      @code(fmt) specifies the format of the message that will be written to
      the log.  @code(params) specifies any values that should be incorporated
      into that message when it is logged.
    }
    function fatalError(const fmt: string;
      const params: array of const): AParserFatalError; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the scanner being used by the parser.

      This value may be @nil until @link(AParser.parse) is called on the
      parser.  In any event, the reference is managed by the parser and should
      NOT be freed by the caller.
    }
    function Scanner: AScanner;

    { Retrieve a reference to the log used by the parser for status and error
      output.

      This reference should NOT be freed by the caller.
    }
    function Log: ALog;

    { Retrieve a reference to the source stream being parsed by the parser.

      This reference should NOT be freed by the caller.
    }
    function Source: AStream;

    { Retrieve a reference to the list of tokens being managed by the parser.

      This reference should NOT be freed by the caller; that will be done by
      the parser when it is, itself, freed.
    }
    function Tokens: ATokenList;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string defines the error message and format used when the end of a
    stream is encountered unexpectedly.

    As defined here, the error message requires no additional parameters.
  }
  parsFatalUnexpectedEOS = 'unexpected end of source';

  { This constant defines the format of the error message logged by instances of
    @link(ASourceParser) when @link(ASourceParser.resyncToToken) and
    @link(ASourceParser.resyncTo) encounter an unexpected token in the source.

    The string placeholder is filled with a string representation of the
    unexpected token, as returned by a call to @link(AToken.toString).
  }
  parsSyntaxUnexpectedToken = 'unexpected %s in source';
{$ENDIF}
{$IFDEF ReadImplementation}
// AParser *********************************************************************
// Constructor
constructor AParser.forSource(const ThisSource: AStream;
  const ThisLog: ALog);

begin
  inherited new;

  MySource := ThisSource;
  MyLog := ThisLog;
end;

// Initializer
function AParser.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyScanner := nil;
    MySource := nil;
    MyLog := nil;

    // Construct a token list to manage tokens parsed from the source
    MyTokens := ATokenList.new;

    result := MyTokens <> nil;
  end;
end;

// Destructor
destructor AParser.destroy;

begin
  // Free the scanner, if any
  if MyScanner <> nil then
    MyScanner.free;

  // Free the list of tokens
  if MyTokens <> nil then
    MyTokens.free;

  inherited destroy;
end;

// Parse the source - does nothing much here
function AParser.parse: int64;

begin
  result := -1;

  if (MySource <> nil) and (MyLog <> nil) then
    result := 0;
end;

// Retrieve a reference to the most recent token parsed from the source
function AParser.CurrentToken: AToken;

begin
  Result := MyTokens.LastToken;
end;

// Retrieve a reference to the second-most-recent token parsed from the source
function AParser.PreviousToken: AToken;

begin
  Result := MyTokens.LastToken;

  if Result <> nil then
    Result := AToken(MyTokens.LastToken.Previous);
end;

// Peek at the next token from the source
function AParser.PeekToken: AToken;

begin
  Result := nil;

  if MyScanner <> nil then
    Result := MyScanner.Peek;
end;

// Retrieve the next token from the source
function AParser.NextToken(const silenceCurrentToken: boolean = false): AToken;

begin
  Result := nil;

  if MyScanner <> nil then
  begin
    if MyScanner.CurrentToken <> nil then
      MyScanner.CurrentToken.setSilenced(silenceCurrentToken);

    MyScanner.Next;
    MyTokens.push(MyScanner.CurrentToken);
    Result := MyTokens.LastToken;
  end;
end;

// Retrieve the next token from the source, but only if the condition is met
function AParser.NextTokenIf(const condition: boolean;
  const silenceCurrentToken: boolean = false): AToken;

begin
  Result := nil;

  if (MyScanner <> nil) and (condition) then
    Result := Self.NextToken(silenceCurrentToken);
end;

// Return the current token to the source
procedure AParser.rewind;

var
  // Refers to the token being returned
  TokenReturned: AToken;

begin
  // Pop the most recent token from the list
  TokenReturned := MyTokens.Pop;

  if (MyScanner <> nil) and (TokenReturned <> nil) then
    // TokenReturned is freed by the scanner's 'returnToken' method
    MyScanner.returnToken(TokenReturned);
end;

// Resynchronize the parser to the specified opcode
procedure AParser.resyncToToken(const opcode: TOpcode);

begin
  // Ensure we have a token
  if CurrentToken = nil then
    exit;

  // Check to see whether the current token matches the expected token
  if CurrentToken.opcode = opcode then
    exit;

  // Syntax error -- this token was not expected here
  Self.syntaxError(parsSyntaxUnexpectedToken, [CurrentToken.toString]);

  // Skip tokens until we find the desired token
  Self.skipToToken(opcode);
end;

// Resynchronize the parser to one of the opcodes in the specified rule
procedure AParser.resyncTo(const ThisRule: ASyntaxRule);

begin
  // Ensure we have a token and a rule
  if (CurrentToken = nil) or (ThisRule = nil) then
    exit;

  // Check to see whether the current token is in the specified rule
  if CurrentToken.isPartOf(ThisRule) then
    exit;

  // Syntax error -- this token was not expected here
  Self.syntaxError(parsSyntaxUnexpectedToken, [CurrentToken.toString]);

  // Skip tokens until we find one that is expected
  Self.skipTo(ThisRule);
end;

// Skip tokens in the source until the one with the specified opcode is found
procedure AParser.skipToToken(const opcode: TOpcode;
  const silenceInterveningTokens: boolean = true);

begin
  // Ensure we have a token
  if CurrentToken = nil then
    exit;

  while CurrentToken.opcode <> opcode do
  begin
    // Check for an unexpected end to the source
    if (CurrentToken.opcode = TOKCAT_EOS) and (opcode <> TOKCAT_EOS) then
      raise fatalError(parsFatalUnexpectedEOS, []);

    Self.NextToken(silenceInterveningTokens);
  end;
end;

// Skip tokens until one of the opcodes in the specified rule is encountered
procedure AParser.skipTo(const ThisRule: ASyntaxRule;
  const silenceInterveningTokens: boolean = true);

begin
  // Ensure we have a token and a rule
  if (CurrentToken = nil) or (ThisRule = nil) then
    exit;

  while not CurrentToken.isPartOf(ThisRule) do
  begin
    // Check for an unexpected end to the source
    if (CurrentToken.opcode = TOKCAT_EOS) and
      (not ThisRule.has(TOKCAT_EOS)) then
      raise fatalError(parsFatalUnexpectedEOS, []);

    Self.NextToken(silenceInterveningTokens);
  end;
end;

// Output a note to the log
procedure AParser.note(const fmt: string; const params: array of const);

var
  // Refers to the note in question
  ThisNote: AParserNote;

begin
  if (MyScanner = nil) or (MyLog = nil) then
    exit;

  ThisNote := AParserNote.inSource(MyScanner, fmt, params);
  MyLog.write([ThisNote]);
  ThisNote.free;
end;

// Output a hint to the log
procedure AParser.hint(const fmt: string; const params: array of const);

var
  // Refers to the hint in question
  ThisHint: AParserHint;

begin
  if (MyScanner = nil) or (MyLog = nil) then
    exit;

  ThisHint := AParserHint.inSource(MyScanner, fmt, params);
  MyLog.write([ThisHint]);
  ThisHint.free;
end;

// Output a warning to the log
procedure AParser.warn(const fmt: string; const params: array of const);

var
  // Refers to the warning in question
  ThisWarning: AParserWarning;

begin
  if (MyScanner = nil) or (MyLog = nil) then
    exit;

  ThisWarning := AParserWarning.inSource(MyScanner, fmt, params);
  MyLog.write([ThisWarning]);
  ThisWarning.free;
end;

// Output a syntax error to the log
procedure AParser.syntaxError(const fmt: string; const params: array of const);

var
  // Refers to the error in question
  ThisError: AParserSyntaxError;

begin
  if (MyScanner = nil) or (MyLog = nil) then
    exit;

  ThisError := AParserSyntaxError.inSource(MyScanner, fmt, params);
  MyLog.write([ThisError]);
  ThisError.free;
end;

// Output a fatal error to the log
function AParser.fatalError(const fmt: string;
  const params: array of const): AParserFatalError;

begin
  Result := nil;

  if (MyScanner = nil) or (MyLog = nil) then
    exit;

  Result := AParserFatalError.inSource(MyScanner, fmt, params);
  MyLog.write([Result]);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the scanner used by the parser
function AParser.Scanner: AScanner;

begin
  Result := MyScanner;
end;

// Retrieve a reference to the log used by the parser for output
function AParser.Log: ALog;

begin
  Result := MyLog;
end;

// Retrieve a reference to the source stream being parsed
function AParser.Source: AStream;

begin
  Result := MySource;
end;

// Retrieve a reference to the list of tokens parsed by the parser
function AParser.Tokens: ATokenList;

begin
  Result := MyTokens;
end;
{$ENDIF}


