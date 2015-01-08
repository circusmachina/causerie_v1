{ parsedlanguageparsers.inc: A class for parsing source code using a language
    definition.

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
{ This class represents a parser that is used to process source code of some
  kind using a parsed language definition.  It extends @link(AParser) to support
  processing source using a language definition provided by an instance of
  @link(AParsedLanguage) and provides several utility routines that are useful
  for parsers that process source using a language definition.

  Instead of directly creating an instance of this class, you will probably
  derive a class from it and then create an instance of that, since calling
  @link(AParsedLanguageParser.parse) on a direct instance of this class does not
  do much that is useful.

  When constructing an instance of @link(AParsedLanguageParser), you are
  expected to provide:
  @unorderedList(
    @item(a language definition)
  )

  as well as the items required by @link(AParser) before calling @code(parse).
}
type AParsedLanguageParser = class(AParser)
  protected
    { Refers to the language used to tokenize the source }
    MyLanguage: AParsedLanguage;

  public
    { Construct a new parser that will inherit the properties of its parent
      parser.

      This method builds on the behavior inherited from
      @code(AParser.forSource): it ensures that the new parser also inherits
      the value of it @code(ThisParent)'s @link(AParsedLanguageParser.Language).
    }
    constructor forSource(const ThisParent: AParser); overload; override;

    { Initializer }
    function init: boolean; override;

    { Destroy the parser.

      This method frees @link(AParsedLanguageParser.Language) and then calls
      the inherited routine.
    }
    destructor destroy; override;

    { Parse the source.

      This method builds on the behavior inherited from @link(AParser.parse).
      It calls the inherited routine and, if that routine returns zero (0) to
      indicate all is well, it checks to determine whether
      @link(AParsedLanguageParser.Language) is @nil.

      For classes that override this method, the generally-accepted rule is
      that, although @link(AParser.CurrentToken Self.CurrentToken)
      @italic(should) refer to the token which ended the given statement when
      this method exits, the actual process of resynchronizing the parser to
      ensure that is done by @link(AParsedLanguageParser.parseUntil).  If you
      would rather do so in this method, you will need to override
      @link(AParsedLanguageParser.parseUntil), if you use it, as you will
      otherwise the confuse the parser.

      @return(A negative value if an error has occurred before any parsing
      was done.  This will most likely occur if @link(AParser.Source),
      @link(AParser.Log), or @link(AParsedLanguageParser.Language) are invalid
      (@nil) pointers.  A positive value indicates the number of non-fatal
      errors that occurred while parsing.  A value of zero (0) indicates all is
      well.)
    }
    function parse: int64; override;

    { Parse tokens or statements until one of the tokens in the specified rule
      is encountered.

      This method looks up the rule identified by @code(thisRule) by
      calling @link(AParsedLanguage.SyntaxRule) on
      @link(AParsedLanguageParser.Language Self.Language).  If the
      specified rule is found, then this method loops, calling
      @link(AParsedLanguageParser.parse Self.parse) repeatedly until a token
      from the specified rule is encountered.

      Each time through the loop, after
      @link(AParsedLanguageParser.parse Self.parse) returns, this method
      checks to see if the end of the stream has been reached.  If it has, and
      @link(TOKCAT_EOS) is not part of @code(thisRule), then this method will
      raise a fatal error to indicate that the end of the stream was not
      supposed to be encountered.  Otherwise, the routine calls
      @link(AParsedLanguageParser.resyncTo Self.resyncTo), passing
      @link(RULE_END_STATEMENT) to ensure that the end of the statement has
      actually been reached.  It then calls
      @link(AParser.NextToken Self.NextToken) and loops again.

      When this method exits,
      @link(AParser.CurrentToken AParsedLanguageStatementBlock.CurrentToken)
      should refer to a token from @code(thisRule) or the end of the stream.

      @raises(AParserFatalError if the end of the source stream is encountered
      before a token from @code(thisRule) is found -- unless
      @link(TOKCAT_EOS) is part of @code(thisRule).)

      @return(The total number of errors that occurred while parsing.)
    }
    function parseUntil(const thisRule: TSortKey): int64; overload; virtual;

    { Resynchronize the parser to one of the tokens in the specified
      @code(rule).

      This method is designed to allow the parser to recover after a syntax
      error.  It logs a syntax error to indicate that the current token is
      not expected, then looks up the specified @code(rule) in the list of
      syntax rules specified by @link(AParsedLanguageParser.Language).  If the
      rule is found, it calls @link(AParser.resyncTo), passing the rule
      reference to that routine.

      If the specified @code(rule) is not found, then this routine does nothing.
    }
    procedure resyncTo(const rule: TSortKey); virtual; overload;

    { Skip tokens until one of the tokens in the specified @code(rule) is
      encountered.

      This method looks up the specified @code(rule) in the list of syntax
      rules specified by @link(AParsedLanguageParser.Language).  If the rule is
      found, it calls @link(AParser.skipTo), passing the rule reference to
      that routine.  If the specified @code(rule) is not found, then this
      routine does nothing.

      If @code(silenceInterveningTokens) is @true, then all tokens encountered
      before @code(opcode) will have their @code(silenced) property set by a
      call to @link(AToken.setSilenced).  Although @link(AParsedLanguageParser)
      does not make use of this behavior, descendant classes may prevent
      silenced tokens from being written to their intermediate code streams.
    }
    procedure skipTo(const rule: TSortKey;
      const silenceInterveningTokens: boolean = true); overload; virtual;

    { Skip over tokens until one is encountered that is not a part of the
      specified @code(rule).

      This method looks up the specified @code(rule) in the list of syntax
      rules specified by @link(AParsedLanguageParser.Language).  If the rule is
      found, it calls @link(AParser.skipOver), passing the rule reference to
      that routine.  If the specified @code(rule) is not found, then this
      routine does nothing.

      If @code(silenceInterveningTokens) is @true, then all tokens encountered
      before @code(opcode) will have their @code(silenced) property set by a
      call to @link(AToken.setSilenced).  Although @link(AParsedLanguageParser)
      does not make use of this behavior, descendant classes may prevent
      silenced tokens from being written to their intermediate code streams.
    }
    procedure skipOver(const rule: TSortKey;
      const silenceInterveningTokens: boolean = true); overload; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the language used to parse the source.

      The caller should NOT free the reference returned by this routine.  That
      will be done by the parser itself when it is freed.
    }
    function Language: AParsedLanguage; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Constructor
constructor AParsedLanguageParser.forSource(const ThisParent: AParser);

begin
  inherited forSource(ThisParent);

  if ThisParent is AParsedLanguageParser then
    MyLanguage := AParsedLanguageParser(ThisParent).Language;
end;

// Initializer
function AParsedLanguageParser.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLanguage := nil;
  end;
end;

// Destructor
destructor AParsedLanguageParser.destroy;

begin
  // Free items if we have no parent
  if MyParent = nil then
  begin
    // Free the language definition, if any
    if MyLanguage <> nil then
      freeAndNil(MyLanguage);
  end;

  inherited destroy;
end;

// Parse the source
function AParsedLanguageParser.parse: int64;

begin
  // Call the inherited routine first
  result := inherited parse;

  if result = 0 then
  begin
    // Check to see whether or not we have a valid language
    if MyLanguage = nil then
    begin
      // We don't -- yet -- but we can inherit from our parent
      if (MyParent <> nil) and (MyParent is AParsedLanguageParser) then
        MyLanguage := AParsedLanguageParser(MyParent).Language

      // Otherwise, we indicate an error
      else
        result := -1;
    end;
  end;
end;

// Parse tokens until a token from the specified rule is encountered
function AParsedLanguageParser.parseUntil(const thisRule: TSortKey): int64;

var
  // Refers to the rule in question
  Rule: ASyntaxRule;

begin
  result := -1;

  Rule := MyLanguage.SyntaxRule(thisRule);

  if Rule = nil then
    exit;

  result := 0;

  // If this is the first token in the source, then CurrentToken may be @nil
  if CurrentToken = nil then
    NextToken;

  while not(CurrentToken.isPartOf(Rule)) do
  begin
    // Parse the next statement
    result := result + Self.parse;

    // Was the end of the stream encountered?
    if CurrentToken.opcode = TOKCAT_EOS then
    begin
      // Was it expected?
      if Rule.has(TOKCAT_EOS) then
        break

      else
        raise FatalError(parsFatalUnexpectedEOS, []);
    end;

    // Is the current token the one sough
    if CurrentToken.isPartOf(Rule) then
      break;

    // Resync to the end of the statement
    //Self.resyncTo(RULE_END_STATEMENT);

    // Get the next token after the end of the statement
    NextToken;
  end;
end;

// Resync to the specified rule
procedure AParsedLanguageParser.resyncTo(const rule: TSortKey);

begin
  if MyLanguage <> nil then
    resyncTo(MyLanguage.SyntaxRule(rule));
end;

// Skip to one of the tokens in the specified rule
procedure AParsedLanguageParser.skipTo(const rule: TSortKey;
  const silenceInterveningTokens: boolean = true);

begin
  if MyLanguage <> nil then
    skipTo(MyLanguage.SyntaxRule(rule), silenceInterveningTokens);
end;

// Skip over the tokens in the specified rule
procedure AParsedLanguageParser.skipOver(const rule: TSortKey;
  const silenceInterveningTokens: boolean = true);

begin
  if MyLanguage <> nil then
    skipOver(MyLanguage.SyntaxRule(rule), silenceInterveningTokens);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the language used to parse the source
function AParsedLanguageParser.Language: AParsedLanguage;

begin
  Result := MyLanguage;
end;

{$ENDIF}

