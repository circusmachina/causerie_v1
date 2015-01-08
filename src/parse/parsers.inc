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

  Parsers descend from @link(ANode) because they are designed to be chained
  together: the top-level (program) parser will construct a block-level parser,
  which will itself construct a block-level or statement-level parser, which
  will construct an expression parser -- and so on down the line.  The
  advantages of this approach include readability -- no long chain of
  @code(case) statements -- and flexibility: new additions to the language
  usually require only the addition of a new type of statement- or
  expression-level parser.

  Each time a higher-level parser constructs a lower-level parser, the new
  parser is linked to the @link(ANode.Child Child) property.  The most recent
  lower-level parser created by a given parser can be obtained by calling
  @link(AParser.CurrentElement).

  Note that, although instances of @link(AParser) descend from @link(ANode),
  they do not implement any of the streaming behavior for obvious reasons.
}
type AParser = class(ANode)
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
      const ThisLog: ALog); virtual; overload;

    { Construct a new parser that will inherit the properties of its parent
      parser.

      The new parser will inherit the values of @code(ThisParent)'s
      @link(AParser.Scanner Scanner), @link(AParser.Source Source) and
      @link(AParser.Log Log).
    }
    constructor forSource(const ThisParent: AParser); virtual; overload;

    { Initializer }
    function init: boolean; override;

    { Destroy the parser instance.

      If @link(ANode.Parent Self.Parent) is @nil, then that indicates this is
      a top-level parser, and so this method will free the token list and
      scanner associated with the parser, if any, before calling the inherited
      routine.

      Otherwise, this method first pops its current token and pushes it back
      into the parent's list, so that the parent has the token which ended the
      program element.  Then it @code(nils) the related properties so
      they are not freed (since the parent will still be using them) before
      calling the inherited routine.
    }
    destructor destroy; override;

    { Parse the source.

      The base definition of this method first checks to ensure that
      @link(AParser.Source Self.Source), @link(AParser.Log Self.Log), and
      @link(AParser.Tokens Self.Tokens) are all valid references (not @nil).
      Descendant classes may call this routine before performing their own
      checks and parsing the specified source.

      @return(A negative value if an error has occurred before any parsing
      was done.  This will most likely occur if either @link(AParser.Source) or
      @link(AParser.Log) are invalid (@nil) pointers.  A positive value
      indicates the number of non-fatal errors that occurred while parsing.  A
      value of zero (0) indicates all is well.)
    }
    function parse: int64; virtual;

    { Parse statements until the specified opcode is reached.

      This method may be called on to parse tokens or statements from the
      source until a specific keyword or delimiter is reached.  It will most
      commonly be used to process function bodies, which are usually terminated
      by a keyword or delimiter of some kind.

      Internally, this method simply loops by calling
      @link(AParser.parse Self.parse) and then checking the
      current token when that method returns.  If the end of the stream is
      reached (and it is not the opcode desired), then this method will flag a
      fatal error.

      When this method exits,
      @link(AParser.CurrentToken Self.CurrentToken) should refer to
      @code(thisOpcode) or to the end of the stream.

      @raises(AParserFatalError if the end of the source stream is encountered
      before @code(thisOpcode) is found -- unless @code(thisOpcode) is
      @link(TOKCAT_EOS).)

      @returns(The total number of syntax errors that have occurred while
      parsing.  This is the accumulated total from successive calls to
      @link(AParser.parse Self.parse).)
    }
    function parseUntil(const thisOpcode: TOpcode): int64; overload; virtual;

    { Parse until a token from the specified rule is encountered.

      This method may be called to parse statements or tokens from the
      source until one of a number of opcodes is reached.  It will most
      commonly be used to process loops, which are normally terminated by a
      keyword, delimiter, or expression.

      Internally, this method simply loops by calling
      @link(AParser.parse Self.parse) and then checking the current token when
      that method returns.  If the end of the stream is reached (and it is not
      part of the specified rule), then this method will flag a fatal error.

      When this method exits, @link(AParser.CurrentToken Self.CurrentToken)
      should refer to a token from @code(ThisRule) or the end of the stream.

      @raises(AParserFatalError if the end of the source stream is encountered
      before a token from @code(ThisRule) is found -- unless
      @link(TOKCAT_EOS) is part of @code(ThisRule).)

      @returns(The total number of syntax errors that have occurred while
      parsing.  This is the accumulated total from successive calls to
      @link(AParser.parse Self.parse).)
    }
    function parseUntil(const ThisRule: ASyntaxRule): int64; overload; virtual;

    { Retrieve a reference to the most recent token parsed from the source.

      This method returns a reference to the last token in the token list
      maintained by the parser.  This reference should NOT be freed by the
      caller; that will be done by the token list to which the token belongs.

      If the caller wishes to pop a token from the token list, it should do so
      by calling @link(ATokenList.Pop) on @link(AParser.Tokens).  It is then
      responsible for freeing the token itself.

      If @link(AParser.Tokens Self.Tokens) is @nil, this routine will construct
      a new instance of @link(ATokenList) to hold the tokens parsed by the
      parser.  Additionally, after creating the new list, the routine checks to
      see if  @link(ANode.Parent Self.Parent) is @italic(not) @nil; if not,
      it means that that this parser was been created to parse a sub-element
      (such as a statement block or statement), and this method will push a copy
      of the previous token from its parent's list onto the new list, as well as
      the ACTUAL current token from the parent.
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
    procedure skipTo(const ThisRule: ASyntaxRule;
      const silenceInterveningTokens: boolean = true); virtual;

    { Skip over tokens in the source until one is encountered that does not
      match the specified @code(opcode), or until the end of the stream is
      reached.

      If @code(silenceInterveningTokens) is @true, then all tokens encountered
      until one that is not @code(opcode) will have their @code(silenced)
      property set by a call to @link(AToken.setSilenced).  Although
      @link(AParser) does not make use of this behavior, descendant classes may
      prevent silenced tokens from being written to their intermediate code
      streams.
    }
    procedure skipOverToken(const opcode: TOpcode;
      const silenceInterveningTokens: boolean = true); virtual;

    { Skip over tokens in the source until one is encountered that is not part
      of @code(ThisRule), or until the end of the stream is reached.

      If @code(ThisRule) is @nil, then this method does nothing.

      If @code(silenceInterveningTokens) is @true, then all tokens encountered
      until one that is not @code(opcode) will have their @code(silenced)
      property set by a call to @link(AToken.setSilenced).  Although
      @link(AParser) does not make use of this behavior, descendant classes may
      prevent silenced tokens from being written to their intermediate code
      streams.
    }
    procedure skipOver(const ThisRule: ASyntaxRule;
      const silenceInterveningTokens: boolean = true); virtual;

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
    function Scanner: AScanner; virtual;

    { Retrieve a reference to the log used by the parser for status and error
      output.

      This reference should NOT be freed by the caller.
    }
    function Log: ALog; virtual;

    { Retrieve a reference to the source stream being parsed by the parser.

      This reference should NOT be freed by the caller.
    }
    function Source: AStream; virtual;

    { Retrieve a reference to the list of tokens being managed by the parser.

      This reference should NOT be freed by the caller; that will be done by
      the parser when it is, itself, freed.
    }
    function Tokens: ATokenList; virtual;

    { Retrieve a reference to the most recent program element (a statement,
      statement block, expression) created by this parser for use in parsing the
      source.  Calling this routine is functionally equivalent to casting the
      return value from @link(ANode.Child AParser.Child) to an instance of
      @link(AParser).

      This function may return @nil if the parser implementation does not make
      use of sub-parsers to process statements, statement blocks, or
      expressions.
    }
    function CurrentElement: AParser; virtual;

    { Retrieve a reference to the second most recent program element (a
      statement, statement block, expression) created by this parser for use in
      parsing the source.  Calling this routine is functionally equivalent to
      casting the return value from @link(ANode.Previous AParser.Previous),
      when called on @link(AParser.CurrentElement), to an instance of
      @link(AParser).

      This function may return @nil if the parser implementation does not make
      use of sub-parsers to process statement, statement blocks, or
      expressions -- or if the parser does not cache such program elements, but
      instead disposes of them as soon as they have done their work.
    }
    function PreviousElement: AParser; virtual;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string defines the error message and format used when the end of a
    stream is encountered unexpectedly.

    As defined here, the error message requires no additional parameters.
  }
  parsFatalUnexpectedEOS = 'unexpected end of source';

  { This constant defines the format of the error message logged by instances of
    @link(AParser) when @link(AParser.resyncToToken) and
    @link(AParser.resyncTo) encounter an unexpected token in the source.

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

// Constructor
constructor AParser.forSource(const ThisParent: AParser);

begin
  inherited asChildOf(ThisParent);

  // Inherit the properties of the parent
  if ThisParent <> nil then
  begin
    MySource := ThisParent.Source;
    MyLog := ThisParent.Log;
    MyScanner := ThisParent.Scanner;
  end;
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
    MyTokens := nil;
  end;
end;

// Destructor
destructor AParser.destroy;

begin
  /// If we have a parent...
  if (MyParent <> nil) and (MyParent is AParser) then
  begin
    { Push our current token back to our parent, so that it has the token which
      ended this statement or statement block
    }
    if MyTokens <> nil then
      AParser(MyParent).Tokens.push(MyTokens.Pop);

    MyScanner := nil;
  end;

  // Free the scanner, if any
  if MyScanner <> nil then
    freeAndNil(MyScanner);

  // Free the list of tokens
  if MyTokens <> nil then
    freeAndNil(MyTokens);

  inherited destroy;
end;

// Parse the source - does nothing much here
function AParser.parse: int64;

begin
  result := -1;

  if MyLog <> nil then
    // Add a default counter for errors
    MyLog.Counters.addLeaf(ACounter.named(lgerCounterName));

  if (MySource <> nil) and (MyLog <> nil) then
    result := 0;
end;

// Parse from the source until the specified token is encountered
function AParser.parseUntil(const thisOpcode: TOpcode): int64;

begin
  result := 0;

  // If this is the first token in the source, then CurrentToken may be @nil
  if CurrentToken = nil then
    NextToken;

  while CurrentToken.opcode <> thisOpcode do
  begin
    // Parse the next statement
    result := result + Self.parse;

    // Was the end of the stream encountered?
    if CurrentToken.opcode = TOKCAT_EOS then
    begin
      // Was it expected?
      if thisOpcode = TOKCAT_EOS then
        break

      else
        raise FatalError(parsFatalUnexpectedEOS, []);
    end;

    // Is the current token the one sought?
    if CurrentToken.opcode = thisOpcode then
      break;

    // Get the next token after the end of the statement
    NextToken;
  end;
end;

// Parse from the source until a token in the specified rule is encountered
function AParser.parseUntil(const ThisRule: ASyntaxRule): int64;

begin
  result := 0;

  // We can do nothing without a valid rule
  if ThisRule = nil then
    exit;

  // If this is the first token in the source, then CurrentToken may be @nil
  if CurrentToken = nil then
    NextToken;

  while not(CurrentToken.isPartOf(ThisRule)) do
  begin
    // Parse the next statement
    result := result + Self.parse;

    // Was the end of the stream encountered?
    if CurrentToken.opcode = TOKCAT_EOS then
    begin
      // Was it expected?
      if ThisRule.has(TOKCAT_EOS) then
        break

      else
        raise FatalError(parsFatalUnexpectedEOS, []);
    end;

    // Is the current token the one sought?
    if ThisRule.has(CurrentToken.opcode) then
      break;

    // Get the next token after the end of the statement
    NextToken;
  end;
end;

// Retrieve a reference to the most recent token parsed from the source
function AParser.CurrentToken: AToken;

begin
  // Construct a token list to manage tokens parsed from the source
  if MyTokens = nil then
  begin
    MyTokens := ATokenList.new;

    { If we are a sub-element, such as a statement, then we need a couple of
      tokens
    }
    if (MyParent <> nil) and (MyParent is AParser) then
    begin
      { Push a copy of the parent's previous token (the one before this
        statement) onto our list
      }
      if AParser(MyParent).PreviousToken <> nil then
        MyTokens.push(AToken(AToken.shallowCopyOf(AParser(MyParent).PreviousToken)));

      { Push the ACTUAL current token from our parent (the token that began this
        statement) onto our list
      }
      if AParser(MyParent).CurrentToken <> nil then
        MyTokens.Push(AParser(MyParent).Tokens.Pop);
    end;
  end;

  Result := MyTokens.LastToken;
{$IFDEF DEBUG}
  write(Self.className, '.CurrentToken(): ');
  if Result <> nil then
    writeln(SysUtils.format('%s: %s (%.8X) @%X', [Result.className, Result.toString, Result.opcode, ptrUint(Result)]))
  else
    writeln('NIL');
{$ENDIF}
end;

// Retrieve a reference to the second-most-recent token parsed from the source
function AParser.PreviousToken: AToken;

begin
  Result := nil;

  if MyTokens <> nil then
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
    if Self.CurrentToken <> nil then
      Self.CurrentToken.setSilenced(silenceCurrentToken);

    MyScanner.Next;
    MyTokens.push(MyScanner.CurrentToken);
    Result := MyTokens.LastToken;
  end;

{$IFDEF DEBUG}
  write(Self.className, '.NextToken(): ');
  if Result <> nil then
    writeln(SysUtils.format('%s: %s (%.8X) @%X', [Result.className, Result.toString, Result.opcode, ptrUint(Result)]))
  else
    writeln('NIL');
{$ENDIF}
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

// Skip over tokens that match the specified opcode
procedure AParser.skipOverToken(const opcode: TOpcode;
  const silenceInterveningTokens: boolean = true);

begin
  // Ensure we have a token
  if CurrentToken = nil then
    exit;

  while CurrentToken.opcode = opcode do
  begin
    // Check for the end of the source
    if CurrentToken.opcode = TOKCAT_EOS then
      break;

    Self.NextToken(silenceInterveningTokens);
  end;
end;

// Skip over tokens that are in the specified rule
procedure AParser.skipOver(const ThisRule: ASyntaxRule;
  const silenceInterveningTokens: boolean = true);

begin
  // Ensure we have a token and a rule
  if (CurrentToken = nil) or (ThisRule = nil) then
    exit;

  while CurrentToken.isPartOf(ThisRule) do
  begin
    // Check for the end of the source
    if CurrentToken.opcode = TOKCAT_EOS then
      break;

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
  MyLog.print([ThisNote, lineEnding]);
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
  MyLog.print([ThisHint, lineEnding]);
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
  MyLog.print([ThisWarning, lineEnding]);
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
  MyLog.print([ThisError, lineEnding]);
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
  MyLog.print([Result, lineEnding]);
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

// Retrieve the most recent element parsed by the parser
function AParser.CurrentElement: AParser;

begin
  Result := AParser(Self.Child);
end;

// Retrieve the second most recent element parsed by the parser
function AParser.PreviousElement: AParser;

begin
  Result := nil;

  if Self.CurrentElement <> nil then
    Result := AParser(Self.CurrentElement.Previous);
end;

{$ENDIF}


