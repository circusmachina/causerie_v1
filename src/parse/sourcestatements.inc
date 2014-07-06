{ sourcestatements.inc: Utility classes to represent statements that are
    parsed with various descendants of AParser.

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
// AParsedLanguageStatement ****************************************************
{ This class represents a statement that is parsed using an instance of
  @link(AParsedLanguageParser).  It is defined primarily for convenience,
  since it allows the programmer to refer to the
  @link(AParsedLanguageParser.Language Language) property without having to
  refer to the parent parser.
}
type AParsedLanguageStatement = class(AStatement)
public
  // Properties --------------------------------------------------------------
  { Retrieve a reference to the language used to parse the statement.

    This method simply checks to see whether the
    @link(AStatement.Parent Parent) property refers to an instance of
    @link(AParsedLanguageParser) and, if so, calls
    @link(AParsedLanguageParser.Language) and returns the result.

    The caller should NOT attempt to free the reference returned by this
    routine.
  }
  function Language: AParsedLanguage; virtual;
end;

// AParsedLanguageStatementBlock ***********************************************
{ This class represents a statement block that is parsed using an instance of
  @link(AParsedLanguageParser).  It is defined primarily for convenience,
  since it allows the programmer to refer to the
  @link(AParsedLanguageParser.Language Language) property without having to
  refer to the parent parser.
}
type AParsedLanguageStatementBlock = class(AStatementBlock)
  public
    { Parse statements until one of the tokens in the specified rule is
      encountered.

      This method looks up the rule identified by @code(thisRuleIdentifier) by
      calling @link(AParsedLanguage.SyntaxRule) on
      @link(AParsedLanguageStatementBlock.Language Self.Language).  If the
      specified rule is found, then this method calls
      @link(AStatementBlock.parseUntil Self.parseUntil), passing the rule found.
      Otherwise, this method does nothing and returns 0.
    }
    function parseUntil(const thisRuleIdentifier: TSortKey): int64; virtual;
      overload;

    { Parse statements until one of the tokens in the specified rule is
      encountered.

      This method looks up the rule identified by @code(thisRuleName) by
      calling @link(AParsedLanguage.SyntaxRule) on
      @link(AParsedLanguageStatementBlock.Language Self.Language).  If the
      specified rule is found, then this method calls
      @link(AStatementBlock.parseUntil Self.parseUntil), passing the rule found.
      Otherwise, this method does nothing and returns 0.
    }
    function parseUntil(const thisRuleName: string): int64; virtual; overload;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the language used to parse the statement.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property refers to an instance of
      @link(AParsedLanguageParser) and, if so, calls
      @link(AParsedLanguageParser.Language) and returns the result.

      The caller should NOT attempt to free the reference returned by this
      routine.
    }
    function Language: AParsedLanguage; virtual;
end;

// ASymbolStatement ************************************************************
{ This class represents a statement that is parsed by an instance of
  @link(ASymbolParser).  It is primarily defined for convenience, since it
  allows the programmer to refer to the @link(ASymbolParser.Symbols Symbols)
  and other properties without having to refer to the parent parser.
}
type ASymbolStatement = class(AParsedLanguageStatement)
  public
    // Properties --------------------------------------------------------------
    { Retrieve a reference to the vector of symbol tables maintained by the
      parser.  The reference returned by this routine should NOT be freed by
      the caller.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.Symbols) and
      then returns the result.
    }
    function Symbols: ASymbolTableVector; virtual;

    { Retrieve a reference to the symbol table at the given scope.  The
      reference returned by this routine should NOT be freed by the caller.

      If @code(thisScope) represents a value that is greater than or equal to
      the value of @link(AVector.length ASymbolTableVector.length), then this
      routine will return a reference to the global symbol table.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.SymbolTableAt)
      and then returns the result.
    }
    function SymbolTableAt(const thisScope: TSymbolScope): ASymbolTable;
      virtual;

    { Retrieve a reference to the symbol table that represents the global scope.
      The reference returned by this routine should NOT be freed by the caller.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.GlobalSymbols)
      and then returns the result.
    }
    function GlobalSymbols: ASymbolTable; virtual;

    { Retrieve a reference to the symbol table that represents the current
      scope.  The reference returned by this routine should NOT be freed by the
      caller.

      This method simply calls @link(ASymbolParser.SymbolTableAt) with the
      current value of @link(ASymbolParser.currentScope) and returns the result.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.CurrentSymbols)
      and then returns the result.
    }
    function CurrentSymbols: ASymbolTable; virtual;

    { Retrieve the current scope.

      The value returned by this routine represents the symbol table currently
      in use at the current token position within the source being parsed.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.currentScope)
      and then returns the result.
    }
    function currentScope: TSymbolScope; virtual;
end;

// ASymbolStatementBlock *******************************************************
{ This class represents a statement block that is parsed by an instance of
  @link(ASymbolParser).  It is primarily defined for convenience, since it
  allows the programmer to refer to the @link(ASymbolParser.Symbols Symbols)
  and other properties without having to refer to the parent parser.
}
type ASymbolStatementBlock = class(AParsedLanguageStatementBlock)
  public
    // Properties --------------------------------------------------------------
    { Retrieve a reference to the vector of symbol tables maintained by the
      parser.  The reference returned by this routine should NOT be freed by
      the caller.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.Symbols) and
      then returns the result.
    }
    function Symbols: ASymbolTableVector; virtual;

    { Retrieve a reference to the symbol table at the given scope.  The
      reference returned by this routine should NOT be freed by the caller.

      If @code(thisScope) represents a value that is greater than or equal to
      the value of @link(AVector.length ASymbolTableVector.length), then this
      routine will return a reference to the global symbol table.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.SymbolTableAt)
      and then returns the result.
    }
    function SymbolTableAt(const thisScope: TSymbolScope): ASymbolTable;
      virtual;

    { Retrieve a reference to the symbol table that represents the global scope.
      The reference returned by this routine should NOT be freed by the caller.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.GlobalSymbols)
      and then returns the result.
    }
    function GlobalSymbols: ASymbolTable; virtual;

    { Retrieve a reference to the symbol table that represents the current
      scope.  The reference returned by this routine should NOT be freed by the
      caller.

      This method simply calls @link(ASymbolParser.SymbolTableAt) with the
      current value of @link(ASymbolParser.currentScope) and returns the result.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.CurrentSymbols)
      and then returns the result.
    }
    function CurrentSymbols: ASymbolTable; virtual;

    { Retrieve the current scope.

      The value returned by this routine represents the symbol table currently
      in use at the current token position within the source being parsed.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASymbolParser).  If so, it calls @link(ASymbolParser.currentScope)
      and then returns the result.
    }
    function currentScope: TSymbolScope; virtual;
end;

// ASourceStatement ************************************************************
{ This class represents a statement that is parsed by an instance of
  @link(ASourceParser).  It is primarily defined for convenience, since it
  allows the programmer to refer to the @link(ASourceParser.Output Output)
  and other properties without having to refer to the parent parser.
}
type ASourceStatement = class(ASymbolStatement)
  public
    // Properties --------------------------------------------------------------
    { Retrieve a reference to the output stream used by the parser.

      This is the stream to which syntax-checked code is written and will most
      likely represent an intermediate code stream of some kind.

      The caller should NOT free the reference returned by this routine.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASourceParser).  If so, it calls @link(ASourceParser.Output)
      and then returns the result.
    }
    function Output: AStream; virtual;
end;

// ASourceStatementBlock *******************************************************
{ This class represents a statement that is parsed by an instance of
  @link(ASourceParser).  It is primarily defined for convenience, since it
  allows the programmer to refer to the @link(ASourceParser.Output Output)
  and other properties without having to refer to the parent parser.
}
type ASourceStatementBlock = class(ASymbolStatementBlock)
  public
    // Properties --------------------------------------------------------------
    { Retrieve a reference to the output stream used by the parser.

      This is the stream to which syntax-checked code is written and will most
      likely represent an intermediate code stream of some kind.

      The caller should NOT free the reference returned by this routine.

      This method simply checks to see whether the
      @link(AStatement.Parent Parent) property is an instance of
      @link(ASourceParser).  If so, it calls @link(ASourceParser.Output)
      and then returns the result.
    }
    function Output: AStream; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AParsedLanguageStatement ****************************************************
// Properties ------------------------------------------------------------------
// Retrieve a reference to the language used to parse
function AParsedLanguageStatement.Language: AParsedLanguage;

begin
  Result := nil;

  if MyParent is AParsedLanguageParser then
    Result := AParsedLanguageParser(MyParent).Language;
end;

// AParsedLanguageStatementBlock ***********************************************
// Parse statements until one of the tokens in the specified rule is reached
function AParsedLanguageStatementBlock.parseUntil(const thisRuleIdentifier: TSortKey): int64;

var
  // Refers to the rule in question
  ThisRule: ASyntaxRule;

begin
  result := 0;

  ThisRule := Language.SyntaxRule(thisRuleIdentifier);

  if ThisRule <> nil then
    result := Self.parseUntil(ThisRule);
end;

// Parse statements until one of the tokens in the specified rule is reached
function AParsedLanguageStatementBlock.parseUntil(const thisRuleName: string): int64;

var
  // Refers to the rule in question
  ThisRule: ASyntaxRule;

begin
  result := 0;

  ThisRule := Language.SyntaxRule(thisRuleName);

  if ThisRule <> nil then
    result := Self.parseUntil(ThisRule);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the language used to parse
function AParsedLanguageStatementBlock.Language: AParsedLanguage;

begin
  Result := nil;

  if MyParent is AParsedLanguageParser then
    Result := AParsedLanguageParser(MyParent).Language;
end;

// ASymbolStatement ************************************************************
// Properties ------------------------------------------------------------------
// Retrieve a reference to the symbol table vector used to store symbols
function ASymbolStatement.Symbols: ASymbolTableVector;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).Symbols;
end;

// Retrieve the specified symbol table
function ASymbolStatement.SymbolTableAt(const thisScope: TSymbolScope): ASymbolTable;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).SymbolTableAt(thisScope);
end;

// Retrieve a reference to the global symbol table
function ASymbolStatement.GlobalSymbols: ASymbolTable;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).GlobalSymbols;
end;

// Retrieve a reference to the current symbol table
function ASymbolStatement.CurrentSymbols: ASymbolTable;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).CurrentSymbols;
end;

// Retrieve the current scope
function ASymbolStatement.currentScope: TSymbolScope;

begin
  result := -1;

  if MyParent is ASymbolParser then
    result := ASymbolParser(MyParent).currentScope;
end;

// ASymbolStatementBlock *******************************************************
// Properties ------------------------------------------------------------------
// Retrieve a reference to the symbol table vector used to store symbols
function ASymbolStatementBlock.Symbols: ASymbolTableVector;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).Symbols;
end;

// Retrieve the specified symbol table
function ASymbolStatementBlock.SymbolTableAt(const thisScope: TSymbolScope): ASymbolTable;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).SymbolTableAt(thisScope);
end;

// Retrieve a reference to the global symbol table
function ASymbolStatementBlock.GlobalSymbols: ASymbolTable;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).GlobalSymbols;
end;

// Retrieve a reference to the current symbol table
function ASymbolStatementBlock.CurrentSymbols: ASymbolTable;

begin
  Result := nil;

  if MyParent is ASymbolParser then
    Result := ASymbolParser(MyParent).CurrentSymbols;
end;

// Retrieve the current scope
function ASymbolStatementBlock.currentScope: TSymbolScope;

begin
  result := -1;

  if MyParent is ASymbolParser then
    result := ASymbolParser(MyParent).currentScope;
end;

// ASourceStatement ************************************************************
// Properties ------------------------------------------------------------------
// Retrieve a reference to the output stream
function ASourceStatement.Output: AStream;

begin
  Result := nil;

  if MyParent is ASourceParser then
    Result := ASourceParser(MyParent).Output;
end;

// ASourceStatementBlock *******************************************************
// Properties ------------------------------------------------------------------
// Retrieve a reference to the output stream
function ASourceStatementBlock.Output: AStream;

begin
  Result := nil;

  if MyParent is ASourceParser then
    Result := ASourceParser(MyParent).Output;
end;
{$ENDIF}
