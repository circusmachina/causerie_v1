{ statements.inc: A class to represent a single statement which is parsed from a
    source file and a class to represent a block of such statements.

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
// AStatement ******************************************************************
{ This class represents a single statement that is parsed from a source stream.
  It is, in effect, a miniature parser that is constructed by the main parser
  in order to process a single statement.
}
type AStatement = class(AParser)
  protected
    { Refers to the parent parser: the parser that constructed this one }
    MyParent: AParser;
    { If the statement is part of a larger block, this will refer to the parent
      block
    }
    MySuperblock: AStatement;

  public
    { Construct a new statement parser that will use the same scanner and token
      list as the specified parser.

      @code(ThisParser) may be an instance of @link(AParser) or
      @link(AStatement).  If the former, then it is assumed that this statement
      stands alone -- that is, that it is not part of a larger block such as
      a loop statement or function body.  In such cases, @code(ThisParser)
      becomes the value of the @link(AStatement.Parent Parent) property for the
      new instance.

      If @code(ThisParser) is an instance of @link(AStatement), then it is
      assumed that the new statement is part of a larger block.  In such cases,
      @code(ThisParser) becomes the value of the
      @link(AStatement.Superblock Superblock) property of the new instance,
      while @code(ThisParser)'s @link(AStatement.Parent Parent) property
      also becomes the value of the new statement's @code(Parent) property.

      It may sound more complicated than it really is.  In short: if you
      are constructing a new statement from inside an instance of
      @link(AStatement), then you will pass @code(Self) as the value of
      @code(ThisParser).  Otherwise, you will pass a reference the appropriate
      instance of @link(AParser).
    }
    constructor fromSource(const ThisParser: AParser); virtual;

    { Initializer }
    function init: boolean; override;

    { Free the statement.

      This method is declared in order to prevent the default parser
      destructor code from attempting to free certain references, which are only
      borrowed by the statement from its parent.  If those references were
      freed, the parent parser would no longer function correctly.
    }
    destructor destroy; override;

    { Parse the statement.

      This method should be overridden by descendant classes to do the work of
      parsing the current statement from the source.  The base implementation
      simply flags a syntax error regarding an unexpected token and returns.

      @returns(The total number of syntax errors that have occurred while
      parsing.  The base implementation of this method always returns 1.)
    }
    function parse: int64; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the parent parser, from which the statement
      derives its various properties.

      The reference returned by this method should NOT be freed by the caller.
    }
    function Parent: AParser; virtual;

    { Retrieve a reference to the superblock, if any.

      If this statement is part of a compound statement (a statement block),
      then the reference returned by this routine will point to the statement
      which represents the entire block.  The caller should NOT free this
      reference.

      If this statement stands outside of any block, as it might for certain
      interpreted languages, then the value returned by this routine is @nil.
    }
    function Superblock: AStatement; virtual;
end;

// AStatementBlock *************************************************************
{ This class represents a compound statement, or statement block, which is made
  up of one or more statements.  It is commonly used to represent block-level
  statements in various languages, such as the statements inside of a loop,
  the body of a function, and so forth.
}
type AStatementBlock = class(AStatement)
  protected
    { Refers to the most recent statement parsed from the block }
    MyCurrentStatement: AStatement;
    { Refers to the previous statement parsed from the block }
    MyPreviousStatement: AStatement;

  public
    { Initializer }
    function init: boolean; override;

    { Free the statement block.

      This method frees both @link(AStatementBlock.CurrentStatement) and
      @link(AStatementBlock.PreviousStatement) before calling the inherited
      routine.
    }
    destructor destroy; override;

    { Parse statements until the specified opcode is reached.

      This method may be called on the block to parse statements from the
      source until a specific keyword or delimiter is reached.  It will most
      commonly be used to process function bodies, which are usually terminated
      by a keyword or delimiter of some kind.

      Internally, this method simply loops by calling
      @link(AStatementBlock.parse) and then checking the current token when
      that method returns.  If the end of the stream is reached (and it is not
      the opcode desired), then this method will flag a fatal error.

      @raises(AParserFatalError if the end of the source stream is encountered
      before @code(thisOpcode) is found -- unless @code(thisOpcode) is
      @link(TOKCAT_EOS).)

      @returns(The total number of syntax errors that have occurred while
      parsing.  This is the accumulated total from successive calls to
      @link(AStatementBlock.parse).)
    }
    function parseUntil(const thisOpcode: TOpcode): int64; virtual; overload;

    { Parse statements until a token from the specified rule is encountered.

      This method may be called on the block to parse statements from the
      source until one of a number of opcodes is reached.  It will most
      commonly be used to process loops, which are normally terminated by a
      keyword, delimiter, or expression.

      Internally, this method simply loops by calling
      @link(AStatementBlock.parse) and then checking the current token when
      that method returns.  If the end of the stream is reached (and it is not
      part of the specified rule), then this method will flag a fatal error.

      @raises(AParserFatalError if the end of the source stream is encountered
      before @code(thisOpcode) is found -- unless @code(thisOpcode) is
      @link(TOKCAT_EOS).)

      @returns(The total number of syntax errors that have occurred while
      parsing.  This is the accumulated total from successive calls to
      @link(AStatementBlock.parse).)
    }
    function parseUntil(const ThisRule: ASyntaxRule): int64; virtual; overload;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the most recent statement parsed from the block.

      The calling routine should NOT attempt to free the reference returned by
      this routine.
    }
    function CurrentStatement: AStatement; virtual;

    { Retrieve a reference to the previous statement parsed from the block.

      The calling routine should NOT attempt to free the reference returned by
      this routine.
    }
    function PreviousStatement: AStatement; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AStatement ******************************************************************
// Constructor
constructor AStatement.fromSource(const ThisParser: AParser);

begin
  inherited new;

  { Is the parent a parser (this statement stands alone) or a statement (this
    statement is part of a larger block)?
  }
  if ThisParser is AStatement then
  begin
    MyParent := AStatement(ThisParser).Parent;
    MySuperblock := AStatement(ThisParser);
  end

  else
    MyParent := ThisParser;

  if MyParent <> nil then
  begin
    // Inherit the necessary properties
    MyScanner := ThisParser.Scanner;
    MyLog := ThisParser.Log;
    MySource := ThisParser.Source;
    MyTokens := ThisParser.Tokens;
  end;
end;

// Initializer
function AStatement.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyParent := nil;
    MySuperblock := nil;
  end;
end;

// Destructor
destructor AStatement.destroy;

begin
  { We @nil everything here because we don't want the inherited code to attempt
    to free it.  We are only borrowing the references that we use here and the
    parent will still need them.
  }
  MyParent := nil;
  MyScanner := nil;
  MyLog := nil;
  MySource := nil;

  inherited destroy;
end;

// Parse the statement
function AStatement.parse: int64;

begin
  // If this is the first token in the source, then CurrentToken may be @nil
  if CurrentToken = nil then
    NextToken;

  // Descendant classes should override this method to prevent this error
  syntaxError(parsSyntaxUnexpectedToken, [CurrentToken.toString]);
  result := 1;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the parent parser
function AStatement.Parent: AParser;

begin
  Result := MyParent;
end;

// Retrieve a reference to the superblock, if any
function AStatement.Superblock: AStatement;

begin
  Result := MySuperblock;
end;

// AStatementBlock *************************************************************
// Initializer
function AStatementBlock.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyCurrentStatement := nil;
    MyPreviousStatement := nil;
  end;
end;

// Destructor
destructor AStatementBlock.destroy;

begin
  // Free our current statement, if any
  if MyCurrentStatement <> nil then
    MyCurrentStatement.free;

  // Free our previous statement, if any
  if MyPreviousStatement <> nil then
    MyPreviousStatement.free;

  inherited destroy;
end;

// Parse statements until a specific opcode is reached
function AStatementBlock.parseUntil(const thisOpcode: TOpcode): int64;

begin
  result := 0;

  repeat
    // Parse the next statement
    inc(result, Self.parse);

    // Get the next token after the end of the statement
    NextToken;

    // Flag an unexpected end-of-stream
    if (CurrentToken.opcode = TOKCAT_EOS) and (thisOpcode <> TOKCAT_EOS) then
      raise FatalError(parsFatalUnexpectedEOS, []);
  until (CurrentToken.opcode = thisOpcode);
end;

// Parse statements until a token from the given rule is reached
function AStatementBlock.parseUntil(const ThisRule: ASyntaxRule): int64;

begin
  result := 0;

  // We can do nothing without a valid rule
  if ThisRule = nil then
    exit;

  repeat
    // Parse the next statement
    inc(result, Self.parse);

    // Get the next token after the end of the statement
    NextToken;

    // Flag an unexpected end-of-stream
    if (CurrentToken.opcode = TOKCAT_EOS) and
      (ThisRule.has(TOKCAT_EOS) = false) then
      raise FatalError(parsFatalUnexpectedEOS, []);
  until CurrentToken.isPartOf(ThisRule);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the current statement
function AStatementBlock.CurrentStatement: AStatement;

begin
  Result := MyCurrentStatement;
end;

// Retrieve a reference to the previous statement
function AStatementBlock.PreviousStatement: AStatement;

begin
  Result := MyPreviousStatement;
end;

{$ENDIF}
