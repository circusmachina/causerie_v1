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

  as well as the items required by @link(AParser) before calling
  @code(parse).
}
type AParsedLanguageParser = class(AParser)
  protected
    { Refers to the language used to tokenize the source }
    MyLanguage: AParsedLanguage;

  public
    { Initializer }
    function init: boolean; override;

    { Parse the source.

      This method builds on the behavior inherited from @link(AParser.parse).
      It calls the inherited routine and, if that routine returns zero (0) to
      indicate all is well, it checks to determine whether
      @link(AParsedLanguageParser.Language) is @nil.

      @return(A negative value if an error has occurred before any parsing
      was done.  This will most likely occur if @link(AParser.Source),
      @link(AParser.Log), or @link(AParsedLanguageParser.Language) are invalid
      (@nil) pointers.  A positive value indicates the number of non-fatal
      errors that occurred while parsing.  A value of zero (0) indicates all is
      well.)
    }
    function parse: int64; override;

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
    procedure resyncTo(const rule: TSortKey); virtual;

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
    procedure resyncTo(const rule: string); virtual;

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
      const silenceInterveningTokens: boolean = true); virtual;

    { Skip tokens until one of the tokens in the specified @code(rule) is
      encountered.

      This method looks up the specified @code(rule) in the list of syntax
      rules specified by @link(AParsedLanguageParser.Language).  If the rule is
      found, it calls @link(AParser.skipTo), passing the rule reference to that
      routine.  If the specified @code(rule) is not found, then this routine
      does nothing.

      If @code(silenceInterveningTokens) is @true, then all tokens encountered
      before @code(opcode) will have their @code(silenced) property set by a
      call to @link(AToken.setSilenced).  Although @link(AParsedLanguageParser)
      does not make use of this behavior, descendant classes may prevent
      silenced tokens from being written to their intermediate code streams.
    }
    procedure skipTo(const rule: string;
      const silenceInterveningTokens: boolean = true); virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the language used to parse the source.

      The caller should NOT free the reference returned by this routine.  That
      will be done by the parser itself when it is freed.
    }
    function Language: AParsedLanguage; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Initializer
function AParsedLanguageParser.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLanguage := nil;
  end;
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
      result := -1;
  end;
end;

// Resync to the specified rule
procedure AParsedLanguageParser.resyncTo(const rule: TSortKey);

begin
  if MyLanguage <> nil then
    inherited resyncTo(MyLanguage.SyntaxRule(rule));
end;

// Resync to the specified rule
procedure AParsedLanguageParser.resyncTo(const rule: string);

begin
  if MyLanguage <> nil then
    inherited resyncTo(MyLanguage.SyntaxRule(rule));
end;

// Skip to one of the tokens in the specified rule
procedure AParsedLanguageParser.skipTo(const rule: TSortKey;
  const silenceInterveningTokens: boolean = true);

begin
  if MyLanguage <> nil then
    inherited skipTo(MyLanguage.SyntaxRule(rule), silenceInterveningTokens);
end;

// Skip to one of the tokens in the specified rule
procedure AParsedLanguageParser.skipTo(const rule: string;
  const silenceInterveningTokens: boolean = true);

begin
  if MyLanguage <> nil then
    inherited skipTo(MyLanguage.SyntaxRule(rule), silenceInterveningTokens);
end;
{$ENDIF}

