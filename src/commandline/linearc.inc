{ linearc.inc - Definitions for a command-line agrument "language".

  This file is part of commandline.pp, a libray for robust command-line parsing.

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
// The opcodes recognized by the language
const
  lcopPlus               = TOKCAT_SPECIAL + (ord('+') shl 8);
  lcopMinus              = TOKCAT_SPECIAL + (ord('-') shl 8);
  lcopTimes              = TOKCAT_SPECIAL + (ord('*') shl 8);
  lcopDivide             = TOKCAT_SPECIAL + (ord('/') shl 8);
  lcopExponent           = TOKCAT_SPECIAL + (ord('^') shl 8);
  lcopAssign             = TOKCAT_SPECIAL + (ord('=') shl 8);

  lcopAnd                = TOKCAT_KEYWORD + (ord('a') shl 8);

  lckwFalse              = TOKCAT_KEYWORD + (ord('f') shl 8);

  lcopModulo             = TOKCAT_KEYWORD + (ord('m') shl 8);

  lcopNot                = TOKCAT_KEYWORD + (ord('n') shl 8);
  lckwNoShort            = lcopNot + 1;
  lckwNoFull             = lckwNoShort + 1;

  lcopOr                 = TOKCAT_KEYWORD + (ord('o') shl 8);
  lckwOn                 = lcopOr + 1;
  lckwOff                = lckwOn + 1;

  lcopShl                = TOKCAT_KEYWORD + (ord('s') shl 8);
  lcopShr                = lcopShl + 1;

  lckwTrue               = TOKCAT_KEYWORD + (ord('t') shl 8);

  lckwYesShort           = TOKCAT_KEYWORD + (ord('y') shl 8);
  lckwYesFull            = lckwYesShort + 1;

  lcdlBeginLongOption    = lcopMinus + 1;
  lcdlBeginShortOption   = lcdlBeginLongOption + 1;

  lcdlQuote1             = TOKCAT_SPECIAL + (ord('"') shl 8);
  lcdlQuote2             = TOKCAT_SPECIAL + (ord('''') shl 8);
  lcdlLeftParen          = TOKCAT_SPECIAL + (ord('(') shl 8);
  lcdlRightParen         = TOKCAT_SPECIAL + (ord(')') shl 8);
  lcdlBackslash          = TOKCAT_SPECIAL + (ord('\') shl 8);

  { This constant represents the total number of opcodes defined for the
    LinearC language.
  }
  lcOpcodeCount = 27;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to begin an expression, which in LinearC consists of those tokens
    encountered after the assignment operator.
  }
  RULE_BEGIN_EXPRESSION = RULE_END_STATEMENT + 1;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to end an expression.
  }
  RULE_END_EXPRESSION   = RULE_BEGIN_EXPRESSION + 1;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to represent a @true value when setting
    @link(ACommandLineSwitch switch options).
  }
  RULE_TRUTH            = RULE_END_EXPRESSION + 1;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to represent a @false value when setting
    @link(ACommandLineSwitch switch options).
  }
  RULE_FALSEHOOD        = RULE_TRUTH + 1;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to represent unary operators, which may appear at the forefront of
    an expression.
  }
  RULE_UNARY_OPERATORS  = RULE_FALSEHOOD + 1;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to represent operators, which may appear between two expressions.
  }
  RULE_OPERATOR = RULE_UNARY_OPERATORS + 1;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to delimit string literals.
  }
  RULE_DELIMIT_STRING = RULE_OPERATOR + 1;

  { This constant represents one of the rules defined by
    @link(TheLinearCLanguage.defineRules); in this case, the tokens that are
    allowed to begin a path.
  }
  RULE_BEGIN_PATH = RULE_DELIMIT_STRING + 1;

// TheLinearCLanguage **********************************************************
{ This class represents the language specification for a command-line
  "language" named LinearC.  It is designed to be a singleton, which is why it
  its class name begins with "the" instead of "a".
}
type TheLinearCLanguage = class(AParsedLanguage)
  protected
    { Define the character categories used by the language.

      This method first calls
      @link(AParsedLanguage.defineCharacterCategories the inherited routine),
      then sets up the following character categories:
      @unorderedList(
        @item(@code(CHARCAT_SPECIAL) is set to @link(lccsSpecial))
        @item(@code(CHARCAT_DIGIT) is set to the combined values of
        @link(plcsTypicalDigit) and @link(lccsDigit))
      )

      The other character categories are left as they have been assigned by the
      inherited routine.
    }
    procedure defineCharacterCategories; override;

    { Define the opcodes used by the language.

      This method calls @link(AnOpcodeDictionary.bindSeveral) to bind the
      values of @code(lcTokenStrings) to @code(lcOpcodes).  Among other things,
      this action causes:
      @unorderedList(
        @item(the value of @link(clopParameterNameDelimiter) to be assigned to
          @link(lcdlBeginLongOption))
        @item(the value of @link(clopParameterAbbreviationDelimiter) to be
          assigned to @link(lcdlBeginShortOption1))
        @item(the value of @link(clopAssignmentOperator) to be assigned to
          @link(lcopAssign))
      )
      This ensures that the values used to describe the operation of
      command-line parameters to the user actually perform as described.
    }
    procedure defineOpcodes; override;

    { Define the rules used by the language.

      This method calls @link(AParsedLanguage.defineRules the inherited routine)
      first, then adds a few rules pertinent to the language:
      @unorderedList(
        @item(@link(RULE_BEGIN_EXPRESSION), which defines the tokens that may
        begin an expression (tokens encountered after the assignment operator))
        @item(@link(RULE_END_EXPRESSION), which defines the tokens that may
        end an expression)
        @item(@link(RULE_TRUTH), which defines those tokens that may represent
        a value of @true when setting @link(ACommandLineSwitch switch options))
        @item(@link(RULE_FALSEHOOD), which defines those tokens that may
        represent a value of @false when setting
        @link(ACommandLineSwitch switch options))
      )

      The method also establishes tokens for the rules defined by the inherited
      routine: @unorderedList(
        @item(@link(RULE_BEGIN_STATEMENT), which defines the tokens that may
        begin a statement (a single command-line parameter))
        @item(@link(RULE_END_STATEMENT), which defines the tokens that may
        end a statement)
      )
    }
    procedure defineRules; override;

  public
    // Initializer
    function init: boolean; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  // Character sets used by the language
  { This string specifies the characters that LinearC recognizes as
    alphanumeric.
  }
  lccsLetter             = '?/\';
  { This string specifies the characters that LinearC recognizes as operators
    or delimiters.
  }
  lccsSpecial            = '+-*/!=''"^()\';

  { This string specifies the characters that LinearC recognizes as beginning
    a numeric constant.  The characters here are added to those defined by
    @link(plcsTypicalDigit) when
    @link(TheLinearCLanguage.defineCharacterCategories) is called.
  }
  lccsDigit              = '.$';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwTrueString                   = 'true';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwFalseString                  = 'false';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwOnString                     = 'on';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwOffString                    = 'off';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwYesShortString                    = 'y';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwYesFullString                    = 'yes';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwNoShortString                    = 'n';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwNoFullString                     = 'no';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopPlusString                   = '+';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopMinusString                  = '-';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopTimesString                  = '*';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopDivideString                 = '/';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopModuloString                 = 'mod';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopExponentString               = '^';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopShlString                    = 'shl';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopShrString                    = 'shr';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopAndString                    = 'and';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopOrString                     = 'or';

  { The LinearC parser allows boolean options to be defined as NOT true or
    NOT false.  To do so, the language must define a token which represents
    a boolean NOT operation.  This string defines that token.
  }
  lcopNotString                    = 'not';

  { In order to parse strings, the LinearC parser must know when one begins and
    ends.  Since strings are usually delimited by quotation marks, this string
    defines one such token.
  }
  lcdlQuote1String                 = '"';

  { In order to parse strings, the LinearC parser must know when one begins and
    ends.  Since strings are usually delimited by quotation marks, this string
    defines one such token.
  }
  lcdlQuote2String                 = '''';

  { In order to parse compound expressions, the LinearC parser must know when
    one expression begins and ends.  Since subexpressions are usually contained
    within parentheses, this string defines one such token.
  }
  lcdlLeftParenString             = '(';

  { In order to parse compound expressions, the LinearC parser must know when
    one expression begins and ends.  Since subexpressions are usually contained
    within parentheses, this string defines one such token.
  }
  lcdlRightParenString             = ')';

  { In order to parse Windows-style paths, the LinearC parser must know about
    backslash characters.
  }
  lcdlBackslashString              = '\';
{$ENDIF}
{$IFDEF ReadImplementation}
const
  { This array contains the opcodes recognized by the language.  Together with
    @link(lcTokenStrings), it can be used by
    @link(AnOpcodeDictionary.bindSeveral) to associate opcodes recognized by the
    language with their token codes.
  }
  lcOpcodes: array[1..lcOpcodeCount] of TOpcode = (
    lcopPlus, lcOpMinus, lcOpTimes, lcopDivide,
    lcopModulo, lcopExponent,
    lcopShl, lcopShr,
    lcopAnd, lcopOr, lcopNot,
    lcdlBeginLongOption,
    lcdlBeginShortOption,
    lcopAssign,
    lckwTrue, lckwFalse,
    lckwOn, lckwOff,
    lckwYesShort, lckwYesFull,
    lckwNoShort, lckwNoFull,
    lcdlQuote1, lcdlQuote2,
    lcdlLeftParen, lcdlRightParen,
    lcdlBackslash
  );

  { This array contains the token strings recognized by the language.  Together
    with @link(lcOpcodes), it can be used by
    @link(AnOpcodeDictionary.bindSeveral) to associate opcodes recognized by the
    language with their token codes.
  }
  lcTokenStrings: array[1..lcOpcodeCount] of string = (
    lcopPlusString, lcopMinusString, lcopTimesString, lcopDivideString,
    lcopModuloString, lcopExponentString,
    lcopShlString, lcopShrString,
    lcopAndString, lcopOrString, lcopNotString,
    clopParameterNameDelimiter,
    clopParameterAbbreviationDelimiter,
    clopAssignmentOperator,
    lckwTrueString, lckwFalseString,
    lckwOnString, lckwOffString,
    lckwYesShortString, lckwYesFullString,
    lckwNoShortString, lckwNoFullString,
    lcdlQuote1String, lcdlQuote2String,
    lcdlLeftParenString, lcdlRightParenString,
    lcdlBackslashString
  );

// TheLinearCLanguage **********************************************************
// Initializer
function TheLinearCLanguage.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    // The language is case-sensitive
    myCaseAwareness := true;
  end;
end;

// Establish character sets
procedure TheLinearCLanguage.defineCharacterCategories;

begin
  { Call the inherited routine first; this establishes typical character sets }
  inherited defineCharacterCategories;

  myCharacterCategories[CHARCAT_LETTER] := plcsTypicalLetter + lccsLetter;
  myCharacterCategories[CHARCAT_SPECIAL] := lccsSpecial;
  myCharacterCategories[CHARCAT_DIGIT] := plcsTypicalDigit + lccsDigit;
end;

// Define opcodes
procedure TheLinearCLanguage.defineOpcodes;

begin
  // Bind token strings to opcodes
  MyOpcodes.bindSeveral(lcTokenStrings, lcOpcodes);
end;

// Define rules
procedure TheLinearCLanguage.defineRules;

begin
  { Call the inherited routine first; this establishes basic rules }
  inherited defineRules;

  // Define the tokens which may begin a statement
  MySyntaxRules.Rule(RULE_BEGIN_STATEMENT).addSeveral([
    TOKCAT_IDENTIFIER, lcdlBeginLongOption,
    lcdlBeginShortOption, lcopMinus, lcopDivide
  ]);

  // Define the tokens which may end a statement
  MySyntaxRules.Rule(RULE_END_STATEMENT).addSeveral([
    TOKCAT_SPACE, TOKCAT_EOS
  ]);

  // Define the tokens which may begin an expression
  MySyntaxRules.addNewRule(RULE_BEGIN_EXPRESSION, [
    TOKCAT_IDENTIFIER, TOKCAT_NUMBER,
    lcopPlus, lcopMinus, lcdlQuote1, lcdlQuote2,
    lcdlLeftParen, RULE_TRUTH, RULE_FALSEHOOD, RULE_BEGIN_PATH
  ]);

  // Define the tokens which may end an expression
  MySyntaxRules.addNewRule(RULE_END_EXPRESSION, [
    lcdlRightParen, TOKCAT_SPACE, TOKCAT_EOS
  ]);

  // Define the tokens which may represent truth
  MySyntaxRules.addNewRule(RULE_TRUTH, [
    lckwTrue, lckwOn, lckwYesShort, lckwYesFull
  ]);

  // Define the tokens which may represent falsehood
  MySyntaxRules.addNewRule(RULE_FALSEHOOD, [
    lckwFalse, lckwOff, lckwNoShort, lckwNoFull
  ]);

  // Define the tokens that may represent unary operators
  MySyntaxRules.addNewRule(RULE_UNARY_OPERATORS, [
    lcopPlus, lcopMinus, lcopNot
  ]);

  // Define the tokens that may be operators
  MySyntaxRules.addNewRule(RULE_OPERATOR, [
    lcopPlus, lcopMinus, lcopTimes, lcopDivide, lcopModulo, lcopExponent,
    lcopShl, lcopShr, lcopAnd, lcopOr
  ]);

  // Define the tokens that may delimit strings
  MySyntaxRules.addNewRule(RULE_DELIMIT_STRING, [
    lcdlQuote1, lcdlQuote2
  ]);

  // Define the tokens that may begin paths
  MySyntaxRules.addNewRule(RULE_BEGIN_PATH, [
    lcopDivide, lcdlBackslash
  ]);
end;
{$ENDIF}
