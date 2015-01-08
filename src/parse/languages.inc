{ languages.inc: Definitions and classes required to define a parsed language.

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
{ This type defines the character types that are recognized by a scanner for
  a parsed language.
}
type ACharacterCategory = (
  { An unrecognized or erroneous character }
  CHARCAT_DUMMY = 0,
  { A character that is not alphanumeric, but which is recognized by the
    language
  }
  CHARCAT_SPECIAL,
  { A character that may begin an identifier or reserved word }
  CHARCAT_LETTER,
  { A character that may begin a numeric constant }
  CHARCAT_DIGIT,
  { A character that may be part of an identifier or reserved word, but which
    may not begin it
  }
  CHARCAT_WORD,
  { A character that may be part of a numeric constant, but which may not
    begin it
  }
  CHARCAT_NUMERIC,
  { A character that is recognized as white space, but which is not an
    indicator that the line or the stream has ended
  }
  CHARCAT_SPACE,
  { A character that may delimit the end of a source line }
  CHARCAT_EOL,
  { A character that may delimit the end of a source stream }
  CHARCAT_EOS,
  { A character that is unrecognized }
  CHARCAT_ERROR
);

const
  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define an invalid, "null" token -- one that has
    not been properly initialized.  This is the initial state of all base
    instances of @link(AToken).
  }
  TOKCAT_DUMMY = 0;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define an identifier, which is a series of
    alphanumeric characters that does not have a pre-defined meaning in the
    syntax of the language being parsed.  Identifiers usually represent
    variables or routines within the source being parsed.
  }
  TOKCAT_IDENTIFIER = $00010000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a keyword, which is a series of alphanumeric
    characters that has a pre-defined meaning in the syntax of the language
    being parsed.  Keywords can represent language constructs, such as a
    conditional statement, or reserved method names.
  }
  TOKCAT_KEYWORD = $00020000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a special token, which may consist of one
    or more characters with pre-defined meaning in the syntax of the language.
    Special tokens often represent operators that are represented with a symbol,
    or delimiters that separate identifiers or which indicate the continuation
    (or end) of a line.
  }
  TOKCAT_SPECIAL = $00030000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a valid numeric constant.  If the token
    has been parsed by the base instance of @link(ANumericConstantToken), and
    the token opcode contains this category, then it means that the sequence of
    characters contained by the token evaluates to a valid number.
  }
  TOKCAT_NUMBER = $00040000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a valid string literal; however, strings are
    NOT parsed by the default scanner and tokenizer methods defined within
    @link(parsing).  This is because there are a variety of ways to define a
    string (such as in Python, where there are strings and docstrings), so it
    is left to the parser to process the string.  However, the token code
    remains valid and can be output into an intermediate code stream.
  }
  TOKCAT_STRING = $00050000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a token that is recognized at whitespace.
    Note that end-of-line and end-of-stream markers are not included in this
    category; these have their own token categories: @link(TOKCAT_EOL) and
    @link(TOKCAT_EOS), respectively.  It is up to the scanner or parser to
    determine whether or not a whitespace token will be processed or ignored;
    some languages, like Python, find whitespace significant.
  }
  TOKCAT_SPACE = $00060000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a sequence of at least one character that
    represents the end of a line in the source being parsed.  If the token has
    been processed by the base instance of @link(ALineEndingTokenFromSource),
    then the token text may contain one or two characters, depending upon
    whether the source file has Windows, Apple, or Unix/Linux-based line
    endings.
  }
  TOKCAT_EOL = $000A0000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a token that represents the end of the
    source stream.  Depending on the language being parsed, there may be an
    actual sequence of characters that denotes the end of the source; it is
    more likely, however, that a scanner will return this type of token when
    the stream being scanned indicates that the end has been reached.
  }
  TOKCAT_EOS = $001A0000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in ordr to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define a rule, thus allowing instances of
    @link(ASyntaxRule) to contain specific tokens as well as references to
    other rules.
  }
  TOKCAT_RULE = $002A0000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to define an unrecognized or erroneous token.  The
    base instances of @link(ANumericConstantToken) will place themselves in
    this category if the number failed to meet basic checks of validity; this
    allows the parser to complain that something in the source looks like a
    number or string constant, but is not actually one.
  }
  TOKCAT_ERROR = $00FF0000;

  { Each of these constants defines a category of token that is recognized by
    a scanner for a parsed language.  The category code resides in the high word
    of a @link(TOpcode), while the specific internal representation of that
    token occupies the low word.  It is possible to mask out the internal
    representation using @link(TOKCAT_CATMASK), in order to quickly determine
    the type of token represented by a specific opcode.

    This constant is used to mask out all but the bits which identify the
    category into which the opcode falls.  One simply @code(AND)s the
    @link(TOpcode) with this value; the resulting value will be one of the
    category constants.
  }
  TOKCAT_CATMASK = $FFFF0000;

// Default syntax rules recognized by a parser
const
  { Rules in @link(ASyntaxRuleset) can be retrieved by an arbitrary name or
    value; however, it is common to define a symbolic constant for each rule so
    that the rule name or value does not have to be hard-coded.  In addition,
    there are some basic rules that will probably be common across all types of
    parsers: rules defining basic token types and indicating which tokens are
    allowed to begin and end a statement.

    This constant is used to define an invalid rule (no rule).
  }
  RULE_NONE = TOKCAT_RULE + $FFFF;

  { Rules in @link(ASyntaxRuleset) can be retrieved by an arbitrary name or
    value; however, it is common to define a symbolic constant for each rule
    so that the rule name or value does not have to be hard-coded.  In addition,
    there are some basic rules that will probably be common across all types of
    parsers: a rules defining basic token types and a rule to indicate which
    tokens are allowed to begin and end a statement.

    This constant is used to define the tokens that may begin a statement.  It
    is not automatically used by the base instance of @link(AParsedLanguage),
    but is provided here as a way to standardize the way in which one of the
    more fundamental rules of a parsed language is defined.
  }
  RULE_BEGIN_STATEMENT = TOKCAT_RULE + 1;

  { Rules in @link(ASyntaxRuleset) can be retrieved by an arbitrary name or
    value; however, it is common to define a symbolic constant for each rule
    so that the rule name or value does not have to be hard-coded.  In addition,
    there are some basic rules that will probably be common across all types of
    parsers: a rules defining basic token types and a rule to indicate which
    tokens are allowed to begin and end a statement.

    This constant is used to define the tokens that may end a statement.  It
    is not automatically used by the base instance of @link(AParsedLanguage),
    but is provided here as a way to standardize the way in which one of the
    more fundamental rules of a parsed language is defined.
  }
  RULE_END_STATEMENT = RULE_BEGIN_STATEMENT + 1;

// AParsedLanguage *************************************************************
{ This class serves as the basis for defining a language that will be parsed.
  It is designed to provide a single point of reference for a parser to use as
  a source stream is processed; as such, it provides routines to determine the
  category of a given character or token, and a way to look up rules.

  To define a new language to parse, simply definee a descendant of this class
  and implement the methods that are called on construction:
  @link(AParsedLanguage.defineCharacterCategories),
  @link(AParsedLanguage.defineOpcodes), and
  @link(AParsedLanguage.defineRules).
}
type AParsedLanguage = class(AnObject)
  protected
    { Specifies whether or not the language is case aware }
    myCaseAwareness: boolean;
    { Stores the character categories recognized by the language }
    myCharacterCategories: array[CHARCAT_SPECIAL..CHARCAT_EOS] of string;
    { Used to manage the opcodes recognized by the langage }
    MyOpcodes: AnOpcodeDictionary;
    { Used to manage the syntax rules defined by the language }
    MySyntaxRules: ASyntaxRuleset;

    { Define the character categories used by the language.

      This method should be overriden by descendant classes.  It is called
      automatically by the constructor.  Descendant classes should first call
      the inherited routine, then use the method to define which characters
      belong in the various character categories; this allows a tokenizer that
      references the language to determine what type of token to create when a
      given character is encountered in the source.

      The base implementation of this routine first sets all character
      category strings to empty strings.  It then assigns the categories as
      follows:
      @unorderedList(
        @item(@code(CHARCAT_LETTER) is set to @link(plcsTypicalLetter))
        @item(@code(CHARCAT_WORD) is set to @link(plcsTypicalWord))
        @item(@code(CHARCAT_DIGIT) is set to @link(plcsTypicalDigit))
        @item(@code(CHARCAT_NUMERIC) is set to @link(plcsTypicalNumeric))
        @item(@code(CHARCAT_SPACE) is set to @link(plcsTypicalWhitespace))
        @item(@code(CHARCAT_EOL) is set to @link(plcsTypicalEndOfLine))
        @item(@code(CHARCAT_EOS) is set to @link(plcsTypicalEndOfStream))
      )
    }
    procedure defineCharacterCategories; virtual;

    { Define the opcodes used by the language.

      This method should be implemented by descendant classes.  It is called
      automatically by the constructor.  Descendant classes should use the
      method to match token strings with opcodes, usually by calling
      @link(AnOpcodeDictionary.bind) or @link(AnOpcodeDictionary.bindSeveral)
      on @code(Self.Opcodes).  This allows
      @link(AParsedLanguage.opcodeFor) to match a given token string with its
      internal representation.
    }
    procedure defineOpcodes; virtual; abstract;

    { Define the rules used by the language.

      This method should be overriden by descendant classes.  It is called
      automatically by the constructor.  Descendant classes should use the
      method to establish syntax rules for the language.  This allows
      @link(AParsedLanguage.SyntaxRule) to return the rule required by a
      parser that needs to determine if a specific token is allowed to be
      positioned where it is found in the source.

      The base implementation of this routine creates two default, empty rules:
      @link(RULE_BEGIN_STATEMENT) and @link(RULE_END_STATEMENT).  If these two
      rules are desired in a given language implementation, then descendant
      classes should call the inherited routine before installing their own
      rules.
    }
    procedure defineRules; virtual;

  public
    { Construct a new instance of the language.  You will likely need only one
      instance of a given language at any given time.

      The constructor calls the inherited routine, after which it calls three
      initialization routines: @link(AParsedLanguage.defineCharacterCategories),
      @link(AParsedLanguage.defineOpcodes), and
      @link(AParsedLanguage.defineRules).  These three methods should be
      implemented by descendant classes to define the character categories,
      opcodes, and rules used by the language.
    }
    constructor new; override;

    { Initializer }
    function init: boolean; override;

    { Destroy the language instance.

      This method is called automatically when @code(TObject.free) is called
      on the language instance.  It frees the syntax ruleset and opcode
      dictionary used by the language before calling the inherited routine.
    }
    destructor destroy; override;

    { Retrieve the category to which the given character belongs.

      This method searches for an instance of @code(ch) in the various
      character category definitions provided by
      @link(AParsedLanguage.myCharacterCategories).  If a match is found, the
      appropriate character category is returned.  If no match is found for the
      given character, then this method returns
      @link(ACharacterCategory CHARCAT_ERROR).

      @code(ch) is passed as a string so that the language definition can
      support both UTF-8 and ASCII characters.
    }
    function categoryOf(const ch: string): ACharacterCategory; virtual;

    { Determine whether the specified character is defined in one of the
      specified character categories.

      This method searches for an instance of @code(ch) in the character
      category definitions specified by @code(categoryList).  The first
      category that has the specified character is returned.  If none of the
      categories specified contains @code(ch), then this method returns
      @link(ACharacterCategory CHARCAT_DUMMY).

      @code(ch) is passed as a string so that the language definition can
      support both UTF-8 and ASCII characters.
    }
    function characterIn(const ch: string;
      const categoryList: array of ACharacterCategory): ACharacterCategory;
      virtual;

    { Determine whether the given token string has a matching opcode; that is,
      whether the given token string is a recognized keyword, operator, or
      special character.

      This method searches the opcode dictionary defined for the language and,
      if @code(tokenString) is found, returns the matching opcode.  Otherwise,
      it returns @link(TOKCAT_DUMMY).

      If @link(AParsedLanguage.caseAware) is @true, then @code(tokenString)
      is converted to lower case before it is checked.  Otherwise, the token
      string is checked exactly as provided by the calling routine.
    }
    function opcodeFor(const tokenString: string): TOpcode; virtual;

    // Properties --------------------------------------------------------------
    { Determine whether or not the language is case aware.

      This method checks the value of @link(AParsedLanguage.myCaseAwareness),
      which is set by the language instance when it is constructed.  If the
      language is case aware, then token strings passed to
      @code(AParsedLanguage.opcodeFor) much exactly match the case specified
      for each keyword, operator, or special character when the language was
      defined.  Otherwise, the token strings are converted to lower case before
      they are checked.
    }
    function caseAware: boolean;

    { Retrieve all characters defined by the language for the given category. }
    function characterCategory(const category: ACharacterCategory): string;
      virtual;

    { Define the characters which belong in the given category.

      This method overwrites any previous characters defined for the specified
      category with the @code(newSet) specified.  Although character category
      types are normally specified when the language instance is created
      (by @link(AParsedLanguage.defineCharacterCategories), this method makes it
      possible to amend or replace these definitions on-the-fly.

      @return(The previous characters defined for the given category, if any.)
    }
    function setCharacterCategory(const category: ACharacterCategory;
      const newSet: string): string; virtual;

    { Retrieve a reference to the dictionary of opcodes maintained by the
      language.  This dictionary is used to match token strings to their
      internal representations for faster parsing and processing.  These
      token strings will represent reserved words, operators, and special
      characters recognized by the parser for the given language.

      The caller may use the reference returned by this method to search the
      dictionary directly, but the reference should NOT be freed.  That will
      be done automatically when the language instance is itself freed.
    }
    function Opcodes: AnOpcodeDictionary;

    { Retrieve a reference to the given syntax rule, which specifies one or
      more opcodes that are to be used or ignored by the parser for the
      language in various circumstances.

      The caller may use the reference returned by this method to search the
      rule directly, but the reference should NOT be freed.  That will be done
      automatically when the language instance is itself freed.
    }
    function SyntaxRule(const rule: TSortKey): ASyntaxRule;

    { Retrieve a reference to the list of syntax rules maintained by the
      language definition.  These rules specify lists of opcodes that the
      parser should process or ignore in various circumstances.

      The caller may use the reference returned by this method to manipulate the
      list directly, but the reference should NOT be freed.  That will be done
      automatically when the language instance is itself freed.
    }
    function SyntaxRules: ASyntaxRuleset;
end;

{ This type refers to the class definition for all instances of
  @link(AParsedLanguage) and its descendants.  It allows generic parsers to
  be defined which can accept any valid instance of @link(AParsedLanguage).
  It also allows parsers for a given language to construct their own
  language definition instances, simplifying the steps required to parse a given
  language.
}
type AParsedLanguageClass = class of AParsedLanguage;

const
  { This constant defines the sequence of characters which makes up typical
    Windows-style line endings.  It is used in the tokenizer source and can
    also be used by language definitions to specify those characters which are
    considered line endings.
  }
  LINE_ENDING_WINDOWS: string = #13#10;

  { This constant the defines the character which makes up a typical Unix-style
    line ending.  It can be used by language definitions to specify those
    characters which are considered line endings.
  }
  LINE_ENDING_UNIX: string = #13;

  { This constant defines the character which makes up a typical Apple-style
    line ending.  It can be used by language definitions to specify those
    characters which are considered line endings.
  }
  LINE_ENDING_APPLE: string = #10;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string defines the characters which are typically allowed to begin
    identifiers in various languages.  It is defined here for convenience,
    so that other language specifications may reference it.
  }
  plcsTypicalLetter = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' +
    'abcdefghijklmnopqrstuvwxyz_';

  { This string defines the characters which are typically allowed to be part
    of an identifier, though they may not begin it, in various languages.  It
    is defined here for convenience, so that other language specifications may
    reference it.
  }
  plcsTypicalWord = '0123456789';

  { This string defines the characters which are typically allowed to begin
    numeric constants in various languages.  It is defined here for convenience,
    so that other language specifications may reference it.
  }
  plcsTypicalDigit = '0123456789';

  { This string defines the characters which are typically allowed to be part
    of a numeric constant, though they may not begin it, by various languages.
    It is defined here for convenience, so that other language specifications
    may reference it.
  }
  plcsTypicalNumeric = 'xXeE';

  { This string defines the characters which are typically counted as whitespace
    by various languages.  It is defined here for convenience, so that other
    language specifications may reference it.
  }
  plcsTypicalWhitespace = #0#1#2#3#4#5#6#7#8#9#11#12#14#15#16#17#18#19#20 +
    #21#22#23#24#25#27#28#29#30#31#32#127;

  { This string defines the characters which are typically counted as line
    ending characters by various languages.  It is defined here for
    convenience, so that other language specifications may reference it.
  }
  plcsTypicalEndOfLine = #10#13;

  { This string defines the characters which are typically used to mark the end
    of a stream by various languages.  It is defined here for convenience, so
    that other language specifications may reference it.
  }
  plcsTypicalEndOfStream = #26;
{$ENDIF}
{$IFDEF ReadImplementation}

{$IFDEF DEBUG}
const
  charCategoryNames: array[CHARCAT_DUMMY..CHARCAT_ERROR] of string = (
    'CHARCAT_DUMMY', 'CHARCAT_SPECIAL', 'CHARCAT_LETTER',
    'CHARCAT_DIGIT', 'CHARCAT_WORD',  'CHARCAT_NUMERIC',
    'CHARCAT_SPACE', 'CHARCAT_EOL', 'CHARCAT_EOS',
    'CHARCAT_ERROR'
  );
{$ENDIF}

// Constructor
constructor AParsedLanguage.new;

begin
  // Call the inherited routine
  inherited new;

  // Call the initialization routines
  Self.defineCharacterCategories;
  Self.defineOpcodes;
  Self.defineRules;
end;

// Initializer
function AParsedLanguage.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myCaseAwareness := false;

    // Establish an opcodes dictionary
    MyOpcodes := AnOpcodeDictionary.new;
    // Establish a syntax ruleset
    MySyntaxRules := ASyntaxRuleset.new;

    result := (MyOpcodes <> nil) and (MySyntaxRules <> nil);
  end;
end;

// Destructor
destructor AParsedLanguage.destroy;

begin
  // Free the syntax ruleset
  if MySyntaxRules <> nil then
    MySyntaxRules.free;

  // Free the opcodes dictionary
  if MyOpcodes <> nil then
    MyOpcodes.free;

  inherited destroy;
end;

// Initialize the list of character categories
procedure AParsedLanguage.defineCharacterCategories;

var
  // Loop counter
  i: ACharacterCategory;

begin
  // Loop through and set all character category strings
  for i := System.low(myCharacterCategories) to
    System.high(myCharacterCategories) do
    // First, clear all of them
    myCharacterCategories[i] := '';

  // Now, establish some typicals
  myCharacterCategories[CHARCAT_LETTER] := plcsTypicalLetter;
  myCharacterCategories[CHARCAT_WORD] := plcsTypicalWord;
  myCharacterCategories[CHARCAT_DIGIT] := plcsTypicalDigit;
  myCharacterCategories[CHARCAT_NUMERIC] := plcsTypicalNumeric;
  myCharacterCategories[CHARCAT_SPACE] := plcsTypicalWhitespace;
  myCharacterCategories[CHARCAT_EOL] := plcsTypicalEndOfLine;
  myCharacterCategories[CHARCAT_EOS] := plcsTypicalEndOfStream;
end;

// Define basic rules common to a majority of langutes
procedure AParsedLanguage.defineRules;

begin
  // Add the rule which is used to determine which tokens begin statements
  MySyntaxRules.addNewRule(RULE_BEGIN_STATEMENT, []);
  // Add the rule which is used to determine which tokens end statements
  MySyntaxRules.addNewRule(RULE_END_STATEMENT, []);
end;

// Retrieve the category to which the given character belongs
function AParsedLanguage.categoryOf(const ch: string): ACharacterCategory;

var
  // Loop counter
  i: ACharacterCategory;

begin
  result := CHARCAT_ERROR;

  if ch = '' then
    exit;

  // Loop through each category string
  for i := System.low(myCharacterCategories) to
    System.high(myCharacterCategories) do
  begin

  {$IFDEF DEBUG}
    writeln(Self.className, '.categoryOf(): Checking for character "', ch, '"',
      ' in ', charCategoryNames[i]);
  {$ENDIF}

    // See if the character exists in the string
    if Charstring.positionOf(ch, myCharacterCategories[i]) > 0 then
    begin
      result := i;
      break;
    end;
  end;
end;

// Determine if the given character is in one of the given categories
function AParsedLanguage.characterIn(const ch: string;
  const categoryList: array of ACharacterCategory): ACharacterCategory;

var
  // Loop counter
  i: longword;

begin
  result := CHARCAT_DUMMY;

  if (ch = '') or (System.length(categoryList) = 0) then
    exit;

  // Loop through each category string
  for i := 0 to System.high(categoryList) do
  begin

  {$IFDEF DEBUG}
    writeln(Self.className, '.characterIn(): Checking for character "', ch, '"',
      ' in ', charCategoryNames[categoryList[i]]);
  {$ENDIF}

    // See if the character exists in the given string
    if Charstring.positionOf(ch, myCharacterCategories[categoryList[i]]) > 0 then
    begin
      result := categoryList[i];
      break;
    end;
  end;
end;

// Retrieve the opcode that matches the given token string
function AParsedLanguage.opcodeFor(const tokenString: string): TOpcode;

begin
  { If the language is case aware, then pass the token string unchanged }
  if myCaseAwareness then
    result := MyOpcodes.valueOf(tokenString)

  else
    result := MyOpcodes.valueOf(Charstring.toLowercase(tokenString));
end;

// Properties ------------------------------------------------------------------
// Determine whether or not the language is case aware
function AParsedLanguage.caseAware: boolean;

begin
  result := myCaseAwareness;
end;

// Retrieve the characters in the specified category
function AParsedLanguage.characterCategory(const category: ACharacterCategory): string;

begin
  result := myCharacterCategories[category];
end;

// Set the characters in the specified category
function AParsedLanguage.setCharacterCategory(const category: ACharacterCategory;
  const newSet: string): string;

begin
  result := myCharacterCategories[category];
  myCharacterCategories[category] := newSet;
end;

// Retrieve the opcode dictionary maintained by the language
function AParsedLanguage.Opcodes: AnOpcodeDictionary;

begin
  Result := MyOpcodes;
end;

// Retrieve a reference to the given syntax rule
function AParsedLanguage.SyntaxRule(const rule: TSortKey): ASyntaxRule;

begin
  Result := MySyntaxRules.Rule(rule);
end;

// Retrieve a reference to all rules defined for the language
function AParsedLanguage.SyntaxRules: ASyntaxRuleset;

begin
  Result := MySyntaxRules;
end;

{$ENDIF}
